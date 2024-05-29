static LRESULT window_proc_(HWND window, UINT message, WPARAM w, LPARAM l) {
    g_gui.last_mouse_x = g_gui.mouse_x;
    g_gui.last_mouse_y = g_gui.mouse_y;
    g_gui.last_mouse_left_down = g_gui.mouse_left_down;
    g_gui.last_mouse_right_down = g_gui.mouse_right_down;

    switch (message) {
        case WM_MOUSEMOVE: {
            g_gui.mouse_x = GET_X_LPARAM(l) / g_gui.scale;
            g_gui.mouse_y = GET_Y_LPARAM(l) / g_gui.scale;
        } break;
        case WM_LBUTTONDOWN: g_gui.mouse_left_down  = true;  break;
        case WM_LBUTTONUP:   g_gui.mouse_left_down  = false; break;
        case WM_RBUTTONDOWN: g_gui.mouse_right_down = true;  break;
        case WM_RBUTTONUP:   g_gui.mouse_right_down = false; break;
        case WM_KEYDOWN: if (w < 128) g_gui.key_down[w] = true;  break;
        case WM_KEYUP:   if (w < 128) g_gui.key_down[w] = false; break;
        case WM_CLOSE: DestroyWindow(window); break;
        case WM_DESTROY: PostQuitMessage(0); break;
        default: return DefWindowProc(window, message, w, l);
    }
    return 0;
}

static Gui *gui_create(Gui gui) {
    g_gui = gui;
    WNDCLASS window_class = { 
        .style = CS_CLASSDC, 
        .lpfnWndProc = window_proc_,
        .lpszClassName = g_gui.name,
        .hCursor = LoadCursor(0, IDC_ARROW),
    };
    RegisterClass(&window_class);
    g_gui.handle = CreateWindow(
        window_class.lpszClassName,
        g_gui.name,
        WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        g_gui.width * g_gui.scale, 
        g_gui.height * g_gui.scale + 39,
        0, 0, 0, 0
    );
    SetTimer(0, 0, 1000 / g_gui.fps, 0);
    QueryPerformanceFrequency(&g_gui.ticks_per_second);
    QueryPerformanceCounter(&g_gui.last_tick_count);
    return &g_gui;
}

static void gui_update_t_delta(void) {
    QueryPerformanceCounter(&g_gui.tick_count);
    u64 elapsed_ticks = g_gui.tick_count.QuadPart - g_gui.last_tick_count.QuadPart;
    u64 elapsed_micro = (elapsed_ticks * 1000000) / g_gui.ticks_per_second.QuadPart;
    g_gui.last_tick_count = g_gui.tick_count;
    g_gui.t_delta = (f32)elapsed_micro / 1000.f / 1000.f;
}

static void gui_upscale_pixels_to_buf_(void) {
    int width = g_gui.width;
    int height = g_gui.height;
    int scale = g_gui.scale;
    int screen_width = width * scale;
    int screen_width_len = sizeof(u32) * screen_width;
    int max_row_i = width * height;

    int screen_row_i = 0;
    for (int row_i = 0; row_i < max_row_i; row_i += width) {
        int max_i = row_i + width;

        int screen_i = screen_row_i;
        for (int i = row_i; i < max_i; i += 1, screen_i += scale) {
            for (int s = 0; s < scale; s += 1) {
                g_gui.screen_buffer[screen_i + s] = g_gui.pixels[i];
            }
        }
        
        u32 *first_screen_row = g_gui.screen_buffer + screen_row_i;
        screen_row_i += screen_width;
        for (int s = 1; s < scale; s += 1, screen_row_i += screen_width) {
            memcpy(g_gui.screen_buffer + screen_row_i, first_screen_row, screen_width_len);
        }
    }
}

static bool gui_render(void) {
    gui_upscale_pixels_to_buf_();

    bool should_exit = false;
    for (MSG message; PeekMessage(&message, 0, 0, 0, PM_REMOVE);) {
        if (message.message == WM_QUIT) should_exit = true;
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    BITMAPINFOHEADER bitmap_info_header = {
        .biSize = sizeof(bitmap_info_header),
        .biWidth = g_gui.width * g_gui.scale,
        .biHeight = -g_gui.height * g_gui.scale,
        .biPlanes = 1,
        .biBitCount = 32,
    };
    HDC device_ctx = GetDC(g_gui.handle);
    SetDIBitsToDevice(
        device_ctx,
        0, 0,
        g_gui.width * g_gui.scale, 
        g_gui.height * g_gui.scale,
        0, 0, 0,
        g_gui.height * g_gui.scale,
        g_gui.screen_buffer,
        (BITMAPINFO *)&bitmap_info_header,
        DIB_RGB_COLORS
    );
    ReleaseDC(g_gui.handle, device_ctx);

    gui_update_t_delta();
    return !should_exit;
}

static bool gui_bounds_check(Gui_Rect *rect) {
    int *x = &rect->x, *y = &rect->y; 
    int *width = &rect->width, *height = &rect->height;

    if (*x > g_gui.width || *x + *width < 0 || *y > g_gui.height || *y + *height < 0) return false;

    if (*x < 0) {
        *width += *x;
        *x = 0;
    } else {
        int width_diff = *x + *width - g_gui.width;
        if (width_diff > 0) *width -= width_diff;
    }

    if (*y < 0) {
        *height += *y;
        *y = 0;
    } else {
        int height_diff = *y + *height - g_gui.height;
        if (height_diff > 0) *height -= height_diff;
    }

    return true;
}

static void gui_draw_rect_no_bounds_check(Gui_Rect rect, u32 colour) {
    int width_len = sizeof(u32) * rect.width;
    int row_i = rect.y * g_gui.width + rect.x;
    int max_row_i = (rect.y + rect.height) * g_gui.width + rect.x;
    
    int max_i = row_i + rect.width;
    for (int i = row_i; i < max_i; i += 1) g_gui.pixels[i] = colour;

    u32 *first_row = g_gui.pixels + row_i;
    row_i += g_gui.width;
    for (; row_i < max_row_i; row_i += g_gui.width) {
        memcpy(g_gui.pixels + row_i, first_row, width_len);
    }
}

static void gui_draw_rect(Gui_Rect rect, u32 colour) {
    if (!gui_bounds_check(&rect)) return;
    gui_draw_rect_no_bounds_check(rect, colour);
}

static void gui_draw_rect_palette(Gui_Rect rect, usize palette_i) {
    if (!gui_bounds_check(&rect)) return;
    gui_draw_rect_no_bounds_check(rect, g_palette[palette_i]);
}

static void gui_draw_sprite_no_bounds_check(u32 *sprite, Gui_V2 dim, Gui_Rect area) {
    gui_draw_no_bounds_check_loop(g_gui.pixels[win_i] = sprite[sprite_i]);
}

static void gui_draw_sprite_palette_no_bounds_check(u32 *sprite, Gui_V2 dim, Gui_Rect area) {
    gui_draw_no_bounds_check_loop(g_gui.pixels[win_i] = g_palette[sprite[sprite_i]]);
}

static void gui_draw_sprite(u32 *sprite, Gui_V2 dim, Gui_Rect area) {
    if (!gui_bounds_check(&area)) return;
    gui_draw_sprite_no_bounds_check(sprite, dim, area);
}

static void gui_draw_sprite_palette(u32 *sprite, Gui_V2 dim, Gui_Rect area) {
    if (!gui_bounds_check(&area)) return;
    gui_draw_sprite_palette_no_bounds_check(sprite, dim, area);
}

static void gui_draw_ascii_3x5(String8 ascii, Gui_V2 pos, u32 colour) {
    gui_draw_ascii_3x5_loop(gui_draw_ascii_char_3x5(c, pos, colour));
}

static void gui_draw_ascii_3x5_palette(String8 ascii, Gui_V2 pos, u32 palette_i) {
    gui_draw_ascii_3x5_loop(gui_draw_ascii_char_3x5(c, pos, g_palette[palette_i]));
}

static void gui_draw_ascii_char_3x5(u8 c, Gui_V2 pos, u32 colour) {
    Gui_Rect area = { .pos = pos, .dim = { 3, 5 } };
    if (!gui_bounds_check(&area)) return;
    gui_draw_ascii_char_3x5_no_bounds_check(c, area, colour);
}

static void gui_draw_ascii_char_3x5_no_bounds_check(u8 c, Gui_Rect area, u32 colour) {
    Gui_V2 dim = { 3, 5 };
    bool *sprite = gui_font_3x5[c];
    gui_draw_no_bounds_check_loop(g_gui.pixels[win_i] = colour);
}

static void gui_clear(void) {
    usize num_bytes = sizeof(u32) * g_gui.width * g_gui.height;
    memset(g_gui.pixels, 0, num_bytes);
}

static void gui_clear_colour(u32 colour) {
    for (int i = 0; i < g_gui.width; i += 1) {
        g_gui.pixels[i] = colour;
    }
    int max_row_i = g_gui.width * g_gui.height;
    int width_len = g_gui.width * sizeof(u32);
    for (int row_i = g_gui.width; row_i < max_row_i; row_i += g_gui.width) {
        memcpy(g_gui.pixels + row_i, g_gui.pixels, width_len);
    }
}

static void gui_clear_palette(usize palette_i) {
    gui_clear_colour(g_palette[palette_i]);
}

static bool gui_is_point_in_rect(Gui_V2 point, Gui_Rect rect) {
    if (point.x < rect.x || rect.x + rect.width  < point.x) return false;
    if (point.y < rect.y || rect.y + rect.height < point.y) return false;
    return true;
}
