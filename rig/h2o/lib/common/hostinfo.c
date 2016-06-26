/*
 * Copyright (c) 2015 DeNA Co., Ltd., Kazuho Oku
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#include "h2o/hostinfo.h"

struct st_h2o_hostinfo_getaddr_req_t {
    h2o_multithread_receiver_t *_receiver;
    h2o_hostinfo_getaddr_cb _cb;
    void *cbdata;
    h2o_linklist_t _pending;
    union {
        struct {
            char *name;
            char *serv;
            struct addrinfo hints;
        } _in;
        struct {
            h2o_multithread_message_t message;
            const char *errstr;
            struct addrinfo *ai;
        } _out;
    };
};

static h2o_once_t queue_init_once = H2O_ONCE_INIT;

static struct {
    h2o_mutex_t mutex;
    h2o_cond_t cond;
    h2o_linklist_t pending; /* anchor of h2o_hostinfo_getaddr_req_t::_pending */
    size_t num_threads;
    size_t num_threads_idle;
} queue;

static void init_queue(void)
{
    h2o_mutex_init(&queue.mutex);
    h2o_cond_init(&queue.cond);
    h2o_linklist_init_anchor(&queue.pending);
}

size_t h2o_hostinfo_max_threads = 1;

static void lookup_and_respond(h2o_hostinfo_getaddr_req_t *req)
{
    struct addrinfo *res;

    int ret = getaddrinfo(req->_in.name, req->_in.serv, &req->_in.hints, &res);
    req->_out.message = (h2o_multithread_message_t){};
    if (ret != 0) {
        req->_out.errstr = gai_strerror(ret);
        req->_out.ai = NULL;
    } else {
        req->_out.errstr = NULL;
        req->_out.ai = res;
    }

    h2o_multithread_send_message(req->_receiver, &req->_out.message);
}

static void lookup_thread_main(void *_unused)
{
    h2o_mutex_lock(&queue.mutex);

    while (1) {
        while (!h2o_linklist_is_empty(&queue.pending)) {
            h2o_hostinfo_getaddr_req_t *req = H2O_STRUCT_FROM_MEMBER(h2o_hostinfo_getaddr_req_t, _pending, queue.pending.next);
            h2o_linklist_unlink(&req->_pending);
            h2o_mutex_unlock(&queue.mutex);
            lookup_and_respond(req);
            h2o_mutex_lock(&queue.mutex);
        }
        h2o_cond_wait(&queue.cond, &queue.mutex);
    }

    h2o_mutex_unlock(&queue.mutex);
}

static void create_lookup_thread(void)
{
    h2o_thread_t tid;

    h2o_multithread_create_thread(&tid, lookup_thread_main, NULL);

    ++queue.num_threads;
    ++queue.num_threads_idle;
}

h2o_hostinfo_getaddr_req_t *h2o_hostinfo_getaddr(h2o_multithread_receiver_t *receiver, h2o_iovec_t name, h2o_iovec_t serv,
                                                 int family, int socktype, int protocol, int flags, h2o_hostinfo_getaddr_cb cb,
                                                 void *cbdata)
{
    h2o_hostinfo_getaddr_req_t *req = h2o_mem_alloc(sizeof(*req) + name.len + 1 + serv.len + 1);
    req->_receiver = receiver;
    req->_cb = cb;
    req->cbdata = cbdata;
    req->_pending = (h2o_linklist_t){};
    req->_in.name = (char *)req + sizeof(*req);
    memcpy(req->_in.name, name.base, name.len);
    req->_in.name[name.len] = '\0';
    req->_in.serv = req->_in.name + name.len + 1;
    memcpy(req->_in.serv, serv.base, serv.len);
    req->_in.serv[serv.len] = '\0';
    memset(&req->_in.hints, 0, sizeof(req->_in.hints));
    req->_in.hints.ai_family = family;
    req->_in.hints.ai_socktype = socktype;
    req->_in.hints.ai_protocol = protocol;
    req->_in.hints.ai_flags = flags;

    h2o__hostinfo_getaddr_dispatch(req);

    return req;
}

void h2o__hostinfo_getaddr_dispatch(h2o_hostinfo_getaddr_req_t *req)
{
    h2o_once(&queue_init_once, init_queue);

    h2o_mutex_lock(&queue.mutex);

    h2o_linklist_insert(&queue.pending, &req->_pending);

    if (queue.num_threads_idle == 0 && queue.num_threads < h2o_hostinfo_max_threads)
        create_lookup_thread();

    h2o_cond_signal(&queue.cond);
    h2o_mutex_unlock(&queue.mutex);
}

void h2o_hostinfo_getaddr_cancel(h2o_hostinfo_getaddr_req_t *req)
{
    int should_free = 0;

    h2o_mutex_lock(&queue.mutex);

    if (h2o_linklist_is_linked(&req->_pending)) {
        h2o_linklist_unlink(&req->_pending);
        should_free = 1;
    } else {
        req->_cb = NULL;
    }

    h2o_mutex_unlock(&queue.mutex);

    if (should_free)
        free(req);
}

void h2o_hostinfo_getaddr_receiver(h2o_multithread_receiver_t *receiver, h2o_linklist_t *messages)
{
    while (!h2o_linklist_is_empty(messages)) {
        h2o_hostinfo_getaddr_req_t *req = H2O_STRUCT_FROM_MEMBER(h2o_hostinfo_getaddr_req_t, _out.message.link, messages->next);
        h2o_linklist_unlink(&req->_out.message.link);
        h2o_hostinfo_getaddr_cb cb = req->_cb;
        if (cb != NULL) {
            req->_cb = NULL;
            cb(req, req->_out.errstr, req->_out.ai, req->cbdata);
        }
        if (req->_out.ai != NULL)
            freeaddrinfo(req->_out.ai);
        free(req);
    }
}

static const char *fetch_aton_digit(const char *p, const char *end, unsigned char *value)
{
    size_t ndigits = 0;
    int v = 0;

    while (p != end && ('0' <= *p && *p <= '9')) {
        v = v * 10 + *p++ - '0';
        ++ndigits;
    }
    if (!(1 <= ndigits && ndigits <= 3))
        return NULL;
    if (v > 255)
        return NULL;
    *value = (unsigned char)v;
    return p;
}

int h2o_hostinfo_aton(h2o_iovec_t host, struct in_addr *addr)
{
    union {
        int32_t n;
        unsigned char c[4];
    } value;
    const char *p = host.base, *end = p + host.len;
    size_t ndots = 0;

    while (1) {
        if ((p = fetch_aton_digit(p, end, value.c + ndots)) == NULL)
            return -1;
        if (ndots == 3)
            break;
        if (p == end || !(*p == '.'))
            return -1;
        ++p;
        ++ndots;
    }
    if (p != end)
        return -1;

    addr->s_addr = value.n;
    return 0;
}
