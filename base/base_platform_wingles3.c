#define BASE_PLATFORM_WINGLES3

#include "base/base_platform_gles3.h"
#include "base/base_platform_windows_common.c"

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
// avoid #include <math.h>
#define STBTT_ifloor(x) ((int)floor(x))
#define STBTT_iceil(x) ((int)ceil(x))
#define STBTT_sqrt sqrt
#define STBTT_pow pow
#define STBTT_fmod fmod
#define STBTT_acos acos
#define STBTT_cos cos
#define STBTT_fabs fabs
// avoid #include <stdlib.h>
#define STBTT_malloc(x, u) arena_make_((u), 1, (x), true)
#define STBTT_free(x, u) ((void)(x), (void)(u))
// avoid #include <assert.h>
#define STBTT_assert assert
// avoid #include <string.h>
#define STBTT_strlen cstring_length
#define STBTT_memcpy memcpy
#define STBTT_memset memset
#if COMPILER_CLANG
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wsign-conversion"
    #pragma clang diagnostic ignored "-Wswitch-default"
    #pragma clang diagnostic ignored "-Wfloat-equal"
    #pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"
#endif
#include "base/stb_truetype.h"
#if COMPILER_CLANG
    #pragma clang diagnostic pop
#endif

// https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-pixelformatdescriptor
typedef struct {
    unsigned short nSize;
    unsigned short nVersion;
    unsigned long  dwFlags;
    unsigned char  iPixelType;
    unsigned char  cColorBits;
    unsigned char  cRedBits;
    unsigned char  cRedShift;
    unsigned char  cGreenBits;
    unsigned char  cGreenShift;
    unsigned char  cBlueBits;
    unsigned char  cBlueShift;
    unsigned char  cAlphaBits;
    unsigned char  cAlphaShift;
    unsigned char  cAccumBits;
    unsigned char  cAccumRedBits;
    unsigned char  cAccumGreenBits;
    unsigned char  cAccumBlueBits;
    unsigned char  cAccumAlphaBits;
    unsigned char  cDepthBits;
    unsigned char  cStencilBits;
    unsigned char  cAuxBuffers;
    unsigned char  iLayerType;
    unsigned char  bReserved;
    unsigned long  dwLayerMask;
    unsigned long  dwVisibleMask;
    unsigned long  dwDamageMask;
} PIXELFORMATDESCRIPTOR;
enum {
    PFD_DRAW_TO_WINDOW = 0x00000004,
    PFD_SUPPORT_OPENGL = 0x00000020,
    PFD_DOUBLEBUFFER = 0x00000001,
    PFD_TYPE_RGBA = 0,
};

meta(link "Gdi32")
WIN32_CALL(int) ChoosePixelFormat(void *hdc, const PIXELFORMATDESCRIPTOR *);
WIN32_CALL(int) DescribePixelFormat(void *hdc, int pixel_format, unsigned bytes, PIXELFORMATDESCRIPTOR *);
WIN32_CALL(int) SetPixelFormat(void *hdc, int format, const PIXELFORMATDESCRIPTOR *);
WIN32_CALL(int) SwapBuffers(void *hdc);

// https://registry.khronos.org/OpenGL/api/GL/wglext.h

#define WGL_DRAW_TO_WINDOW_ARB 0x2001
#define WGL_SUPPORT_OPENGL_ARB 0x2010
#define WGL_DOUBLE_BUFFER_ARB  0x2011
#define WGL_PIXEL_TYPE_ARB     0x2013
#define WGL_COLOR_BITS_ARB     0x2014
#define WGL_DEPTH_BITS_ARB     0x2022
#define WGL_STENCIL_BITS_ARB   0x2023
#define WGL_TYPE_RGBA_ARB      0x202B

#define WGL_CONTEXT_DEBUG_BIT_ARB       0x00000001
#define WGL_CONTEXT_ES2_PROFILE_BIT_EXT 0x00000004
#define WGL_CONTEXT_MAJOR_VERSION_ARB   0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB   0x2092
#define WGL_CONTEXT_FLAGS_ARB           0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB    0x9126

meta(link "Opengl32")
typedef int (* PROC)(void);
WIN32_CALL(void *) wglCreateContext(void *hdc);
WIN32_CALL(int) wglDeleteContext(void *hglrc);
WIN32_CALL(PROC)   wglGetProcAddress(const char *name);
WIN32_CALL(int)    wglMakeCurrent(void *hdc, void *hglrc);

struct Platform_Implementation {
    Platform base;

    // TODO(felix)
};

static V2 platform_measure_text(Platform *platform, String text, f32 font_size) {
    assert(font_size > 0);
    assert(text.count > 0);

    Platform_Implementation *p = (Platform_Implementation *)platform;

    // TODO(felix)
    (void)p;

    return (V2){0};
}

static Platform_Implementation *window_procedure__platform;
static i64 window_procedure__(void *window, u32 message, u64 w, i64 l) {
    Platform_Implementation *platform = window_procedure__platform;
    i64 result = win32_window_procedure(window, message, w, l, &platform->base, &platform->base.frame.window_size.x, &platform->base.frame.mouse_position.x);
    return result;
}

static void program(void) {
    static Arena persistent_arena;
    persistent_arena = arena_init(4 * 1024 * 1024);
    static Arena frame_arena;
    frame_arena = arena_init(4 * 1024 * 1024);

    static Platform_Implementation platform = { .base = {
        .persistent_arena = &persistent_arena,
        .frame_arena = &frame_arena,
    } };
    App_Frame_Info *frame = &platform.base.frame;

    app_start(&platform.base);

    typedef int WGLCHOOSEPIXELFORMATARBPROC(void *hdc, const int *attribute_i_list, const float *attribute_f_list, unsigned max_format_count, int *formats, unsigned *format_count);
    WGLCHOOSEPIXELFORMATARBPROC *wglChoosePixelFormatARB = 0;
    typedef void *WGLCREATECONTEXTATTRIBSARBPROC(void *hdc, void *hglrc_share_context, const int *attribute_list);
    WGLCREATECONTEXTATTRIBSARBPROC *wglCreateContextAttribsARB = 0;
    typedef int WGLSWAPINTERVALEXTPROC(int interval);
    WGLSWAPINTERVALEXTPROC *wglSwapIntervalEXT = 0;

    {
        void *dummy = CreateWindowExA(0, "STATIC", "dummy", WS_OVERLAPPED, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, 0, 0);
        ensure(dummy != 0);

        void *device_context = GetDC(dummy);
        ensure(device_context != 0);

        PIXELFORMATDESCRIPTOR descriptor = {
            .nSize = sizeof descriptor,
            .nVersion = 1,
            .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
            .iPixelType = PFD_TYPE_RGBA,
            .cColorBits = 24,
        };
        int format = ChoosePixelFormat(device_context, &descriptor);
        ensure(format != 0);

        int ok = DescribePixelFormat(device_context, format, sizeof descriptor, &descriptor);
        ensure(ok);

        ok = SetPixelFormat(device_context, format, &descriptor);
        ensure(ok);

        void *render_context = wglCreateContext(device_context);
        ensure(render_context != 0);

        ok = wglMakeCurrent(device_context, render_context);
        ensure(ok);

        typedef const char *WGLGETEXTENSIONSSTRINGARBPROC(void *hdc);
        WGLGETEXTENSIONSSTRINGARBPROC *wglGetExtensionsStringARB = (WGLGETEXTENSIONSSTRINGARBPROC *)wglGetProcAddress("wglGetExtensionsStringARB");
        ensure(wglGetExtensionsStringARB != 0);

        bool es_profile_supported = false;
        const char *extensions = wglGetExtensionsStringARB(device_context);
        ensure(extensions != 0);
        for (const char *start = 0;; extensions += 1) {
            start = extensions;
            while (*extensions != 0 && *extensions != ' ') extensions += 1;
            String extension = string_from_pointers(start, extensions);

            if (string_equals(extension, S("WGL_ARB_pixel_format"))) {
                wglChoosePixelFormatARB = (WGLCHOOSEPIXELFORMATARBPROC *)wglGetProcAddress("wglChoosePixelFormatARB");
            } else if (string_equals(extension, S("WGL_ARB_create_context"))) {
                wglCreateContextAttribsARB = (WGLCREATECONTEXTATTRIBSARBPROC *)wglGetProcAddress("wglCreateContextAttribsARB");
            } else if (string_equals(extension, S("WGL_EXT_swap_control"))) {
                wglSwapIntervalEXT = (WGLSWAPINTERVALEXTPROC *)wglGetProcAddress("wglSwapIntervalEXT");
            } else if (string_equals(extension, S("WGL_EXT_create_context_es_profile")) || string_equals(extension, S("WGL_EXT_create_context_es2_profile"))) {
                es_profile_supported = true;
            }

            if (*extensions == 0) break;
        }
        ensure(es_profile_supported);
        ensure(wglChoosePixelFormatARB != 0);
        ensure(wglCreateContextAttribsARB != 0);
        ensure(wglSwapIntervalEXT != 0);

        wglMakeCurrent(0, 0);
        wglDeleteContext(render_context);
        ReleaseDC(dummy, render_context);
        DestroyWindow(dummy);
    }

    window_procedure__platform = &platform;
    const char *window_name = platform.base.window_name != 0 ? platform.base.window_name : "app";
    void *window = win32_create_window(window_name, window_procedure__);
    ensure(window != 0);

    void *device_context = GetDC(window);
    ensure(device_context != 0);

    {
        int attributes[] = {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
            WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
            WGL_COLOR_BITS_ARB,     24,
            WGL_DEPTH_BITS_ARB,     24,
            WGL_STENCIL_BITS_ARB,   8,
            0,
        };

        int format = 0;
        unsigned formats = 0;

        int ok = wglChoosePixelFormatARB(device_context, attributes, 0, 1, &format, &formats);
        ensure(ok);
        ensure(formats != 0);

        PIXELFORMATDESCRIPTOR descriptor = { .nSize = sizeof descriptor };
        ok = DescribePixelFormat(device_context, format, sizeof descriptor, &descriptor);
        ensure(ok);

        ok = SetPixelFormat(device_context, format, &descriptor);
        ensure(ok);
    }

    {
        int attributes[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
            WGL_CONTEXT_MINOR_VERSION_ARB, 0,
            WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_ES2_PROFILE_BIT_EXT,

            #if BUILD_DEBUG
                WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
            #endif

            0,
        };

        void *render_context = wglCreateContextAttribsARB(device_context, 0, attributes);
        ensure(render_context != 0);

        int ok = wglMakeCurrent(device_context, render_context);
        ensure(ok);

        #define X(type, name) name = (type)wglGetProcAddress(#name); ensure(name != 0);
        GLES3_FUNCTIONS
        #undef X

        if (BUILD_DEBUG) {
            glDebugMessageCallbackKHR(&gles3_debug_message_callback, 0);
            glEnable(GL_DEBUG_OUTPUT_KHR);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_KHR);
        }
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    bool vsync = true;
    wglSwapIntervalEXT(vsync ? 1 : 0);

    ShowWindow(window, SW_SHOWDEFAULT);

    for (Arena frame_scratch = *platform.base.frame_arena; !platform.base.should_quit; *platform.base.frame_arena = frame_scratch) {
        platform.base.draw_commands = (Array_Draw_Command){0};

        win32_process_messages(&platform.base.should_quit);
        if (platform.base.should_quit) break;

        static Time_Averaged_Sampler sampler = {0};
        static bool sampler_init = false;
        if (!sampler_init) {
            #define PLATFORM__TIME_SAMPLES 16
            static f64 samples[PLATFORM__TIME_SAMPLES] = {0};
            sampler = time_averaged_sampler(samples, PLATFORM__TIME_SAMPLES);
            sampler_init = true;
        }
        frame->seconds_since_last_frame = (f32)time_averaged_sample_elapsed_seconds(&sampler);

        // TODO(felix): uncomment once text measurement works
        // app_update_and_render(&platform.base);

        bool should_render = frame->window_size.x > 0 && frame->window_size.y > 0;
        if (should_render) {
            glViewport(0, 0, (GLsizei)roundf(frame->window_size.x), (GLsizei)roundf(frame->window_size.y));

            {
                V4 c = rgba_float_from_hex(platform.base.clear_color);
                glClearColor(c.x, c.y, c.z, c.w);
            }
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            int ok = SwapBuffers(device_context);
            ensure(ok);
        } else {
            // TODO(felix): cannot vsync so instead sleep a bit
        }

        app_frame_info_reset_end_of_frame(frame);
        if (platform.base.should_quit) break;
    }

    app_quit(&platform.base);
}
