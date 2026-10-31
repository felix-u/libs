#define CW_USEDEFAULT ((int)0x80000000)

#define DECLARE_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name
DECLARE_HANDLE(DPI_AWARENESS_CONTEXT);
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((DPI_AWARENESS_CONTEXT)-4)

enum { // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-showwindow
    SW_SHOWDEFAULT = 10,
};

enum { // https://learn.microsoft.com/en-us/windows/win32/winmsg/window-styles
    WS_OVERLAPPED = 0x00000000L,
    WS_MAXIMIZEBOX = 0x00010000L,
    WS_MINIMIZEBOX = 0x00020000L,
    WS_THICKFRAME = 0x00040000L,
    WS_SYSMENU = 0x00080000L,
    WS_CAPTION = 0x00C00000L,

    WS_OVERLAPPEDWINDOW = (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX),

    WS_EX_APPWINDOW = 0x00040000L,
};

#define MAKEINTRESOURCEA(i) ((char *)((unsigned long)((unsigned short)(i))))
#define MAKEINTRESOURCE MAKEINTRESOURCEA
#define IDC_ARROW MAKEINTRESOURCE(32512)

#define WIN32__LOWORD(l) ((unsigned short)(((unsigned long long)(l)) & 0xffff))
#define WIN32__HIWORD(l) ((unsigned short)((((unsigned long long)(l)) >> 16) & 0xffff))
#define WIN32__GET_X_LPARAM(l) ((int)(short)WIN32__LOWORD(l))
#define WIN32__GET_Y_LPARAM(l) ((int)(short)WIN32__HIWORD(l))

typedef struct { long x, y; } POINT;
typedef struct {
    POINT ptReserved;
    POINT ptMaxSize;
    POINT ptMaxPosition;
    POINT ptMinTrackSize;
    POINT ptMaxTrackSize;
} MINMAXINFO;

typedef struct {
    long left;
    long top;
    long right;
    long bottom;
} RECT;
typedef struct {
    unsigned long cbSize;
    RECT rcMonitor;
    RECT rcWork;
    unsigned long dwFlags;
} MONITORINFO;

typedef struct {
    void *hwnd;
    unsigned message;
    unsigned long long wParam;
    long long lParam;
    unsigned long time;
    POINT pt;
    unsigned long lPrivate;
} MSG;

typedef long long (*WNDPROC)(void *window_handle, unsigned message, unsigned long long wparam, long long lparam);
typedef struct {
    unsigned style;
    WNDPROC lpfnWndProc;
    int cbClsExtra;
    int cbWndExtra;
    void *hInstance;
    void *hIcon;
    void *hCursor;
    void *hbrBackground;
    const char *lpszMenuName;
    const char *lpszClassName;
} WNDCLASSA;

META(link "Gdi32")
WIN32_CALL(int) ReleaseDC(void *window_handle, void *device_context_handle);

META(link "User32")
WIN32_CALL(void *)         CreateWindowExA(unsigned long extended_style, const char *class_name, const char *window_name, unsigned long style, int x, int y, int width, int height, void *window_parent_handle, void *menu_handle, void *instance_handle, void *param);
WIN32_CALL(long long)      DefWindowProcA(void *window_handle, unsigned message, unsigned long long wparam, long long lparam);
WIN32_CALL(int)            DestroyWindow(void *window_handle);
WIN32_CALL(long long)      DispatchMessageA(const MSG *);
WIN32_CALL(void *)         GetDC(void *window_handle);
WIN32_CALL(unsigned)       GetDpiForWindow(void *window_handle);
WIN32_CALL(int)            GetMonitorInfoA(void *monitor_handle, MONITORINFO *);
WIN32_CALL(void *)         LoadCursorA(void *instance_handle, const char *cursor_name);
WIN32_CALL(void *)         MonitorFromWindow(void *window_handle, unsigned long flags);
WIN32_CALL(int)            PeekMessageA(MSG *, void *window_handle, unsigned message_filter_min, unsigned message_filter_max, unsigned remove_message);
WIN32_CALL(void)           PostQuitMessage(int exit_code);
WIN32_CALL(unsigned short) RegisterClassA(const WNDCLASSA *);
WIN32_CALL(int)            SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT);
WIN32_CALL(int)            ShowWindow(void *window_handle, int show_command);
WIN32_CALL(int)            TranslateMessage(const MSG *);

static void *win32_create_window(const char *window_name, WNDPROC window_procedure) {
    const char *class_name = window_name;
    WNDCLASSA window_class = {
        .lpfnWndProc = window_procedure,
        .hCursor = LoadCursorA(0, IDC_ARROW),
        .lpszClassName = class_name,
    };
    {
        unsigned short ok = RegisterClassA(&window_class);
        ensure(ok != 0);
    }

    int ok = SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    ensure(ok);

    unsigned long extended_style = WS_EX_APPWINDOW;
    int x = CW_USEDEFAULT, y = CW_USEDEFAULT, width = CW_USEDEFAULT, height = CW_USEDEFAULT;

    void *window = CreateWindowExA(extended_style, class_name, window_name, WS_OVERLAPPEDWINDOW, x, y, width, height, 0, 0, 0, 0);
    ensure(window != 0);

    return window;
}

static void win32_process_messages(bool *should_quit) {
    // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-peekmessagea
    enum { PM_REMOVE = 0x0001 };

    enum { WM_QUIT = 0x0012 };

    for (MSG message = {0}; PeekMessageA(&message, 0, 0, 0, PM_REMOVE);) {
        if (message.message == WM_QUIT) *should_quit = true;
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }
}

static i64 win32_window_procedure(void *window, u32 message, u64 w, i64 l, Platform *platform, f32 window_size[2], f32 mouse_position[2]) {
    App_Frame_Info *frame = &platform->frame;

    enum {
        WM_DESTROY       = 0x0002,
        WM_SIZE          = 0x0005,
        WM_CLOSE         = 0x0010,
        WM_GETMINMAXINFO = 0x0024,
        WM_DISPLAYCHANGE = 0x007E,
        WM_KEYDOWN       = 0x0100,
        WM_KEYUP         = 0x0101,
        WM_MOUSEMOVE     = 0x0200,
        WM_MOUSEWHEEL    = 0x020a,
        WM_LBUTTONDOWN   = 0x0201,
        WM_LBUTTONUP     = 0x0202,
        WM_RBUTTONDOWN   = 0x0204,
        WM_RBUTTONUP     = 0x0205,
        WM_SIZING        = 0x0214,
        WM_DPICHANGED    = 0x02E0,
    };

    App_Key key = 0;
    if (message == WM_KEYDOWN || message == WM_KEYUP) switch (w) { // https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
        case 0x10: key = App_Key_SHIFT; break;
        case 0x11: key = App_Key_CONTROL; break;
        case 0x25: key = App_Key_LEFT; break;
        case 0x26: key = App_Key_UP; break;
        case 0x27: key = App_Key_RIGHT; break;
        case 0x28: key = App_Key_DOWN; break;
        case 0xbb: key = '='; break;
        case 0xbd: key = '-'; break;
        default: {
            bool is_same = ' ' <= w && w <= 'Z';
            if (is_same) {
                key = (App_Key)w;
                break;
            }

            bool is_function_key = 0x70 <= w && w <= 0x87;
            if (is_function_key) {
                key = (App_Key)(w - 0x70 + App_Key_F1);
                break;
            }

            static bool once = false;
            if (!once) {
                log_info("unimplemented translation from win32 keycode <0x%x> (logging ONCE for all keys)", w);
                once = true;
            }
        } break;
    }

    switch (message) {
        case WM_CLOSE: platform->should_quit = true; break;
        case WM_DESTROY: PostQuitMessage(0); break;
        case WM_DPICHANGED: {
            f32 dpi_1x = 96.f;
            frame->dpi_scale = (f32)GetDpiForWindow(window) / dpi_1x;
        } break;
        case WM_GETMINMAXINFO: { // TODO(felix): not working. The goal is to not allow the window to become smaller than a certain size
            MINMAXINFO *info = (MINMAXINFO *)l;
            int min_width = 100, min_height = 100;
            info->ptMinTrackSize.x = min_width;
            info->ptMinTrackSize.y = min_height;
        } break;
        case WM_KEYDOWN: {
            frame->key_pressed[key] = !frame->key_down[key];
            frame->key_down[key] = true;
        } break;
        case WM_KEYUP: {
            frame->key_down[key] = false;
        } break;
        case WM_LBUTTONDOWN: {
            App_Mouse_Button button = App_Mouse_LEFT;
            frame->mouse_clicked[button] = !frame->mouse_down[button];
            frame->mouse_down[button] = true;
        } break;
        case WM_LBUTTONUP: {
            App_Mouse_Button button = App_Mouse_LEFT;
            frame->mouse_down[button] = false;
        } break;
        case WM_MOUSEMOVE: {
            mouse_position[0] = (f32)WIN32__GET_X_LPARAM(l);
            mouse_position[1] = (f32)WIN32__GET_Y_LPARAM(l);
        } break;
        case WM_MOUSEWHEEL: {
            i16 delta = (i16)WIN32__HIWORD(w);
            frame->scroll = (f32)delta / 120.f;
        } break;
        case WM_RBUTTONDOWN: {
            App_Mouse_Button button = App_Mouse_RIGHT;
            frame->mouse_clicked[button] = !frame->mouse_down[button];
            frame->mouse_down[button] = true;
        } break;
        case WM_RBUTTONUP: {
            App_Mouse_Button button = App_Mouse_RIGHT;
            frame->mouse_down[button] = false;
        } break;
        case WM_SIZE: {
            window_size[0] = (f32)WIN32__LOWORD(l);
            window_size[1] = (f32)WIN32__HIWORD(l);
        } break;
        case WM_SIZING: {
            // TODO(felix): handle redrawing here so that we're not blocked while resizing
            // NOTE(felix): apparently there might be some problems with this and the better way to do it is for the thread with window events to be different than the application thread that submits draw calls and updates the program
        } break;
        default: break;
    }

    return DefWindowProcA(window, message, w, l);
}
