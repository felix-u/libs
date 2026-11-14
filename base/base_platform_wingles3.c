#define BASE_PLATFORM_WINGLES3

#include "base/base_platform_gles3_common.c"
#include "base/base_platform_windows_common.c"

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
int ChoosePixelFormat(void *hdc, const PIXELFORMATDESCRIPTOR *);
int DescribePixelFormat(void *hdc, int pixel_format, unsigned bytes, PIXELFORMATDESCRIPTOR *);
int SetPixelFormat(void *hdc, int format, const PIXELFORMATDESCRIPTOR *);
int SwapBuffers(void *hdc);

// https://registry.khronos.org/OpenGL/api/GL/wglext.h

#define WGL_CONTEXT_DEBUG_BIT_ARB       0x00000001
#define WGL_CONTEXT_ES2_PROFILE_BIT_EXT 0x00000004
#define WGL_CONTEXT_MAJOR_VERSION_ARB   0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB   0x2092
#define WGL_CONTEXT_FLAGS_ARB           0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB    0x9126

META(link "Opengl32")
typedef int (* PROC)(void);
void * wglCreateContext(void *hdc);
int wglDeleteContext(void *hglrc);
PROC   wglGetProcAddress(const char *name);
int    wglMakeCurrent(void *hdc, void *hglrc);

META(link "Imm32")
int ImmDisableIME(unsigned long thread_id);

void *GetModuleHandleA(const char *module_name);
PROC  GetProcAddress(void *module, const char *procedure_name);


static void gles3_debug_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *user_parameter) {
    (void)source;
    (void)type;
    (void)id;
    (void)severity;
    (void)user_parameter;

    String string = {0};
    if (length == -1) string = string_from_cstring(message);
    else string = (String){ .data = message, .count = (u64)length };

    print("%S%s", string, string.data[string.count - 1] == '\n' ? "" : "\n");
}


struct Platform_Implementation {
    Platform base;
    GLES3_GPU gpu;
    void *device_context;
};

static GLES3_GPU *gles3__gpu_from_platform(Platform *platform) {
    return &((Platform_Implementation *)platform)->gpu;
}

static void wingles3_update_and_render(Platform_Implementation *platform) {
    Scratch frame_scratch = scratch_begin(platform->base.frame_arena);
    App_Frame_Info *frame = &platform->base.frame;

    gles3__begin_frame(&platform->base);

    static Time_Averaged_Sampler sampler = {0};
    static bool sampler_init = false;
    if (!sampler_init) {
        #define PLATFORM__TIME_SAMPLES 16
        static f64 samples[PLATFORM__TIME_SAMPLES] = {0};
        sampler = time_averaged_sampler(samples, PLATFORM__TIME_SAMPLES);
        sampler_init = true;
    }
    frame->seconds_since_last_frame = (f32)time_averaged_sample_elapsed_seconds(&sampler);

    app_update_and_render(&platform->base);
    gles3__end_frame(&platform->base);

    bool should_present = frame->window_size.x > 0 && frame->window_size.y > 0;
    if (should_present) {
        int ok = SwapBuffers(platform->device_context);
        ensure(ok);
    }

    app_frame_info_reset_end_of_frame(frame);
    scratch_end(frame_scratch);
}



static Platform_Implementation *window_procedure__platform;
static i64 window_procedure__(void *window, u32 message, u64 w, i64 l) {
    Platform_Implementation *platform = window_procedure__platform;
    i64 result = win32_window_procedure(window, message, w, l, &platform->base, &platform->base.frame.window_size.x, &platform->base.frame.mouse_position.x);

    enum { WM_SIZE = 0x0005 };
    bool should_render_after_resize = message == WM_SIZE && platform->gpu.is_ready;
    if (should_render_after_resize) wingles3_update_and_render(platform);

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

    // NOTE(felix): Disabling the IME saves ~15 ms on the first ShowWindow, but it can't be undone and must happen before the first window.
    //              It removes CJK composition, the emoji panel and voice typing, which otherwise get through the IME as WM_CHAR.
    //              Natively-integrated text input will have to keep the IME, associating it only while a text field has focus (ImmAssociateContextEx).
    enum { IME_ALL_THREADS_IN_PROCESS = -1 };
    ImmDisableIME((unsigned long)IME_ALL_THREADS_IN_PROCESS);

    app_start(&platform.base);
    base_platform_reserve_frame_arena(&platform.base);

    window_procedure__platform = &platform;
    const char *window_name = platform.base.window_name != 0 ? platform.base.window_name : "app";
    void *window = win32_create_window(window_name, window_procedure__);
    ensure(window != 0);

    platform.device_context = GetDC(window);
    ensure(platform.device_context != 0);
    void *device_context = platform.device_context;

    {
        PIXELFORMATDESCRIPTOR descriptor = {
            .nSize = sizeof descriptor,
            .nVersion = 1,
            .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
            .iPixelType = PFD_TYPE_RGBA,
            .cColorBits = 24,
            .cDepthBits = 24,
            .cStencilBits = 8,
        };
        int format = ChoosePixelFormat(device_context, &descriptor);
        ensure(format != 0);

        int ok = DescribePixelFormat(device_context, format, sizeof descriptor, &descriptor);
        ensure(ok);

        ok = SetPixelFormat(device_context, format, &descriptor);
        ensure(ok);
    }

    typedef void *WGLCREATECONTEXTATTRIBSARBPROC(void *hdc, void *hglrc_share_context, const int *attribute_list);
    WGLCREATECONTEXTATTRIBSARBPROC *wglCreateContextAttribsARB = 0;
    typedef int WGLSWAPINTERVALEXTPROC(int interval);
    WGLSWAPINTERVALEXTPROC *wglSwapIntervalEXT = 0;

    {
        void *legacy_context = wglCreateContext(device_context);
        ensure(legacy_context != 0);

        int ok = wglMakeCurrent(device_context, legacy_context);
        ensure(ok);

        typedef const char *WGLGETEXTENSIONSSTRINGARBPROC(void *hdc);
        WGLGETEXTENSIONSSTRINGARBPROC *wglGetExtensionsStringARB = (WGLGETEXTENSIONSSTRINGARBPROC *)wglGetProcAddress("wglGetExtensionsStringARB");
        ensure(wglGetExtensionsStringARB != 0);

        bool es_profile_supported = false;
        const char *extensions = wglGetExtensionsStringARB(device_context);
        ensure(extensions != 0);
        for (const char *start = extensions; *start != 0;) {
            const char *end = start;
            while (*end != 0 && *end != ' ') end += 1;
            String extension = string_from_pointers(start, end);

            if (string_equals(extension, S("WGL_ARB_create_context"))) {
                wglCreateContextAttribsARB = (WGLCREATECONTEXTATTRIBSARBPROC *)wglGetProcAddress("wglCreateContextAttribsARB");
            } else if (string_equals(extension, S("WGL_EXT_swap_control"))) {
                wglSwapIntervalEXT = (WGLSWAPINTERVALEXTPROC *)wglGetProcAddress("wglSwapIntervalEXT");
            } else if (string_equals(extension, S("WGL_EXT_create_context_es_profile")) || string_equals(extension, S("WGL_EXT_create_context_es2_profile"))) {
                es_profile_supported = true;
            }

            start = end + (*end == ' ');
        }
        ensure(es_profile_supported);
        ensure(wglCreateContextAttribsARB != 0);
        ensure(wglSwapIntervalEXT != 0);

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

        ok = wglMakeCurrent(device_context, render_context);
        ensure(ok);
        wglDeleteContext(legacy_context);

        void *opengl32 = GetModuleHandleA("opengl32.dll");
        ensure(opengl32 != 0);

        #define X(type, name) { \
            PROC procedure = wglGetProcAddress(#name); \
            i64 procedure_value = (i64)procedure; \
            bool is_not_from_driver = -1 <= procedure_value && procedure_value <= 3; \
            if (is_not_from_driver) procedure = GetProcAddress(opengl32, #name); \
            ensure(procedure != 0); \
            name = (type)procedure; \
        }
        GLES3_FUNCTIONS
        #undef X

        // NOTE(felix): ChoosePixelFormat can't ask for a non-sRGB framebuffer. The driver can pick sRGB and then brighten every colour. This turns that off, so colours reach the window as written, as in WebGL2. The alternative is a dummy window for wglChoosePixelFormatARB, which costs about 30 ms at startup.
        glDisable(GL_FRAMEBUFFER_SRGB_EXT);

        if (BUILD_DEBUG) {
            glDebugMessageCallbackKHR(&gles3_debug_message_callback, 0);
            glEnable(GL_DEBUG_OUTPUT_KHR);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_KHR);
        }
    }

    bool vsync = true;
    wglSwapIntervalEXT(vsync ? 1 : 0);

    gles3__initialise(&platform.gpu, platform.base.persistent_arena);
    frame->dpi_scale = (f32)GetDpiForWindow(window) / 96.f;

    ShowWindow(window, SW_SHOWDEFAULT);

    while (!platform.base.should_quit) {
        win32_process_messages(&platform.base.should_quit);
        if (!platform.base.should_quit) wingles3_update_and_render(&platform);
    }
}
