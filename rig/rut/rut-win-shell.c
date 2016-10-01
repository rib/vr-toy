//#include <windows.h>

#include "rut-win-shell.h"

#include <MMDeviceAPI.h>
#include <AudioClient.h>

#include <uv.h>
#include <clib.h>

static UINT rut_ui_thread_msg_id;

struct _rut_ui_thread_msg {
    enum {
        RUT_UI_THREAD_CREATE_WINDOW,
    } type;
    union {
        struct {
            uv_mutex_t mutex;
            uv_cond_t window_created;
            int width;
            int height;
            HWND hwnd_out;
        } create_window;
    };
};

#define RUT_WM_MSG  (WM_USER + 0)

struct _rut_wm_msg {
    enum {
        RUT_WM_RESIZE,
        RUT_WM_SET_TITLE,
        RUT_WM_SET_CURSOR,
        RUT_WM_SET_FULLSCREEN,
    } type;
    union {
        struct {
            int width;
            int height;
        } resize;
        struct {
            rut_cursor_t cursor;
        } set_cursor;
        WCHAR *title;
        bool set_fullscreen;
    };
};


static WCHAR *utf8_to_utf16_wchar(const char *str)
{
    c_utf16_t *wstr = c_utf8_to_utf16(str,
                                      -1, //len
                                      NULL, // items read
                                      NULL, // items written,
                                      NULL); // don't catch errors
    return (WCHAR *)wstr;
}

struct _ui_thread_window_state
{
    ATOM window_class;
    int width;
    int height;
    rut_shell_onscreen_t *onscreen;
    HWND win;

    bool override_cursor;
    HCURSOR cursor;
    WINDOWPLACEMENT saved_placement;
};


static bool
update_cursor(struct _ui_thread_window_state *state)
{
    /* In the case of the default cusor (override_cursor == false) then Windows
     * will reset the cursor to it's default state for us (assuming a NULL
     * class cursor) and we just need to avoid explicitly setting the cursor
     */
    if (state->override_cursor) {
        SetCursor(state->cursor);
        return true;
    } else
        return false;
}

static void
set_cursor(struct _ui_thread_window_state *state, rut_cursor_t cursor)
{
    if (state->cursor) {
        DestroyCursor(state->cursor);
        state->cursor = NULL;
    }

    state->override_cursor = true;

    switch(cursor) {
    case RUT_CURSOR_DEFAULT:
        state->override_cursor = false;
        break;
    case RUT_CURSOR_INVISIBLE:
        state->cursor = NULL;
        return;
    case RUT_CURSOR_ARROW:
        state->cursor = LoadCursor(NULL, IDC_ARROW);
        break;
    case RUT_CURSOR_IBEAM:
        state->cursor = LoadCursor(NULL, IDC_IBEAM);
        break;
    case RUT_CURSOR_WAIT:
        state->cursor = LoadCursor(NULL, IDC_WAIT);
        break;
    case RUT_CURSOR_CROSSHAIR:
        state->cursor = LoadCursor(NULL, IDC_CROSS);
        break;
    case RUT_CURSOR_SIZE_WE:
        state->cursor = LoadCursor(NULL, IDC_SIZEWE);
        break;
    case RUT_CURSOR_SIZE_NS:
        state->cursor = LoadCursor(NULL, IDC_SIZENS);
        break;
    }

    update_cursor(state);
}
static void
set_fullscreen(struct _ui_thread_window_state *state, bool fullscreen)
{
    HWND hwnd = state->win;
    DWORD style = GetWindowLong(hwnd, GWL_STYLE);

    if (fullscreen) {
        MONITORINFO info = {
            .cbSize = sizeof(info)
        };

        if (!(style & WS_OVERLAPPEDWINDOW))
            return;

        if (!GetWindowPlacement(hwnd, &state->saved_placement)) {
            c_warning("Failed to query window's current placement");
            return;
        }

        if (!GetMonitorInfo(MonitorFromWindow(hwnd,
                            MONITOR_DEFAULTTOPRIMARY), &info)) {
            c_warning("Failed to query default primary monitor associated with window");
            return;
        }

        SetWindowLong(hwnd, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
        SetWindowPos(hwnd, HWND_TOP,
                     info.rcMonitor.left, info.rcMonitor.top,
                     info.rcMonitor.right - info.rcMonitor.left,
                     info.rcMonitor.bottom - info.rcMonitor.top,
                     SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    } else {
        if (style & WS_OVERLAPPEDWINDOW)
            return;

        SetWindowLong(hwnd, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(hwnd, &state->saved_placement);
        SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                     SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
}

static LRESULT
handle_rut_wm_msg(struct _ui_thread_window_state *state,
                  struct _rut_wm_msg *msg)
{
    HWND hwnd = state->win;

    switch (msg->type) {
    case RUT_WM_RESIZE:
        SetWindowPos(hwnd,
                     HWND_TOP, // ignored due to flags
                     0, 0, // x, y ignored due to flags
                     msg->resize.width,
                     msg->resize.height,
                     SWP_NOZORDER|SWP_NOMOVE);
        break;

    case RUT_WM_SET_TITLE:
        SetWindowTextW(hwnd, msg->title);
        break;

    case RUT_WM_SET_CURSOR:
        set_cursor(state, msg->set_cursor.cursor);
        break;

    case RUT_WM_SET_FULLSCREEN:
        set_fullscreen(state, msg->set_fullscreen);
        break;
    }

    return 0;
}
static LRESULT CALLBACK
rig_window_msg_cb(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    struct _ui_thread_window_state *state = (void *)GetWindowLongPtr(hwnd, 0);

    switch (message) {
    case RUT_WM_MSG:
        return handle_rut_wm_msg(state, (struct _rut_wm_msg *)lParam);

    /* Since we leave the class cursor NULL then Windows will automatically
     * call SetCursor whenever the mouse moves to reset the cursor to its
     * default which in-turn results in a WM_SETCURSOR message where
     * we can override the reset if appropriate...
     */
    case WM_SETCURSOR:
        if (update_cursor(state))
            return true;
        else
            return DefWindowProc(hwnd, message, wParam, lParam);
        break;

    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}

static void
create_window(rut_shell_t *shell,
              struct _rut_ui_thread_msg *msg)
{
    struct _ui_thread_window_state *state = c_malloc0(sizeof(*state));

    /* The size of the window passed to CreateWindow for some reason
       includes the window decorations so we need to compensate for
       that */
    int width = msg->create_window.width + GetSystemMetrics(SM_CXSIZEFRAME) * 2;
    int height = msg->create_window.height + (GetSystemMetrics(SM_CYSIZEFRAME) * 2 +
            GetSystemMetrics(SM_CYCAPTION));

    state->win = msg->create_window.hwnd_out =
        CreateWindowW((LPWSTR)MAKEINTATOM(shell->win.window_class),
                      L".",
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT, /* xpos */
                      CW_USEDEFAULT, /* ypos */
                      width,
                      height,
                      NULL, /* parent */
                      NULL, /* menu */
                      GetModuleHandle(NULL),
                      NULL /* lparam for the WM_CREATE message */);

    SetWindowLongPtr(state->win, 0, (LONG_PTR)state);

    state->saved_placement.length = sizeof(state->saved_placement);

    uv_cond_signal(&msg->create_window.window_created);
}


static void
windows_ui_thread_cb(void *user_data)
{
    rut_shell_t *shell = user_data;

    while (1) {
        MSG msg;
        BOOL status = GetMessage(&msg, NULL, 0, 0);

        if (status > 0) {
            if (msg.message == rut_ui_thread_msg_id) {
                struct _rut_ui_thread_msg *ui_msg = (void *)msg.lParam;

                switch (ui_msg->type) {
                    case RUT_UI_THREAD_CREATE_WINDOW:
                        create_window(shell, ui_msg);
                        break;
                }
            } else {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        } else if (status < 0) {
            /* XXX: handle error */
        } else {
            /* XXX: notify main thread of WM_QUIT */
            break;
        }
    }
}

static cg_onscreen_t *
rut_win_allocate_onscreen(rut_shell_onscreen_t *onscreen)
{
    rut_shell_t *shell = onscreen->shell;
    cg_onscreen_t *cg_onscreen;
    cg_error_t *error = NULL;
    struct _rut_ui_thread_msg *msg = c_malloc0(sizeof(*msg));

    cg_onscreen = cg_onscreen_new(shell->cg_device,
                                  onscreen->width,
                                  onscreen->height);

    msg->type = RUT_UI_THREAD_CREATE_WINDOW;

    uv_mutex_init(&msg->create_window.mutex);
    msg->create_window.width = onscreen->width;
    msg->create_window.height = onscreen->height;
    uv_cond_init(&msg->create_window.window_created);

    PostThreadMessage(GetThreadId(shell->win.ui_thread),
                      rut_ui_thread_msg_id,
                      0,
                      (LPARAM)msg);

    uv_mutex_lock(&msg->create_window.mutex);
    uv_cond_wait(&msg->create_window.window_created, &msg->create_window.mutex);
    onscreen->win.hwnd = msg->create_window.hwnd_out;
    uv_mutex_unlock(&msg->create_window.mutex);

    if (onscreen->win.hwnd == NULL) {
        c_error("Unable to create window");
        cg_object_unref(cg_onscreen);
        return NULL;
    }

    cg_win32_onscreen_set_foreign_window(cg_onscreen, onscreen->win.hwnd);

    if (!cg_framebuffer_allocate(cg_onscreen, &error)) {
        c_error("Unable to allocate onscreen framebuffer: %s", error->message);
        cg_error_free(error);
        cg_object_unref(cg_onscreen);
        return NULL;
    }

    return cg_onscreen;
}
static void
rut_win_onscreen_destroy(rut_shell_onscreen_t *onscreen)
{
    //rut_shell_t *shell = onscreen->shell;

    //c_hash_table_remove(shell->win.hwnd_to_onscreen_map, onscreen);
}

void
rut_win_onscreen_resize(rut_shell_onscreen_t *onscreen,
                        int width,
                        int height)
{
    struct _rut_wm_msg msg = {
        RUT_WM_RESIZE,
        .resize = { width, height }
    };
    SendMessage(onscreen->win.hwnd, RUT_WM_MSG, 0, (LPARAM)&msg);
}

static void
rut_win_onscreen_set_title(rut_shell_onscreen_t *onscreen,
                           const char *title)
{
    struct _rut_wm_msg msg = {
        RUT_WM_SET_TITLE,
        .title = utf8_to_utf16_wchar(title)
    };
    SendMessage(onscreen->win.hwnd, RUT_WM_MSG, 0, (LPARAM)&msg);

    c_free(msg.title);
}

static void
rut_win_onscreen_set_cursor(rut_shell_onscreen_t *onscreen,
                            rut_cursor_t cursor)
{
    struct _rut_wm_msg msg = {
        RUT_WM_SET_CURSOR,
        .set_cursor = { cursor }
    };
    SendMessage(onscreen->win.hwnd, RUT_WM_MSG, 0, (LPARAM)&msg);
}

void
rut_win_onscreen_set_fullscreen(rut_shell_onscreen_t *onscreen,
                                bool fullscreen)
{
    struct _rut_wm_msg msg = {
        RUT_WM_SET_FULLSCREEN,
        .set_fullscreen = fullscreen
    };
    SendMessage(onscreen->win.hwnd, RUT_WM_MSG, 0, (LPARAM)&msg);
}

static bool
create_window_class(rut_shell_t *shell)
{
    char *class_name_ascii;
    WCHAR *class_name_wchar;
    WNDCLASSW wndclass;

    class_name_ascii = c_strdup_printf("RigWindow%p", shell);
    class_name_wchar = utf8_to_utf16_wchar(class_name_ascii);

    memset(&wndclass, 0, sizeof(wndclass));
    wndclass.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = rig_window_msg_cb;
    wndclass.cbWndExtra = sizeof(LONG_PTR);
    wndclass.hInstance = GetModuleHandleW(NULL);
    wndclass.hIcon = LoadIconW(NULL, (LPWSTR)IDI_APPLICATION);
    wndclass.hCursor = NULL;
    wndclass.hbrBackground = NULL;
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = class_name_wchar;

    shell->win.window_class = RegisterClassW(&wndclass);

    c_free(class_name_wchar);
    c_free(class_name_ascii);

    if (shell->win.window_class == 0) {
        c_error("Unable to register window class");
        return false;
    }

    return true;
}
static void
rut_win_shell_audio_chunk_init(rut_shell_t *shell, rut_audio_chunk_t *chunk)
{
    chunk->n_channels = shell->pcm_n_channels;
    chunk->channels = shell->pcm_channel_layouts;

    chunk->n_frames = shell->pcm_period_n_frames;
    chunk->data_len = chunk->n_frames * chunk->n_channels * 2; /* assumes 16bit format a.t.m */
    chunk->data = c_malloc0(chunk->data_len);
}

static bool
audio_init(rut_shell_t *shell)
{
#if 0
    HRESULT hresult;
    IMMDeviceEnumerator *pEnumerator = NULL;
    IMMDevice *pDevice = NULL;
    IAudioClient *pAudioClient = NULL;
    IAudioRenderClient *pRenderClient = NULL;
    WAVEFORMATEX *pwfx = NULL;
    UINT32 bufferFrameCount;
    UINT32 numFramesAvailable;
    UINT32 numFramesPadding;
    BYTE *pData;
    DWORD flags = 0;

#warning "may need to initialize COM first?"

    hresult = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL,
                               CLSCTX_ALL, IID_IMMDeviceEnumerator,
                               (void**)&pEnumerator);
    if (FAILED(hresult)) {
        c_warning("Failed to create audio device enumerator");
        return false;
    }

    hresult = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    if (FAILED(hresult)) {
        c_warning("Failed to find an audio end point");
        return false;
    }

    hresult = pDevice->Activate(IID_IAudioClient, CLSCTX_ALL,
                                NULL, (void**)&pAudioClient);
    if (FAILED(hresult)) {
        c_warning("Failed to active select audio end point");
        return false;
    }

    /* XXX: the aim is to be event based using CreateEvent
     * and dev->SetEvent
     *
     * Since this involves using an Event Object which we can't
     * easily attach to the libuv mainloop we can use
     * RegisterWaitForSingleObject(,,,,,WT_EXECUTEINWAITTHREAD))
     * to ask the thread pool to
     * handle the wait for us (this is also what libuv uses internally
     * when it has to deal with event objects). We can then use
     * uv_async_send to trigger a wake up of the libuv mainloop.
     */
#warning "XXX: see if we can ignore the mix format and select S16"
    hresult = audio_client_impl->Initialize(AUDCLNT_SHAREMODE_SHARED,
                                            0,
                                            hnsRequestedDuration,
                                            0,
                                            mix_fmt,
                                            NULL);
    if (FAILED(hresult)) {
        c_warning("Failed to initialize shared mode audio client");
        return false;
    }

        // Tell the audio source which format to use.
    hresult = pMySource->SetFormat(mix_fmt);
    if (FAILED(hresult)) {
        c_warning("Failed to set audio format");
        return false;
    }

    // Get the actual size of the allocated buffer.
    hresult = audio_client_impl->GetBufferSize(&bufferFrameCount);
    if (FAILED(hresult)) {
        c_warning("Failed to query size of audio client buffer");
        return false;
    }

    hresult = audio_client_impl->GetService(IID_IAudioRenderClient,
                                            (void**)&render_client_impl);
    if (FAILED(hresult)) {
        c_warning("Failed to query IAudioRenderClient COM interface");
        return false;
    }

    // Grab the entire buffer for the initial fill operation.
    hresult = pRenderClient->GetBuffer(bufferFrameCount, &pData);
    if (FAILED(hresult)) {
    }

    // Load the initial data into the shared buffer.
    hr = pMySource->LoadData(bufferFrameCount, pData, &flags);
    EXIT_ON_ERROR(hr)

    hr = pRenderClient->ReleaseBuffer(bufferFrameCount, flags);
    EXIT_ON_ERROR(hr)

        // Calculate the actual duration of the allocated buffer.
        hnsActualDuration = (double)REFTIMES_PER_SEC *
        bufferFrameCount / pwfx->nSamplesPerSec;

    hr = audio_client_impl->Start();  // Start playing.
    EXIT_ON_ERROR(hr)
#endif
    int n_channels = 2;
    unsigned int frame_size = n_channels * 2;

#warning "remove hacked audio latency"
    /* Corresponds to ~30ms... */
    shell->pcm_period_n_frames = (shell->pcm_freq * 300) / 1000;
    shell->pcm_buffer_n_frames = shell->pcm_period_n_frames * 2;

    shell->pcm_buf = c_malloc(shell->pcm_period_n_frames * frame_size);

    /* FIXME: very hacky... */
    rut_channel_layout_t *channels = c_new0(rut_channel_layout_t, n_channels);
    for (int i = 0; i < n_channels; i++) {
        channels[i].offset = 2 * i;
        channels[i].stride = 2 * n_channels;
        channels[i].format = RUT_CHANNEL_FORMAT_S16_SYS;
        channels[i].type = i;
    }
    shell->pcm_channel_layouts = channels;
    shell->pcm_n_channels = n_channels;

#if 0
        Exit:
        CoTaskMemFree(pwfx);
        SAFE_RELEASE(pDevice)
        SAFE_RELEASE(audio_client_impl)
        SAFE_RELEASE(pRenderClient)

        return hr;
#endif
    return true;
}

bool
rut_win_shell_init(rut_shell_t *shell)
{
    cg_error_t *error = NULL;

    if (!create_window_class(shell))
        return false;

    shell->cg_renderer = cg_renderer_new();

    shell->cg_device = cg_device_new();

    cg_renderer_set_winsys_id(shell->cg_renderer, CG_WINSYS_ID_WGL);

    //cg_win32_renderer_add_filter(shell->cg_renderer,
    //                             msg_filter_cb,
    //                             shell);

    cg_win32_renderer_set_event_retrieval_enabled(shell->cg_renderer, false);

    cg_device_set_renderer(shell->cg_device, shell->cg_renderer);

    cg_device_connect(shell->cg_device, &error);
    if (!shell->cg_device) {
        c_warning("Failed to setup CGlib device: %s", error->message);
        cg_error_free(error);
        goto error;
    }

    rut_ui_thread_msg_id = RegisterWindowMessage("RigUIMessage");

    uv_thread_create(&shell->win.ui_thread,
                     windows_ui_thread_cb,
                     shell);

    //if (getenv("RIG_USE_HMD")) {
    //    if (!rut_win_check_for_hmd(shell)) {
    //        c_warning("Failed to find a head mounted display");
    //    }
    //}

    //shell->win.hwnd_to_onscreen_map = c_hash_table_new(NULL, NULL);

    //shell->platform.check_for_hmd = rut_win_check_for_hmd;

    shell->platform.allocate_onscreen = rut_win_allocate_onscreen;
    //shell->platform.onscreen_destroy = rut_win_onscreen_destroy;
    shell->platform.onscreen_resize = rut_win_onscreen_resize;
    shell->platform.onscreen_set_title = rut_win_onscreen_set_title;
    shell->platform.onscreen_set_cursor = rut_win_onscreen_set_cursor;
    shell->platform.onscreen_set_fullscreen = rut_win_onscreen_set_fullscreen;
#if 0
    shell->platform.key_event_get_keysym = rut_win_key_event_get_keysym;
    shell->platform.key_event_get_action = rut_win_key_event_get_action;
    shell->platform.key_event_get_modifier_state = rut_win_key_event_get_modifier_state;

    shell->platform.motion_event_get_action = rut_win_motion_event_get_action;
    shell->platform.motion_event_get_button = rut_win_motion_event_get_button;
    shell->platform.motion_event_get_button_state = rut_win_motion_event_get_button_state;
    shell->platform.motion_event_get_modifier_state = rut_win_motion_event_get_modifier_state;
    shell->platform.motion_event_get_transformed_xy = rut_win_motion_event_get_transformed_xy;

    shell->platform.text_event_get_text = rut_win_text_event_get_text;

    shell->platform.free_input_event = rut_win_free_input_event;
#endif

    shell->platform.audio_chunk_init = rut_win_shell_audio_chunk_init;

    return true;

error:

    if (shell->cg_device) {
        cg_object_unref(shell->cg_device);
        shell->cg_device = NULL;
    }
    if (shell->cg_renderer) {
        cg_object_unref(shell->cg_renderer);
        shell->cg_renderer = NULL;
    }

    c_free(shell);

    return false;
}

