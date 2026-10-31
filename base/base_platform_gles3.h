// https://registry.khronos.org/OpenGL/api/GL/glcorearb.h
// https://registry.khronos.org/OpenGL/api/GLES2/gl2ext.h

// GL_VERSION_1_0
typedef unsigned int GLenum;
typedef float GLfloat;
typedef int GLint;
typedef int GLsizei;
typedef unsigned int GLbitfield;
typedef unsigned int GLuint;
typedef unsigned char GLboolean;
#define GL_DEPTH_BUFFER_BIT    0x00000100
#define GL_STENCIL_BUFFER_BIT  0x00000400
#define GL_COLOR_BUFFER_BIT    0x00004000
#define GL_FALSE               0
#define GL_TRUE                1
#define GL_TRIANGLE_STRIP      0x0005
#define GL_SRC_ALPHA           0x0302
#define GL_ONE_MINUS_SRC_ALPHA 0x0303
#define GL_INVALID_ENUM        0x0500
#define GL_INVALID_OPERATION   0x0502
#define GL_CULL_FACE           0x0B44
#define GL_DEPTH_TEST          0x0B71
#define GL_BLEND               0x0BE2
#define GL_UNPACK_ALIGNMENT    0x0CF5
#define GL_TEXTURE_2D          0x0DE1
#define GL_UNSIGNED_BYTE       0x1401
#define GL_FLOAT               0x1406
#define GL_RED                 0x1903
#define GL_LINEAR              0x2601
#define GL_TEXTURE_MAG_FILTER  0x2800
#define GL_TEXTURE_MIN_FILTER  0x2801
#define GL_TEXTURE_WRAP_S      0x2802
#define GL_TEXTURE_WRAP_T      0x2803
typedef void (*PFNGLBLENDFUNCPROC)(GLenum sfactor, GLenum dfactor);
typedef void (*PFNGLCLEARPROC) (GLbitfield mask);
typedef void (*PFNGLCLEARCOLORPROC) (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
typedef void (*PFNGLDISABLEPROC)(GLenum cap);
typedef void (*PFNGLENABLEPROC)(GLenum cap);
typedef void (*PFNGLPIXELSTOREIPROC)(GLenum pname, GLint param);
typedef void (*PFNGLTEXIMAGE2DPROC)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
typedef void (*PFNGLTEXPARAMETERIPROC)(GLenum target, GLenum pname, GLint param);
typedef void (*PFNGLVIEWPORTPROC)(GLint x, GLint y, GLsizei width, GLsizei height);

// GL_VERSION_1_1
typedef void (*PFNGLBINDTEXTUREPROC)(GLenum target, GLuint texture);
typedef void (*PFNGLGENTEXTURESPROC)(GLsizei n, GLuint *textures);

// GL_VERSION_1_2
#define GL_CLAMP_TO_EDGE 0x812F

// GL_VERSION_1_5
typedef i64 GLsizeiptr; _Static_assert(sizeof(GLsizeiptr) == sizeof(size_t), "");
#define GL_ARRAY_BUFFER 0x8892
#define GL_STREAM_DRAW  0x88E0
typedef void (*PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
typedef void (*PFNGLBUFFERDATAPROC)(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef void (*PFNGLGENBUFFERSPROC)(GLsizei n, GLuint *buffers);

// GL_VERSION_2_0
typedef char GLchar;
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER   0x8B31
#define GL_COMPILE_STATUS  0x8B81
#define GL_LINK_STATUS     0x8B82
typedef void   (*PFNGLATTACHSHADERPROC)(GLuint program, GLuint shader);
typedef void   (*PFNGLCOMPILESHADERPROC)(GLuint shader);
typedef GLuint (*PFNGLCREATEPROGRAMPROC)(void);
typedef GLuint (*PFNGLCREATESHADERPROC)(GLenum type);
typedef void   (*PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint index);
typedef void   (*PFNGLGETPROGRAMIVPROC)(GLuint program, GLenum pname, GLint *params);
typedef void   (*PFNGLGETSHADERIVPROC)(GLuint shader, GLenum pname, GLint *params);
typedef GLint  (*PFNGLGETUNIFORMLOCATIONPROC)(GLuint program, const GLchar *name);
typedef void   (*PFNGLLINKPROGRAMPROC)(GLuint program);
typedef void   (*PFNGLSHADERSOURCEPROC)(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
typedef void   (*PFNGLUNIFORM2FPROC)(GLint location, GLfloat v0, GLfloat v1);
typedef void   (*PFNGLUSEPROGRAMPROC)(GLuint program);
typedef void   (*PFNGLVERTEXATTRIBPOINTERPROC)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);

// GLES2/gl2ext.h
#define GL_DEBUG_OUTPUT_SYNCHRONOUS_KHR 0x8242
#define GL_DEBUG_OUTPUT_KHR             0x92E0
typedef void (*GLDEBUGPROCKHR)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *user_parameter);
typedef void (*PFNGLDEBUGMESSAGECALLBACKKHRPROC)(GLDEBUGPROCKHR callback, const void *user_parameter);

// GL_VERSION_3_0
#define GL_R8 0x8229
typedef void (*PFNGLBINDVERTEXARRAYPROC)(GLuint array);
typedef void (*PFNGLGENVERTEXARRAYSPROC)(GLsizei n, GLuint *arrays);

// GL_VERSION_3_1
typedef void (*PFNGLDRAWARRAYSINSTANCEDPROC)(GLenum mode, GLint first, GLsizei count, GLsizei instancecount);

// GL_VERSION_3_3
typedef void (*PFNGLVERTEXATTRIBDIVISORPROC)(GLuint index, GLuint divisor);

// X(type, name)
#define GLES3_FUNCTIONS \
    /* GL_VERSION_1_0 */ \
    X(PFNGLBLENDFUNCPROC, glBlendFunc) \
    X(PFNGLCLEARPROC, glClear) \
    X(PFNGLCLEARCOLORPROC, glClearColor) \
    X(PFNGLDISABLEPROC, glDisable) \
    X(PFNGLENABLEPROC, glEnable) \
    X(PFNGLPIXELSTOREIPROC, glPixelStorei) \
    X(PFNGLTEXIMAGE2DPROC, glTexImage2D) \
    X(PFNGLTEXPARAMETERIPROC, glTexParameteri) \
    X(PFNGLVIEWPORTPROC, glViewport) \
    /* GL_VERSION_1_1 */ \
    X(PFNGLBINDTEXTUREPROC, glBindTexture) \
    X(PFNGLGENTEXTURESPROC, glGenTextures) \
    /* GL_VERSION_1_5 */ \
    X(PFNGLBINDBUFFERPROC, glBindBuffer) \
    X(PFNGLBUFFERDATAPROC, glBufferData) \
    X(PFNGLGENBUFFERSPROC, glGenBuffers) \
    /* GL_VERSION_2_0 */ \
    X(PFNGLATTACHSHADERPROC, glAttachShader) \
    X(PFNGLCOMPILESHADERPROC, glCompileShader) \
    X(PFNGLCREATEPROGRAMPROC, glCreateProgram) \
    X(PFNGLCREATESHADERPROC, glCreateShader) \
    X(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray) \
    X(PFNGLGETPROGRAMIVPROC, glGetProgramiv) \
    X(PFNGLGETSHADERIVPROC, glGetShaderiv) \
    X(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation) \
    X(PFNGLLINKPROGRAMPROC, glLinkProgram) \
    X(PFNGLSHADERSOURCEPROC, glShaderSource) \
    X(PFNGLUNIFORM2FPROC, glUniform2f) \
    X(PFNGLUSEPROGRAMPROC, glUseProgram) \
    X(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer) \
    /* GLES2/gl2ext.h */ \
    X(PFNGLDEBUGMESSAGECALLBACKKHRPROC, glDebugMessageCallbackKHR) \
    /* GL_VERSION_3_0 */ \
    X(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray) \
    X(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays) \
    /* GL_VERSION_3_1 */ \
    X(PFNGLDRAWARRAYSINSTANCEDPROC, glDrawArraysInstanced) \
    /* GL_VERSION_3_3 */ \
    X(PFNGLVERTEXATTRIBDIVISORPROC, glVertexAttribDivisor) \

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
