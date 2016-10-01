/*
 * Copyright (C) 2015 Intel Corporation.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <clib-config.h>

#include <clib.h>

#if defined(C_PLATFORM_WINDOWS)
#include <windows.h>
#include <Dbghelp.h>
#endif

#define MAX_BACKTRACE_SIZE 100
struct thread_state
{
    void *addresses[MAX_BACKTRACE_SIZE];
};

static INIT_ONCE init_once;
static c_tls_t tls;
static c_mutex_t symbol_table_lock;
static c_hash_table_t *symbol_table;

struct _c_backtrace
{
    int n_frames;
    void *addresses[];
};

static void
destroy_tls_state_cb(void *tls_data)
{
    c_free(tls_data);
}

BOOL CALLBACK initialize(PINIT_ONCE once, PVOID param, PVOID *ctx)
{
    c_tls_init(&tls, destroy_tls_state_cb);

    c_mutex_init(&symbol_table_lock);

    symbol_table = c_hash_table_new_full(c_direct_hash,
                                         c_direct_equal,
                                         NULL, /* key destroy */
                                         c_free /* value destroy */);

    SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
#ifdef C_DEBUG
    SymSetOptions(SYMOPT_LOAD_LINES);
#endif
    SymInitialize(GetCurrentProcess(), NULL, TRUE);

    return TRUE;
}

void **
c_backtrace(int *n_frames)
{
    struct thread_state *state;

    InitOnceExecuteOnce(&init_once, initialize, NULL, NULL);

    state = c_tls_get(&tls);
    if (C_UNLIKELY(!state)) {
        state = c_malloc(sizeof(*state));
        c_tls_set(&tls, state);
    }

    *n_frames = CaptureStackBackTrace(1, // frames to skip
                                      MAX_BACKTRACE_SIZE,
                                      state->addresses,
                                      NULL); /* hash_out */
    return state->addresses;
}

static void
resolve_symbols(void **addresses, int n_addresses)
{
    DWORD64 displacement64 = 0;
    DWORD displacement = 0;
    SYMBOL_INFO *symbol_info = c_alloca(sizeof(SYMBOL_INFO) + 256);
    IMAGEHLP_LINE64 line_info;
    HANDLE process = GetCurrentProcess();

    symbol_info->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol_info->MaxNameLen = 256;

    line_info.SizeOfStruct = sizeof(line_info);

    for (int i = 0; i < n_addresses; i++) {
        if (SymFromAddr(process, (DWORD64)addresses[i], &displacement64, symbol_info)) {
#ifdef C_DEBUG
            if (SymGetLineFromAddr64(process, (DWORD64)addresses[i], &displacement, &line_info)) {
                c_hash_table_insert(symbol_table,
                                    addresses[i], c_strdup_printf("%p in %s at %s:%d",
                                                                  addresses[i],
                                                                  symbol_info->Name,
                                                                  line_info.FileName,
                                                                  line_info.LineNumber));
            } else
#endif
                c_hash_table_insert(symbol_table,
                                    addresses[i], c_strdup_printf("%p in %s",
                                                                  addresses[i], symbol_info->Name));
        } else
            addresses[i] = c_strdup_printf("%p", addresses[i]);
    }
}

static void
backtrace_get_frame_symbols(void **addresses,
                            char **frames,
                            int n_frames)

{
    void *missing[n_frames];
    int n_missing = 0;
    int i;

    c_mutex_lock(&symbol_table_lock);

    for (i = 0; i < n_frames; i++) {
        frames[i] = c_hash_table_lookup(symbol_table, addresses[i]);
        if (!frames[i])
            missing[n_missing++] = addresses[i];
    }

    if (n_missing) {
        resolve_symbols(missing, n_missing);

        for (i = 0; i < n_frames; i++) {
            if (!frames[i])
                frames[i] = c_hash_table_lookup(symbol_table, addresses[i]);
            if (!frames[i])
                frames[i] = "unknown";
        }
    }

    c_mutex_unlock(&symbol_table_lock);
}

void
c_backtrace_symbols(void **addresses,
                    char **frames,
                    int n_frames)
{
    backtrace_get_frame_symbols(addresses, frames, n_frames);
}

c_backtrace_t *
c_backtrace_new(void)
{
    int n_frames = 0;
    void **addresses = c_backtrace(&n_frames);
    c_backtrace_t *bt = c_malloc(sizeof(*bt) + sizeof(void *) * n_frames);

    memcpy(bt->addresses, addresses, sizeof(void *) * n_frames);
    bt->n_frames = n_frames;

    return bt;
}

void
c_backtrace_free(c_backtrace_t *backtrace)
{
    c_free(backtrace);
}

int
c_backtrace_get_n_frames(c_backtrace_t *bt)
{
    return bt->n_frames;
}

void
c_backtrace_get_frame_symbols(c_backtrace_t *bt,
                              char **frames,
                              int n_frames)
{
    backtrace_get_frame_symbols(bt->addresses, frames, n_frames);
}

void
c_backtrace_log(c_backtrace_t *bt,
                c_log_context_t *lctx,
                const char *log_domain,
                c_log_level_flags_t log_level)
{
    char *symbols[bt->n_frames];
    int i;

    c_backtrace_get_frame_symbols(bt, symbols, bt->n_frames);

    for (i = 0; i < bt->n_frames; i++)
        c_print("%s\n", symbols[i]);
}

void
c_backtrace_log_error(c_backtrace_t *bt)
{
    c_backtrace_log(bt, NULL, C_LOG_DOMAIN, C_LOG_LEVEL_ERROR);
}

c_backtrace_t *
c_backtrace_copy(c_backtrace_t *bt)
{
    c_backtrace_t *copy = c_malloc(sizeof(*bt) + sizeof(void *) * bt->n_frames);

    copy->n_frames = bt->n_frames;
    memcpy(copy->addresses, bt->addresses, sizeof(void *) * bt->n_frames);

    return copy;
}
