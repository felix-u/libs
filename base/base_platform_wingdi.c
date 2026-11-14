#define BASE_PLATFORM_WINGDI

#include "base/base_platform_windows_common.c"

long DwmFlush(void);

META(link "Gdi32")
void * CreateCompatibleDC(void *device_context_handle);
#if !defined(_WINGDI_)
    typedef struct {
        unsigned long biSize;
        long biWidth;
        long biHeight;
        unsigned short biPlanes;
        unsigned short biBitCount;
        unsigned long biCompression;
        unsigned long biSizeImage;
        long biXPelsPerMeter;
        long biYPelsPerMeter;
        unsigned long biClrUsed;
        unsigned long biClrImportant;
    } BITMAPINFOHEADER;
    typedef struct {
        unsigned char rgbBlue;
        unsigned char rgbGreen;
        unsigned char rgbRed;
        unsigned char rgbReserved;
    } RGBQUAD;
    typedef struct {
        BITMAPINFOHEADER bmiHeader;
        RGBQUAD bmiColors[1];
    } BITMAPINFO;
#endif
void * CreateDIBSection(void *device_context_handle, const BITMAPINFO *, unsigned usage, void **bits, void *section_handle, unsigned long offset);
int    DeleteDC(void *device_context_handle);
int    DeleteObject(void *object_handle);
void * SelectObject(void *device_context_handle, void *object_handle);
int    StretchBlt(void *destination_device_context_handle, int destination_x, int destination_y, int destination_width, int destination_height, void *source_device_context_handle, int source_x, int source_y, int source_width, int source_height, unsigned long raster_operation);

struct Platform_Implementation {
    Platform base;

    u32 *pixels;
    i32 pixel_stride;
    bool keep_pixels_between_frames;

    f32 real_over_virtual_window_scale;
    V2 real_window_size;
    V2 real_mouse_position;

    void *window;
    void *memory_dc;
    void *bitmap;

    cpu_draw_Font font;
};

static V2 platform_measure_text(Platform *platform, String text, f32 font_size) {
    return cpu_draw_platform_measure_text(&((Platform_Implementation *)platform)->font, text, font_size);
}

static Platform_Implementation *window_procedure__platform;
static i64 window_procedure__(void *window, u32 message, u64 w, i64 l) {
    Platform_Implementation *platform = window_procedure__platform;
    i64 result = win32_window_procedure(window, message, w, l, &platform->base, &platform->real_window_size.x, &platform->real_mouse_position.x);
    return result;
}

static void program(void) {
    static Arena persistent_arena;
    static Arena frame_arena;

    static Platform_Implementation platform = { .base = {
        .persistent_arena = &persistent_arena,
        .frame_arena = &frame_arena,
    } };
    App_Frame_Info *frame = &platform.base.frame;

    app_start(&platform.base);
    if (!platform.base.should_quit) {
        window_procedure__platform = &platform;
        const char *window_name = platform.base.window_name != 0 ? platform.base.window_name : "app";
        platform.window = win32_create_window(window_name, window_procedure__);

        ShowWindow(platform.window, SW_SHOWDEFAULT);

        if (platform.real_over_virtual_window_scale == 0.f) platform.real_over_virtual_window_scale = 3.f;
        frame->dpi_scale = 1.f;

        i32 max_virtual_width = 1920;
        i32 max_virtual_height = 1200;

        {
            void *window_dc = GetDC(platform.window);
            ensure(window_dc != 0);

            BITMAPINFOHEADER bitmap_info_header = {
                .biSize = sizeof bitmap_info_header,
                .biWidth = max_virtual_width,
                .biHeight = -max_virtual_height,
                .biPlanes = 1,
                .biBitCount = sizeof(*platform.pixels) * 8,
            };

            platform.memory_dc = CreateCompatibleDC(window_dc);
            ensure(platform.memory_dc != 0);

            enum { dib_rgb_colors = 0 };
            platform.bitmap = CreateDIBSection(platform.memory_dc, (BITMAPINFO *)&bitmap_info_header, dib_rgb_colors, (void **)&platform.pixels, 0, 0);
            ensure(platform.bitmap != 0);
            platform.pixel_stride = max_virtual_width;

            u32 clear = pixel_from_rgba(platform.base.clear_color);
            for (i64 i = 0; i < (i64)max_virtual_width * max_virtual_height; i += 1) platform.pixels[i] = clear;

            SelectObject(platform.memory_dc, platform.bitmap);

            ReleaseDC(platform.window, window_dc);
        }

        for (Scratch frame_scratch = scratch_begin(platform.base.frame_arena); !platform.base.should_quit; scratch_end(frame_scratch)) {
            win32_process_messages(&platform.base.should_quit);
            if (platform.base.should_quit) break;

            f32 actual_real_over_virtual_window_scale = platform.real_over_virtual_window_scale;
            actual_real_over_virtual_window_scale = ceilf(MAX(platform.real_window_size.x / (f32)max_virtual_width, actual_real_over_virtual_window_scale));
            actual_real_over_virtual_window_scale = ceilf(MAX(platform.real_window_size.y / (f32)max_virtual_height, actual_real_over_virtual_window_scale));
            assert(actual_real_over_virtual_window_scale > 0);

            platform.base.frame.window_size = v2_scale(platform.real_window_size, 1.f / actual_real_over_virtual_window_scale);

            for (u64 i = 0; i < 2; i += 1) {
                v(frame->mouse_position)[i] = v(platform.real_mouse_position)[i] * (v(frame->window_size)[i] / v(platform.real_window_size)[i]);
            }

            static Time_Averaged_Sampler sampler = {0};
            static bool sampler_init = false;
            if (!sampler_init) {
                #define PLATFORM__TIME_SAMPLES 16
                static f64 samples[PLATFORM__TIME_SAMPLES] = {0};
                sampler = time_averaged_sampler(samples, PLATFORM__TIME_SAMPLES);
                sampler_init = true;
            }
            platform.base.frame.seconds_since_last_frame = (f32)time_averaged_sample_elapsed_seconds(&sampler);

            app_update_and_render(&platform.base);

            i32 virtual_width = (i32)frame->window_size.x;
            i32 virtual_height = (i32)frame->window_size.y;

            cpu_draw_Surface surface = cpu_draw_surface(platform.pixels, max_virtual_width, virtual_width, virtual_height);

            if (!platform.keep_pixels_between_frames) {
                u32 clear = pixel_from_rgba(platform.base.clear_color);
                cpu_draw_rectangle(surface, 0, 0, surface.clip_right - surface.clip_left, surface.clip_bottom - surface.clip_top, clear);
            }
            // TODO(felix): Support the new GPU API.

            void *window_dc = GetDC(platform.window);
            ensure(window_dc != 0);
            {
                i32 real_width = (i32)platform.real_window_size.x;
                i32 real_height = (i32)platform.real_window_size.y;

                enum { srccopy = 0x00CC0020 };

                int ok = StretchBlt(
                    window_dc,
                    0, 0, real_width, real_height,
                    platform.memory_dc,
                    0, 0, virtual_width, virtual_height,
                    srccopy
                );
                ensure(ok);

                META(link "Dwmapi")
                DwmFlush(); // TODO(felix): better way to "vsync"
            }
            ReleaseDC(platform.window, window_dc);

            app_frame_info_reset_end_of_frame(frame);
        }

        DestroyWindow(platform.window);
        DeleteObject(platform.bitmap);
        DeleteDC(platform.memory_dc);
    }
}
