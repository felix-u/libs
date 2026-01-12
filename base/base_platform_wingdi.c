#include <windowsx.h>
#include <dwmapi.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

struct Platform {
    Platform_Base base;

    u32 *pixels;
    HWND window;
    V2 real_window_size;
    V2 real_mouse_position;
    HDC memory_dc;
    HBITMAP bitmap;
};

#define CPU_DRAW_IMPLEMENTATION
#define CPU_DRAW_FUNCTION static
#define CPU_DRAW_SINF sinf
#define CPU_DRAW_COSF cosf
#include "cpu_draw.h"

static V2 platform_measure_text(Platform_Base *platform, String text, f32 font_size) {
    (void)platform;
    (void)font_size; // TODO(felix)
    int width = 0;
    cpu_draw_text((cpu_draw_Surface){0}, cpu_draw_default_font, 0, 0, 0, (const char *)text.data, (int)text.count, &width);
    V2 result = {
        .x = (f32)width,
        .y = (f32)cpu_draw_default_font.height,
    };
    return result;
}

static App_Key app_key_from_win32(WPARAM winkey) {
    App_Key key = 0;

    bool same = ' ' <= winkey && winkey <= 'Z';
    if (same) key = (App_Key)winkey;
    else switch (winkey) {
        default: {
            static bool once = false;
            if (!once) {
                log_info("unimplemented translation from win32 keycode <0x%x> (logging ONCE for all keys)", winkey);
                once = true;
            }
        } break;
    }

    return key;
}

static Platform *window_procedure__platform;
static LRESULT window_procedure__(HWND window, u32 message, WPARAM w, LPARAM l) {
    Platform *platform = window_procedure__platform;
    App_Frame_Info *frame = &platform->base.frame;
    switch (message) {
        case WM_CLOSE: platform->base.should_quit = true; break;
        case WM_DESTROY: PostQuitMessage(0); break;
        case WM_DISPLAYCHANGE: {
            HMONITOR monitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);
            MONITORINFO info = { .cbSize = sizeof info };
            BOOL ok = GetMonitorInfoA(monitor, &info);
            ensure(ok);
            platform->real_window_size.x = (f32)(info.rcMonitor.right - info.rcMonitor.left);
            platform->real_window_size.y = (f32)(info.rcMonitor.bottom - info.rcMonitor.top);
        } break;
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
            App_Key key = app_key_from_win32(w);
            frame->key_pressed[key] = !frame->key_down[key];
            frame->key_down[key] = true;
        } break;
        case WM_KEYUP: {
            App_Key key = app_key_from_win32(w);
            frame->key_down[key] = false;
        } break;
        case WM_LBUTTONDOWN: {
            App_Mouse_Button button = App_Mouse_Button_LEFT;
            frame->mouse_clicked[button] = !frame->mouse_down[button];
            frame->mouse_down[button] = true;
        } break;
        case WM_LBUTTONUP: {
            App_Mouse_Button button = App_Mouse_Button_LEFT;
            frame->mouse_down[button] = false;
        } break;
        case WM_MOUSEMOVE: {
            // NOTE(felix): we include windowsx.h just for these two lparam macros. Can we remove that and do something else?
            platform->real_mouse_position.x = (f32)GET_X_LPARAM(l);
            platform->real_mouse_position.y = (f32)GET_Y_LPARAM(l);
        } break;
        case WM_RBUTTONDOWN: {
            App_Mouse_Button button = App_Mouse_Button_RIGHT;
            frame->mouse_clicked[button] = !frame->mouse_down[button];
            frame->mouse_down[button] = true;
        } break;
        case WM_RBUTTONUP: {
            App_Mouse_Button button = App_Mouse_Button_RIGHT;
            frame->mouse_down[button] = false;
        } break;
        case WM_SIZE: {
            platform->real_window_size.x = (f32)LOWORD(l);
            platform->real_window_size.y = (f32)HIWORD(l);
        } break;
        case WM_SIZING: {
            // TODO(felix): handle redrawing here so that we're not blocked while resizing
            // NOTE(felix): apparently there might be some problems with this and the better way to do it is for the thread with window events to be different than the application thread that submits draw calls and updates the program
        } break;
    }
    return DefWindowProcA(window, message, w, l);
}

static u32 platform__argb_from_rgba(u32 rgba) {
    u32 bgra = (rgba >> 8) | (rgba << 24);
    return bgra;
}

static void program(void) {
    static Arena persistent_arena;
    persistent_arena = arena_init(32 * 1024 * 1024);
    static Arena frame_arena;
    frame_arena = arena_init(32 * 1024 * 1024);

    static Platform platform = { .base = {
        .persistent_arena = &persistent_arena,
        .frame_arena = &frame_arena,
    } };
    App_Frame_Info *frame = &platform.base.frame;

    app_start(&platform.base);

    if (frame->window_size.x == 0) frame->window_size.x = 960.f;
    if (frame->window_size.y == 0) frame->window_size.y = 540.f;

    const char *window_name = "app";

    frame->dpi_scale = 1.f;
    window_procedure__platform = &platform;
    WNDCLASSA window_class = {
        .lpfnWndProc = window_procedure__,
        .hCursor = LoadCursorA(0, IDC_ARROW),
        .lpszClassName = window_name,
    };
    ATOM atom = RegisterClassA(&window_class);
    ensure(atom != 0);

    BOOL bool_ok = SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    ensure(bool_ok);

    {
        DWORD extended_style = 0;
        const char *class_name = window_name;
        int x = CW_USEDEFAULT, y = CW_USEDEFAULT, window_width = CW_USEDEFAULT, window_height = CW_USEDEFAULT;
        platform.window = CreateWindowExA(extended_style, class_name, window_name, WS_OVERLAPPEDWINDOW, x, y, window_width, window_height, 0, 0, 0, 0);
        ensure(platform.window != 0);
    }

    ShowWindow(platform.window, SW_SHOWDEFAULT);

    {
        HDC window_dc = GetDC(platform.window);
        ensure(window_dc != 0);

        i32 virtual_width = (i32)frame->window_size.x;
        i32 virtual_height = (i32)frame->window_size.y;

        BITMAPINFOHEADER bitmap_info_header = {
            .biSize = sizeof bitmap_info_header,
            .biWidth = virtual_width,
            .biHeight = -virtual_height,
            .biPlanes = 1,
            .biBitCount = sizeof(*platform.pixels) * 8,
        };

        platform.memory_dc = CreateCompatibleDC(window_dc);
        ensure(platform.memory_dc != 0);

        platform.bitmap = CreateDIBSection(platform.memory_dc, (BITMAPINFO *)&bitmap_info_header, DIB_RGB_COLORS, (void **)&platform.pixels, 0, 0);
        ensure(platform.bitmap != 0);

        SelectObject(platform.memory_dc, platform.bitmap);

        ReleaseDC(platform.window, window_dc);
    }

    // TODO(felix): init font

    f64 time_samples[16] = {0};
    u64 time_sample_i = 0;
    static bool filled_samples = false;

    f64 frame_start_time = time_now();
    while (!platform.base.should_quit) {
        f64 frame_end_time = time_now();
        f64 elapsed = time_elapsed_seconds(frame_start_time, frame_end_time);
        frame_start_time = time_now();

        if (!filled_samples) {
            for (u64 i = 0; i < count_of(time_samples); i += 1) time_samples[i] = elapsed;
            filled_samples = true;
        } else {
            time_samples[time_sample_i] = elapsed;
            time_sample_i += 1;
            time_sample_i %= count_of(time_samples);
        }

        elapsed = 0;
        for (u64 i = 0; i < count_of(time_samples); i += 1) elapsed += time_samples[i];
        elapsed /= count_of(time_samples);

        platform.base.frame.seconds_since_last_frame = (f32)elapsed;

        Scratch scratch = scratch_begin(platform.base.frame_arena);
        platform.base.draw_commands = (Array_Draw_Command){ .arena = scratch.arena };

        for (MSG message = {0}; PeekMessageA(&message, 0, 0, 0, PM_REMOVE);) {
            if (message.message == WM_QUIT) {
                platform.base.should_quit = true;
            }
            TranslateMessage(&message);
            DispatchMessageA(&message);
        }
        if (platform.base.should_quit) break;

        for (u64 i = 0; i < 2; i += 1) {
            frame->mouse_position.v[i] = platform.real_mouse_position.v[i] * (frame->window_size.v[i] / platform.real_window_size.v[i]);
        }

        app_update_and_render(&platform.base);

        i32 virtual_width = (i32)frame->window_size.x;
        i32 virtual_height = (i32)frame->window_size.y;

        cpu_draw_Surface surface = {
            .pixels = platform.pixels,
            .stride = virtual_width,
            .width = virtual_width,
            .height = virtual_height,
        };

        u32 clear = platform__argb_from_rgba(platform.base.clear_color);
        cpu_draw_rectangle(surface, 0, 0, surface.width, surface.height, clear);

        for (u64 i = 0; i < platform.base.draw_commands.count; i += 1) {
            Draw_Command *c = &platform.base.draw_commands.data[i];
            u32 argb = platform__argb_from_rgba(c->color[Draw_Color_SOLID]);
            switch (c->kind) {
                case Draw_Kind_TEXT: {
                    String s = c->text.string;
                    int left = (int)(c->position.x + 0.5f);
                    int top = (int)(c->position.y + 0.5f);
                    cpu_draw_text(surface, cpu_draw_default_font, left, top, argb, (const char *)s.data, (int)s.count, 0);
                } break;
                case Draw_Kind_RECTANGLE: {
                    i32 left = (i32)c->position.x;
                    i32 top = (i32)c->position.y;
                    i32 width = (i32)c->rectangle.size.x;
                    i32 height = (i32)c->rectangle.size.y;

                    i32 bottom = CLAMP(top + height, 0, virtual_height);
                    i32 right = CLAMP(left + width, 0, virtual_width);
                    top = CLAMP(top, 0, virtual_height - 1);
                    left = CLAMP(left, 0, virtual_width - 1);

                    f32 theta = c->rectangle.rotation_radians;

                    V2 pivot = {0};
                    if (v2_equals(c->rectangle.pivot, (V2){0})) {
                        V2 add = v2_scale(c->rectangle.size, 0.5f);
                        pivot = v2_add(c->position, add);
                    } else pivot = c->rectangle.pivot;

                    cpu_draw_rectangle_rotated(surface, left, top, right, bottom, argb, theta, (int)pivot.x, (int)pivot.y);
                } break;
                case Draw_Kind_QUADRILATERAL: {
                    int coordinates[2 * Draw_Corner_COUNT] = {0};
                    for (Draw_Corner corner = 0; corner < Draw_Corner_COUNT; corner += 1) {
                        V2 position = c->quadrilateral[corner];
                        for (int axis = 0; axis < 2; axis += 1) {
                            coordinates[corner * 2 + axis] = (int)(position.v[axis] + 0.5f);
                        }
                    }

                    int *tl = &coordinates[Draw_Corner_TOP_LEFT * 2];
                    int *bl = &coordinates[Draw_Corner_BOTTOM_LEFT * 2];
                    int *br = &coordinates[Draw_Corner_BOTTOM_RIGHT * 2];
                    int *tr = &coordinates[Draw_Corner_TOP_RIGHT * 2];

                    if (c->gradient) {
                        u32 tl_color = argb;
                        u32 bl_color = argb;
                        u32 br_color = argb;
                        u32 tr_color = argb;
                        bl_color = platform__argb_from_rgba(c->color[Draw_Color_BOTTOM_LEFT]);
                        br_color = platform__argb_from_rgba(c->color[Draw_Color_BOTTOM_RIGHT]);
                        tr_color = platform__argb_from_rgba(c->color[Draw_Color_TOP_RIGHT]);
                        cpu_draw_triangle_interpolate(surface, tl[0], tl[1], bl[0], bl[1], br[0], br[1], tl_color, bl_color, br_color);
                        cpu_draw_triangle_interpolate(surface, tl[0], tl[1], br[0], br[1], tr[0], tr[1], tl_color, br_color, tr_color);
                    } else {
                        cpu_draw_triangle(surface, tl[0], tl[1], bl[0], bl[1], br[0], br[1], argb);
                        cpu_draw_triangle(surface, tl[0], tl[1], br[0], br[1], tr[0], tr[1], argb);
                    }
                } break;
                case Draw_Kind_LINE: {
                    assert(c->line.thickness == 1.f); // TODO(felix)
                    int start_x = (int)(c->position.x + 0.5f);
                    int start_y = (int)(c->position.y + 0.5f);
                    int end_x = (int)(c->line.end.x + 0.5f);
                    int end_y = (int)(c->line.end.y + 0.5f);
                    cpu_draw_line(surface, start_x, start_y, end_x, end_y, argb);
                } break;
                default: unreachable;
            }
        }

        HDC window_dc = GetDC(platform.window);
        ensure(window_dc != 0);
        {
            i32 real_width = (i32)platform.real_window_size.x;
            i32 real_height = (i32)platform.real_window_size.y;

            BOOL ok = StretchBlt(
                window_dc,
                0, 0, real_width, real_height,
                platform.memory_dc,
                0, 0, virtual_width, virtual_height,
                SRCCOPY
            );
            ensure(ok);

            #pragma comment(lib, "dwmapi.lib")
            DwmFlush(); // TODO(felix): better way to "vsync"
        }
        ReleaseDC(platform.window, window_dc);

        memset(frame->mouse_clicked, 0, sizeof frame->mouse_clicked);
        memset(frame->key_pressed, 0, sizeof frame->key_pressed);
        scratch_end(scratch);
        if (platform.base.should_quit) break;
    }

    DestroyWindow(platform.window);
    DeleteObject(platform.bitmap);
    DeleteDC(platform.memory_dc);

    app_quit(&platform.base);
}
