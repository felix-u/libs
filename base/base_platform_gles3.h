// https://registry.khronos.org/OpenGL/api/GL/glcorearb.h
// https://registry.khronos.org/OpenGL/api/GLES2/gl2ext.h

// GL_VERSION_1_0
typedef unsigned int GLenum;
typedef float GLfloat;
typedef int GLint;
typedef int GLsizei;
typedef unsigned int GLbitfield;
typedef unsigned int GLuint;
#define GL_DEPTH_BUFFER_BIT    0x00000100
#define GL_STENCIL_BUFFER_BIT  0x00000400
#define GL_COLOR_BUFFER_BIT    0x00004000
#define GL_TRUE                1
#define GL_SRC_ALPHA           0x0302
#define GL_ONE_MINUS_SRC_ALPHA 0x0303
#define GL_CULL_FACE           0x0B44
#define GL_DEPTH_TEST          0x0B71
#define GL_BLEND               0x0BE2
typedef void (*PFNGLBLENDFUNCPROC)(GLenum sfactor, GLenum dfactor);
typedef void (*PFNGLCLEARPROC) (GLbitfield mask);
typedef void (*PFNGLCLEARCOLORPROC) (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
typedef void (*PFNGLDISABLEPROC)(GLenum cap);
typedef void (*PFNGLENABLEPROC)(GLenum cap);
typedef void (*PFNGLVIEWPORTPROC)(GLint x, GLint y, GLsizei width, GLsizei height);

// GL_VERSION_2_0
typedef char GLchar;

// GLES2/gl2ext.h
#define GL_DEBUG_OUTPUT_SYNCHRONOUS_KHR 0x8242
#define GL_DEBUG_OUTPUT_KHR             0x92E0
typedef void (*GLDEBUGPROCKHR)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *user_parameter);
typedef void (*PFNGLDEBUGMESSAGECALLBACKKHRPROC)(GLDEBUGPROCKHR callback, const void *user_parameter);

// X(type, name)
#define GLES3_FUNCTIONS \
    /* GL_VERSION_1_0 */ \
    X(PFNGLBLENDFUNCPROC, glBlendFunc) \
    X(PFNGLCLEARPROC, glClear) \
    X(PFNGLCLEARCOLORPROC, glClearColor) \
    X(PFNGLDISABLEPROC, glDisable) \
    X(PFNGLENABLEPROC, glEnable) \
    X(PFNGLVIEWPORTPROC, glViewport) \
    /* GLES2/gl2ext.h */ \
    X(PFNGLDEBUGMESSAGECALLBACKKHRPROC, glDebugMessageCallbackKHR) \

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

#define X(type, name) static type name;
GLES3_FUNCTIONS
#undef X
