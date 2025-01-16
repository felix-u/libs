// TODO(felix): is vsync enabled by default? if not, let's enable it
// TODO(felix): get delta time

// TODO(felix): these should be in a struct
static V2 window_size;
static f32 dpi_scale;
static V2 mouse_pos;
static bool mouse_left_down, mouse_right_down;
static bool mouse_left_clicked, mouse_right_clicked;

static LRESULT win32_window_proc(HWND window, u32 message, WPARAM w, LPARAM l) {
    switch (message) {
        case WM_CLOSE: DestroyWindow(window); break;
        case WM_DESTROY: PostQuitMessage(0); break;
        case WM_DPICHANGED: {
            f32 dpi_1x = 96.f;
            dpi_scale = (f32)GetDpiForWindow(window) / dpi_1x;
        } break;
        case WM_GETMINMAXINFO: { // TODO(felix): not working. The goal is to not allow the window to become smaller than a certain size
            MINMAXINFO *info = (MINMAXINFO *)l;
            int min_width = 100, min_height = 100;
            info->ptMinTrackSize.x = min_width;
            info->ptMinTrackSize.y = min_height;
        } break;
        case WM_LBUTTONDOWN: mouse_left_clicked = true; mouse_left_down = true; break;
        case WM_LBUTTONUP: mouse_left_clicked = false; mouse_left_down = false; break;
        case WM_MOUSEMOVE: {
            // NOTE(felix): we include windowsx.h just for these two lparam macros. Can we remove that and do something else?
            mouse_pos.x = (f32)GET_X_LPARAM(l);
            mouse_pos.y = (f32)GET_Y_LPARAM(l);
        } break;
        case WM_RBUTTONDOWN: mouse_right_clicked = true; mouse_right_down = true; break;
        case WM_RBUTTONUP: mouse_right_clicked = false; mouse_right_down = false; break;
        case WM_SIZE: {
            window_size.x = (f32)LOWORD(l);
            window_size.y = (f32)HIWORD(l);
        } break;
        case WM_SIZING: {
            // TODO(felix): handle redrawing here so that we're not blocked while resizing
            // NOTE(felix): apparently there might be some problems with this and the better way to do it is for the thread with window events to be different than the application thread that submits draw calls and updates the program
        } break;
    }
    return DefWindowProcA(window, message, w, l);
}

// NOTE(felix): was previously after target view resizing in main loop
// Minimal usage of D3D11 with D2D. Draws 3 coloured squares
/* {
    D3D11_MAPPED_SUBRESOURCE mapped = {0};
    win32_assert_hr(vcalla(device_ctx, Map, (ID3D11Resource *)vbuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped));
    Vertex *vertex = mapped.pData;

    f32 w = (f32)width, h = (f32)height;
    f32 qsize = 400;
    f32 pos[][2] = { { 100.f, 300.f }, { 300.f, 500.f }, { 600.f, 300.f } };
    f32 col[][4] = { { 1.f, 0, 0, 1.f }, { 0, 1.f, 0, 1.f }, { 0, 0, 1.f, 1.f } };
    f32 corner[][2] = { { 0, 0 }, { 0, 1.f }, { 1.f, 1.f }, { 1.f, 0 } };

    for (usize q = 0; q < 3; q += 1) {
        f32 ax = (pos[q][0] + qsize * corner[0][0]) / w * 2.f - 1.f;
        f32 ay = (pos[q][1] + qsize * corner[0][1]) / h * 2.f - 1.f;
        f32 bx = (pos[q][0] + qsize * corner[1][0]) / w * 2.f - 1.f;
        f32 by = (pos[q][1] + qsize * corner[1][1]) / h * 2.f - 1.f;
        f32 cx = (pos[q][0] + qsize * corner[2][0]) / w * 2.f - 1.f;
        f32 cy = (pos[q][1] + qsize * corner[2][1]) / h * 2.f - 1.f;
        f32 dx = (pos[q][0] + qsize * corner[3][0]) / w * 2.f - 1.f;
        f32 dy = (pos[q][1] + qsize * corner[3][1]) / h * 2.f - 1.f;

        vertex[6 * q + 0] = (Vertex){ .x = ax, .y = -ay, .r = col[q][0], .g = col[q][1], .b = col[q][2], .a = col[q][3] };
        vertex[6 * q + 1] = (Vertex){ .x = cx, .y = -cy, .r = col[q][0], .g = col[q][1], .b = col[q][2], .a = col[q][3] };
        vertex[6 * q + 2] = (Vertex){ .x = bx, .y = -by, .r = col[q][0], .g = col[q][1], .b = col[q][2], .a = col[q][3] };
        vertex[6 * q + 3] = (Vertex){ .x = cx, .y = -cy, .r = col[q][0], .g = col[q][1], .b = col[q][2], .a = col[q][3] };
        vertex[6 * q + 4] = (Vertex){ .x = ax, .y = -ay, .r = col[q][0], .g = col[q][1], .b = col[q][2], .a = col[q][3] };
        vertex[6 * q + 5] = (Vertex){ .x = dx, .y = -dy, .r = col[q][0], .g = col[q][1], .b = col[q][2], .a = col[q][3] };
    }
    vcalla(device_ctx, Unmap, (ID3D11Resource *)vbuf, 0);
} */
