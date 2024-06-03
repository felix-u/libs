// link: gdi32
// link: user32
// include: windows.h
// include: windowsx.h

static LRESULT window_proc_(HWND window, UINT message, WPARAM w, LPARAM l) {
    g_gfx.last_mouse_x = g_gfx.mouse_x;
    g_gfx.last_mouse_y = g_gfx.mouse_y;
    g_gfx.last_mouse_left_down = g_gfx.mouse_left_down;
    g_gfx.last_mouse_right_down = g_gfx.mouse_right_down;

    switch (message) {
        case WM_MOUSEMOVE: {
            g_gfx.mouse_x = GET_X_LPARAM(l) / g_gfx.scale;
            g_gfx.mouse_y = GET_Y_LPARAM(l) / g_gfx.scale;
        } break;
        case WM_LBUTTONDOWN: g_gfx.mouse_left_down  = true;  break;
        case WM_LBUTTONUP:   g_gfx.mouse_left_down  = false; break;
        case WM_RBUTTONDOWN: g_gfx.mouse_right_down = true;  break;
        case WM_RBUTTONUP:   g_gfx.mouse_right_down = false; break;
        case WM_KEYDOWN: if (w < 128) g_gfx.key_down[w] = true;  break;
        case WM_KEYUP:   if (w < 128) g_gfx.key_down[w] = false; break;
        case WM_CLOSE: DestroyWindow(window); break;
        case WM_DESTROY: PostQuitMessage(0); break;
        default: return DefWindowProc(window, message, w, l);
    }
    return 0;
}

static Gfx *gfx_create(Gfx gfx) {
    g_gfx = gfx;
    WNDCLASS window_class = { 
        .style = CS_CLASSDC, 
        .lpfnWndProc = window_proc_,
        .lpszClassName = g_gfx.name,
        .hCursor = LoadCursor(0, IDC_ARROW),
    };
    RegisterClass(&window_class);
    int windows_swallows_these_pixels_vertically_and_i_should_find_out_why = 39;
    g_gfx.handle = CreateWindow(
        window_class.lpszClassName,
        g_gfx.name,
        WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        g_gfx.width * g_gfx.scale, 
        g_gfx.height * g_gfx.scale + windows_swallows_these_pixels_vertically_and_i_should_find_out_why,
        0, 0, 0, 0
    );
    SetTimer(0, 0, 1000 / g_gfx.fps, 0);
    QueryPerformanceFrequency(&g_gfx.ticks_per_second);
    QueryPerformanceCounter(&g_gfx.last_tick_count);
    return &g_gfx;
}

static void gfx_get_input(void) { /* handled by win32 window proc */ }

static void gfx_main_loop(void (*user_loop_fn)(void)) {
    while (gfx_render()) {
        gfx_get_input();
        user_loop_fn(); 
    }
}

static bool gfx_render(void) {
    gfx_upscale_pixels_to_buf_();

    bool should_exit = false;
    for (MSG message; PeekMessage(&message, 0, 0, 0, PM_REMOVE);) {
        if (message.message == WM_QUIT) should_exit = true;
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    BITMAPINFOHEADER bitmap_info_header = {
        .biSize = sizeof(bitmap_info_header),
        .biWidth = g_gfx.width * g_gfx.scale,
        .biHeight = -g_gfx.height * g_gfx.scale,
        .biPlanes = 1,
        .biBitCount = 32,
    };
    HDC device_ctx = GetDC(g_gfx.handle);
    SetDIBitsToDevice(
        device_ctx,
        0, 0,
        g_gfx.width * g_gfx.scale, 
        g_gfx.height * g_gfx.scale,
        0, 0, 0,
        g_gfx.height * g_gfx.scale,
        g_gfx.screen_buffer,
        (BITMAPINFO *)&bitmap_info_header,
        DIB_RGB_COLORS
    );
    ReleaseDC(g_gfx.handle, device_ctx);

    gfx_update_t_delta();
    return !should_exit;
}

static force_inline u32 gfx_rgb(u32 rgb) {
    return rgb | 0xff000000;
}

static void gfx_update_t_delta(void) {
    QueryPerformanceCounter(&g_gfx.tick_count);
    u64 elapsed_ticks = g_gfx.tick_count.QuadPart - g_gfx.last_tick_count.QuadPart;
    u64 elapsed_micro = (elapsed_ticks * 1000000) / g_gfx.ticks_per_second.QuadPart;
    g_gfx.last_tick_count = g_gfx.tick_count;
    g_gfx.t_delta = (f32)elapsed_micro / 1000.f / 1000.f;
}
