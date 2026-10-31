#define BASE_PLATFORM_WINGLES3

#include "base/base_platform_gles3.h"
#include "base/base_platform_windows_common.c"

// TODO(felix): kb_text_shape + slug
// #define KB_TEXT_SHAPE_IMPLEMENTATION
// #define KB_TEXT_SHAPE_STATIC
// #define KB_TEXT_SHAPE_NO_CRT
// #define KBTS_MALLOC(u, n) arena_make_((u), 1, (n), 1)
// #define KBTS_FREE(u, p) ((void)(u), (void)(p))
// #if COMPILER_MSVC
//     #pragma warning(push)
//     #pragma warning(disable: 4100 4366)
// #endif
// #include "base/third_party/kb_text_shape.h"
// #if COMPILER_MSVC
//     #pragma warning(pop)
// #endif

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
#include "base/third_party/stb_truetype.h"
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

META(link "Gdi32")
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

META(link "Opengl32")
typedef int (* PROC)(void);
WIN32_CALL(void *) wglCreateContext(void *hdc);
WIN32_CALL(int) wglDeleteContext(void *hglrc);
WIN32_CALL(PROC)   wglGetProcAddress(const char *name);
WIN32_CALL(int)    wglMakeCurrent(void *hdc, void *hglrc);

struct Platform_Implementation {
    Platform base;

    stbtt_fontinfo font_info;
    int font_ascent;
    int font_descent;
};

static V2 platform_measure_text(Platform *platform, String text, f32 font_size) {
    assert(font_size > 0);
    assert(text.count > 0);
    V2 result = {0};

    Platform_Implementation *p = (Platform_Implementation *)platform;
    f32 scale = stbtt_ScaleForPixelHeight(&p->font_info, font_size);

    int width = 0;
    int last_glyph_index = 0;
    for (u64 i = 0; i < text.count; i += 1) {
        int glyph_index = stbtt_FindGlyphIndex(&p->font_info, (unsigned char)text.data[i]);

        int advance_width = 0, left_side_bearing = 0;
        stbtt_GetGlyphHMetrics(&p->font_info, glyph_index, &advance_width, &left_side_bearing);
        if (i > 0) width += stbtt_GetGlyphKernAdvance(&p->font_info, last_glyph_index, glyph_index);
        width += advance_width;

        last_glyph_index = glyph_index;
    }

    result.x = (f32)width * scale;
    result.y = (f32)(p->font_ascent - p->font_descent) * scale;
    return result;
}

static Platform_Implementation *window_procedure__platform;
static i64 window_procedure__(void *window, u32 message, u64 w, i64 l) {
    Platform_Implementation *platform = window_procedure__platform;
    i64 result = win32_window_procedure(window, message, w, l, &platform->base, &platform->base.frame.window_size.x, &platform->base.frame.mouse_position.x);
    return result;
}

static void program(void) {
    static Arena persistent_arena;
    persistent_arena = arena_init(16 * 1024 * 1024);
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
        ReleaseDC(dummy, device_context);
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
    frame->dpi_scale = (f32)GetDpiForWindow(window) / 96.f;
    // TODO(felix): Dogfood custom shader code parser once that exists.

    enum { VERTEX = 0, FRAGMENT = 1 };
    static const char *ubershader[2] = {
        [VERTEX] = "#version 300 es\n" STRINGIFY(
            layout(location = 0) in vec4 rectangle_attribute;
            layout(location = 1) in vec4 uv_attribute;
            layout(location = 2) in vec4 color_attribute;

            uniform vec2 window_size_uniform;

            out vec2 uv_varying;
            out vec4 color_varying;

            void main() {
                vec2 corner = vec2(gl_VertexID & 1, gl_VertexID >> 1);
                vec2 position = mix(rectangle_attribute.xy, rectangle_attribute.zw, corner);

                uv_varying = mix(uv_attribute.xy, uv_attribute.zw, corner);
                color_varying = color_attribute;

                vec2 normalised_device_coordinates = (position / window_size_uniform * 2.0) - 1.0;
                gl_Position = vec4(normalised_device_coordinates.x, -normalised_device_coordinates.y, 0.0, 1.0);
            }
        ),
        [FRAGMENT] = "#version 300 es\n" STRINGIFY(
            precision highp float;

            uniform sampler2D atlas_uniform;

            in vec2 uv_varying;
            in vec4 color_varying;

            out vec4 color_out;

            void main() {
                color_out = vec4(color_varying.rgb, color_varying.a * texture(atlas_uniform, uv_varying).r);
            }
        ),
    };

    GLuint program = glCreateProgram();
    ensure(program != 0);

    GLenum gl_shader_kind[2] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };
    for (int shader_kind = 0; shader_kind < 2; shader_kind += 1) {
        GLuint shader = glCreateShader(gl_shader_kind[shader_kind]);
        ensure(shader != 0);

        glShaderSource(shader, 1, &ubershader[shader_kind], 0);
        glCompileShader(shader);
        GLint shader_ok = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_ok);
        ensure(shader_ok);

        glAttachShader(program, shader);
    }

    glLinkProgram(program);
    GLint program_ok = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &program_ok);
    ensure(program_ok);

    glUseProgram(program);
    GLint window_size_uniform = glGetUniformLocation(program, "window_size_uniform");
    // GLint atlas_uniform = 0;

    GLuint vertex_array_object = 0;
    glGenVertexArrays(1, &vertex_array_object);
    glBindVertexArray(vertex_array_object);

    GLuint vertex_buffer_object = 0;
    glGenBuffers(1, &vertex_buffer_object);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);

    typedef struct { V4 rect, uv; u8 color[4]; } Ubershader_Quad;
    static Ubershader_Quad ubershader_quads[16384]; // TODO(felix): Is this limit ok?

    GLsizei stride = sizeof(Ubershader_Quad);
    glVertexAttribPointer(0, 4, GL_FLOAT,         GL_FALSE, stride, 0);
    glVertexAttribPointer(1, 4, GL_FLOAT,         GL_FALSE, stride, (void *)16);
    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE,  stride, (void *)32);
    for (GLuint i = 0; i < 3; i += 1) {
        glEnableVertexAttribArray(i);
        glVertexAttribDivisor(i, 1);
    }

    static const f32 atlas_font_sizes[] = { 16.f, 18.f, 24.f, 32.f, 36.f };
    enum {
        ATLAS_SIZE = 1024,
        ATLAS_FIRST_CHAR = 32,
        ATLAS_CHAR_COUNT = 95,
        ATLAS_FONT_SIZES_COUNT = (sizeof atlas_font_sizes) / (sizeof atlas_font_sizes[0]),
    };
    static stbtt_packedchar atlas_chars[ATLAS_FONT_SIZES_COUNT][ATLAS_CHAR_COUNT];
    {
        META(embed "src/base/third_party/Inter/Inter-Regular.ttf")
        extern const char ttf_bytes[];
        extern const u64 ttf_bytes_count;
        unsigned const char *ttf = (unsigned const char *)ttf_bytes;

        stbtt_pack_range ranges[ATLAS_FONT_SIZES_COUNT] = {0};
        for (int i = 0; i < ATLAS_FONT_SIZES_COUNT; i += 1) {
            ranges[i] = (stbtt_pack_range){
                .font_size = STBTT_POINT_SIZE(atlas_font_sizes[i]),
                .first_unicode_codepoint_in_range = ATLAS_FIRST_CHAR,
                .num_chars = ATLAS_CHAR_COUNT,
                .chardata_for_range = atlas_chars[i],
            };
        }

        int font_ok = stbtt_InitFont(&platform.font_info, ttf, 0);
        ensure(font_ok);
        int line_gap = 0;
        stbtt_GetFontVMetrics(&platform.font_info, &platform.font_ascent, &platform.font_descent, &line_gap);

        Arena scratch = *platform.base.persistent_arena;
        unsigned char *atlas = arena_make(&scratch, ATLAS_SIZE * ATLAS_SIZE, unsigned char);

        stbtt_pack_context pack_context = {0};
        int pack_ok = stbtt_PackBegin(&pack_context, atlas, ATLAS_SIZE, ATLAS_SIZE, 0, 1, &scratch);
        ensure(pack_ok);
        stbtt_PackSetOversampling(&pack_context, 2, 1);
        pack_ok = stbtt_PackFontRanges(&pack_context, ttf, 0, ranges, ATLAS_FONT_SIZES_COUNT);
        ensure(pack_ok);
        stbtt_PackEnd(&pack_context);

        GLuint texture = 0;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, ATLAS_SIZE, ATLAS_SIZE, 0, GL_RED, GL_UNSIGNED_BYTE, atlas);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

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

        app_update_and_render(&platform.base);

        bool should_render = frame->window_size.x > 0 && frame->window_size.y > 0;
        if (should_render) {
            glViewport(0, 0, (GLsizei)roundf(frame->window_size.x), (GLsizei)roundf(frame->window_size.y));

            {
                V4 c = rgba_float_from_hex(platform.base.clear_color);
                glClearColor(c.x, c.y, c.z, c.w);
            }
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            glUniform2f(window_size_uniform, frame->window_size.x, frame->window_size.y);

            u64 quad_count = 0;
            for (u64 i = 0; i < platform.base.draw_commands.count; i += 1) {
                Draw_Command *c = &platform.base.draw_commands.data[i];

                bool has_clip = !!memcmp(&c->clip, &(V4){0}, sizeof c->clip);
                if (has_clip) {
                    panic("TODO(felix)");
                }

                u32 color = c->color[Draw_Color_SOLID];
                switch (c->kind) {
                    case Draw_Kind_TEXT: {
                        String string = c->text.string;
                        f32 font_size = c->text.font_size;

                        int nearest_baked_size_index = 0;
                        for (int size_index = 1; size_index < ATLAS_FONT_SIZES_COUNT; size_index += 1) {
                            f32 least_difference = fabsf(atlas_font_sizes[size_index] - font_size);
                            f32 this_difference = fabsf(atlas_font_sizes[nearest_baked_size_index] - font_size);
                            // NOTE(felix): <=, not <, assuming the sizes are smallest->biggest, to pick the bigger size and later downsample.
                            if (least_difference <= this_difference) nearest_baked_size_index = size_index;
                        }
                        stbtt_packedchar *chars = atlas_chars[nearest_baked_size_index];
                        f32 atlas_scale = font_size / atlas_font_sizes[nearest_baked_size_index];
                        f32 scale = stbtt_ScaleForMappingEmToPixels(&platform.font_info, font_size);
                        f32 x = c->position.x;
                        f32 baseline = c->position.y + (f32)platform.font_ascent * scale;

                        int last_glyph_index = 0;
                        for (u64 string_index = 0; string_index < string.count; string_index += 1) {
                            int codepoint = (int)(unsigned char)string.data[string_index];
                            int glyph_index = stbtt_FindGlyphIndex(&platform.font_info, codepoint);
                            if (string_index > 0) x += (f32)stbtt_GetGlyphKernAdvance(&platform.font_info, last_glyph_index, glyph_index) * scale;

                            bool is_in_atlas = ATLAS_FIRST_CHAR <= codepoint && codepoint < (ATLAS_FIRST_CHAR + ATLAS_CHAR_COUNT);
                            if (is_in_atlas) {
                                stbtt_packedchar *packed_char = &chars[codepoint - ATLAS_FIRST_CHAR];
                                if (packed_char->x1 > packed_char->x0) {
                                    ensure(quad_count < (sizeof ubershader_quads / sizeof *ubershader_quads));
                                    f32 x0 = roundf(x + packed_char->xoff * atlas_scale);
                                    f32 y0 = roundf(baseline + packed_char->yoff * atlas_scale);
                                    ubershader_quads[quad_count] = (Ubershader_Quad){
                                        .rect = {
                                            .x = x0,
                                            .y = y0,
                                            .z = x0 + (packed_char->xoff2 - packed_char->xoff) * atlas_scale,
                                            .w = y0 + (packed_char->yoff2 - packed_char->yoff) * atlas_scale,
                                        },
                                        .uv = {
                                            .x = (f32)packed_char->x0 / (f32)ATLAS_SIZE,
                                            .y = (f32)packed_char->y0 / (f32)ATLAS_SIZE,
                                            .z = (f32)packed_char->x1 / (f32)ATLAS_SIZE,
                                            .w = (f32)packed_char->y1 / (f32)ATLAS_SIZE,
                                        },
                                        .color = {
                                            [0] = (u8)(color >> 24),
                                            [1] = (u8)(color >> 16),
                                            [2] = (u8)(color >>  8),
                                            [3] = (u8)(color),
                                        },
                                    };
                                    quad_count += 1;
                                }
                            } else {
                                // TODO(felix)
                            }

                            int advance_width = 0, left_side_bearing = 0;
                            stbtt_GetGlyphHMetrics(&platform.font_info, glyph_index, &advance_width, &left_side_bearing);
                            x += (f32)advance_width * scale;
                            last_glyph_index = glyph_index;
                        }
                    } break;
                    default: unreachable;
                }
            }

            glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(quad_count * sizeof(Ubershader_Quad)), ubershader_quads, GL_STREAM_DRAW);
            glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, (GLsizei)quad_count);
            int ok = SwapBuffers(device_context);
            ensure(ok);
        } else {
            // TODO(felix): cannot vsync so instead sleep a bit
        }

        app_frame_info_reset_end_of_frame(frame);
        if (platform.base.should_quit) break;
    }
}
