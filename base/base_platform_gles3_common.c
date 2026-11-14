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
#define GL_POINTS              0x0000
#define GL_LINES               0x0001
#define GL_TRIANGLES           0x0004
#define GL_TRIANGLE_STRIP      0x0005
#define GL_LESS                0x0201
#define GL_LEQUAL              0x0203
#define GL_GREATER             0x0204
#define GL_GEQUAL              0x0206
#define GL_ONE                 1
#define GL_SRC_ALPHA           0x0302
#define GL_ONE_MINUS_SRC_ALPHA 0x0303
#define GL_FRONT               0x0404
#define GL_BACK                0x0405
#define GL_INVALID_ENUM        0x0500
#define GL_INVALID_OPERATION   0x0502
#define GL_CULL_FACE           0x0B44
#define GL_DEPTH_TEST          0x0B71
#define GL_BLEND               0x0BE2
#define GL_SCISSOR_TEST        0x0C11
#define GL_UNPACK_ROW_LENGTH   0x0CF2
#define GL_UNPACK_ALIGNMENT    0x0CF5
#define GL_TEXTURE_2D          0x0DE1
#define GL_UNSIGNED_BYTE       0x1401
#define GL_UNSIGNED_SHORT      0x1403
#define GL_UNSIGNED_INT        0x1405
#define GL_FLOAT               0x1406
#define GL_RED                 0x1903
#define GL_RGBA                0x1908
#define GL_NEAREST             0x2600
#define GL_LINEAR              0x2601
#define GL_LINEAR_MIPMAP_LINEAR 0x2703
#define GL_TEXTURE_MAG_FILTER  0x2800
#define GL_TEXTURE_MIN_FILTER  0x2801
#define GL_TEXTURE_WRAP_S      0x2802
#define GL_TEXTURE_WRAP_T      0x2803
#define GL_REPEAT              0x2901
typedef void (*PFNGLBLENDFUNCPROC)(GLenum sfactor, GLenum dfactor);
typedef void (*PFNGLCLEARPROC) (GLbitfield mask);
typedef void (*PFNGLCLEARCOLORPROC) (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
typedef void (*PFNGLCULLFACEPROC)(GLenum mode);
typedef void (*PFNGLDEPTHFUNCPROC)(GLenum func);
typedef void (*PFNGLDEPTHMASKPROC)(GLboolean flag);
typedef void (*PFNGLDISABLEPROC)(GLenum cap);
typedef void (*PFNGLENABLEPROC)(GLenum cap);
typedef void (*PFNGLGETINTEGERVPROC)(GLenum pname, GLint *data);
typedef void (*PFNGLPIXELSTOREIPROC)(GLenum pname, GLint param);
typedef void (*PFNGLREADPIXELSPROC)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels);
typedef void (*PFNGLSCISSORPROC)(GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (*PFNGLTEXIMAGE2DPROC)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
typedef void (*PFNGLTEXPARAMETERIPROC)(GLenum target, GLenum pname, GLint param);
typedef void (*PFNGLVIEWPORTPROC)(GLint x, GLint y, GLsizei width, GLsizei height);

// GL_VERSION_1_1
#define GL_RGBA8 0x8058
typedef void (*PFNGLBINDTEXTUREPROC)(GLenum target, GLuint texture);
typedef void (*PFNGLDELETETEXTURESPROC)(GLsizei n, const GLuint *textures);
typedef void (*PFNGLGENTEXTURESPROC)(GLsizei n, GLuint *textures);
typedef void (*PFNGLTEXSUBIMAGE2DPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);

// GL_VERSION_1_2
#define GL_CLAMP_TO_EDGE 0x812F

// GL_VERSION_1_3
#define GL_TEXTURE0 0x84C0
typedef void (*PFNGLACTIVETEXTUREPROC)(GLenum texture);

// GL_VERSION_1_4
#define GL_MIRRORED_REPEAT 0x8370

// GL_VERSION_1_5
#if BASE_OS == BASE_OS_WASM
    typedef i32 GLsizeiptr;
    typedef i32 GLintptr;
#else
    typedef i64 GLsizeiptr;
    typedef i64 GLintptr;
#endif
_Static_assert(sizeof(GLsizeiptr) == sizeof(size_t) && sizeof(GLintptr) == sizeof(size_t), "GL pointer-sized integers match the target");
#define GL_ARRAY_BUFFER         0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_STREAM_DRAW          0x88E0
#define GL_STATIC_DRAW          0x88E4
#define GL_DYNAMIC_DRAW         0x88E8
typedef void (*PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
typedef void (*PFNGLBUFFERDATAPROC)(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef void (*PFNGLDELETEBUFFERSPROC)(GLsizei n, const GLuint *buffers);
typedef void (*PFNGLGENBUFFERSPROC)(GLsizei n, GLuint *buffers);

// GL_VERSION_2_0
typedef char GLchar;
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER   0x8B31
#define GL_COMPILE_STATUS  0x8B81
#define GL_LINK_STATUS     0x8B82
#define GL_INFO_LOG_LENGTH 0x8B84
typedef void   (*PFNGLATTACHSHADERPROC)(GLuint program, GLuint shader);
typedef void   (*PFNGLCOMPILESHADERPROC)(GLuint shader);
typedef GLuint (*PFNGLCREATEPROGRAMPROC)(void);
typedef GLuint (*PFNGLCREATESHADERPROC)(GLenum type);
typedef void   (*PFNGLDELETEPROGRAMPROC)(GLuint program);
typedef void   (*PFNGLDELETESHADERPROC)(GLuint shader);
typedef void   (*PFNGLDISABLEVERTEXATTRIBARRAYPROC)(GLuint index);
typedef void   (*PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint index);
typedef void   (*PFNGLGETPROGRAMINFOLOGPROC)(GLuint program, GLsizei buffer_size, GLsizei *length, GLchar *info_log);
typedef void   (*PFNGLGETPROGRAMIVPROC)(GLuint program, GLenum pname, GLint *params);
typedef void   (*PFNGLGETSHADERINFOLOGPROC)(GLuint shader, GLsizei buffer_size, GLsizei *length, GLchar *info_log);
typedef void   (*PFNGLGETSHADERIVPROC)(GLuint shader, GLenum pname, GLint *params);
typedef GLint  (*PFNGLGETUNIFORMLOCATIONPROC)(GLuint program, const GLchar *name);
typedef void   (*PFNGLLINKPROGRAMPROC)(GLuint program);
typedef void   (*PFNGLSHADERSOURCEPROC)(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
typedef void   (*PFNGLUNIFORM1IPROC)(GLint location, GLint v0);
typedef void   (*PFNGLUNIFORM2FPROC)(GLint location, GLfloat v0, GLfloat v1);
typedef void   (*PFNGLUSEPROGRAMPROC)(GLuint program);
typedef void   (*PFNGLVERTEXATTRIBPOINTERPROC)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);

// GLES2/gl2ext.h
#define GL_FRAMEBUFFER_SRGB_EXT         0x8DB9
#define GL_DEBUG_OUTPUT_SYNCHRONOUS_KHR 0x8242
#define GL_DEBUG_OUTPUT_KHR             0x92E0
typedef void (*GLDEBUGPROCKHR)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *user_parameter);
typedef void (*PFNGLDEBUGMESSAGECALLBACKKHRPROC)(GLDEBUGPROCKHR callback, const void *user_parameter);

// GL_VERSION_3_0
#define GL_HALF_FLOAT               0x140B
#define GL_DEPTH_STENCIL_ATTACHMENT 0x821A
#define GL_R8                       0x8229
#define GL_RGBA16F                  0x881A
#define GL_DEPTH24_STENCIL8         0x88F0
#define GL_FRAMEBUFFER_COMPLETE     0x8CD5
#define GL_COLOR_ATTACHMENT0        0x8CE0
#define GL_FRAMEBUFFER              0x8D40
#define GL_RENDERBUFFER             0x8D41
typedef void   (*PFNGLBINDBUFFERRANGEPROC)(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
typedef void   (*PFNGLBINDFRAMEBUFFERPROC)(GLenum target, GLuint framebuffer);
typedef void   (*PFNGLBINDRENDERBUFFERPROC)(GLenum target, GLuint renderbuffer);
typedef void   (*PFNGLBINDVERTEXARRAYPROC)(GLuint array);
typedef GLenum (*PFNGLCHECKFRAMEBUFFERSTATUSPROC)(GLenum target);
typedef void   (*PFNGLDELETEFRAMEBUFFERSPROC)(GLsizei n, const GLuint *framebuffers);
typedef void   (*PFNGLDELETERENDERBUFFERSPROC)(GLsizei n, const GLuint *renderbuffers);
typedef void   (*PFNGLFRAMEBUFFERRENDERBUFFERPROC)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
typedef void   (*PFNGLFRAMEBUFFERTEXTURE2DPROC)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef void   (*PFNGLGENERATEMIPMAPPROC)(GLenum target);
typedef void   (*PFNGLGENFRAMEBUFFERSPROC)(GLsizei n, GLuint *framebuffers);
typedef void   (*PFNGLGENRENDERBUFFERSPROC)(GLsizei n, GLuint *renderbuffers);
typedef void   (*PFNGLGENVERTEXARRAYSPROC)(GLsizei n, GLuint *arrays);
typedef void   (*PFNGLRENDERBUFFERSTORAGEPROC)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
typedef void   (*PFNGLVERTEXATTRIBIPOINTERPROC)(GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer);

// GL_VERSION_3_1
#define GL_UNIFORM_BUFFER                  0x8A11
#define GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT 0x8A34
#define GL_UNIFORM_BLOCK_DATA_SIZE         0x8A40
#define GL_INVALID_INDEX                   0xFFFFFFFFu
typedef void   (*PFNGLDRAWARRAYSINSTANCEDPROC)(GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
typedef void   (*PFNGLDRAWELEMENTSINSTANCEDPROC)(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount);
typedef void   (*PFNGLGETACTIVEUNIFORMBLOCKIVPROC)(GLuint program, GLuint uniform_block_index, GLenum pname, GLint *params);
typedef GLuint (*PFNGLGETUNIFORMBLOCKINDEXPROC)(GLuint program, const GLchar *uniform_block_name);
typedef void   (*PFNGLUNIFORMBLOCKBINDINGPROC)(GLuint program, GLuint uniform_block_index, GLuint uniform_block_binding);

// GL_VERSION_3_3
typedef void (*PFNGLVERTEXATTRIBDIVISORPROC)(GLuint index, GLuint divisor);

// X(type, name)
#define GLES3_FUNCTIONS \
    /* GL_VERSION_1_0 */ \
    X(PFNGLBLENDFUNCPROC, glBlendFunc) \
    X(PFNGLCLEARPROC, glClear) \
    X(PFNGLCLEARCOLORPROC, glClearColor) \
    X(PFNGLCULLFACEPROC, glCullFace) \
    X(PFNGLDEPTHFUNCPROC, glDepthFunc) \
    X(PFNGLDEPTHMASKPROC, glDepthMask) \
    X(PFNGLDISABLEPROC, glDisable) \
    X(PFNGLENABLEPROC, glEnable) \
    X(PFNGLGETINTEGERVPROC, glGetIntegerv) \
    X(PFNGLPIXELSTOREIPROC, glPixelStorei) \
    X(PFNGLREADPIXELSPROC, glReadPixels) \
    X(PFNGLSCISSORPROC, glScissor) \
    X(PFNGLTEXIMAGE2DPROC, glTexImage2D) \
    X(PFNGLTEXPARAMETERIPROC, glTexParameteri) \
    X(PFNGLVIEWPORTPROC, glViewport) \
    /* GL_VERSION_1_1 */ \
    X(PFNGLBINDTEXTUREPROC, glBindTexture) \
    X(PFNGLDELETETEXTURESPROC, glDeleteTextures) \
    X(PFNGLGENTEXTURESPROC, glGenTextures) \
    X(PFNGLTEXSUBIMAGE2DPROC, glTexSubImage2D) \
    /* GL_VERSION_1_3 */ \
    X(PFNGLACTIVETEXTUREPROC, glActiveTexture) \
    /* GL_VERSION_1_5 */ \
    X(PFNGLBINDBUFFERPROC, glBindBuffer) \
    X(PFNGLBUFFERDATAPROC, glBufferData) \
    X(PFNGLDELETEBUFFERSPROC, glDeleteBuffers) \
    X(PFNGLGENBUFFERSPROC, glGenBuffers) \
    /* GL_VERSION_2_0 */ \
    X(PFNGLATTACHSHADERPROC, glAttachShader) \
    X(PFNGLCOMPILESHADERPROC, glCompileShader) \
    X(PFNGLCREATEPROGRAMPROC, glCreateProgram) \
    X(PFNGLCREATESHADERPROC, glCreateShader) \
    X(PFNGLDELETEPROGRAMPROC, glDeleteProgram) \
    X(PFNGLDELETESHADERPROC, glDeleteShader) \
    X(PFNGLDISABLEVERTEXATTRIBARRAYPROC, glDisableVertexAttribArray) \
    X(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray) \
    X(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog) \
    X(PFNGLGETPROGRAMIVPROC, glGetProgramiv) \
    X(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog) \
    X(PFNGLGETSHADERIVPROC, glGetShaderiv) \
    X(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation) \
    X(PFNGLLINKPROGRAMPROC, glLinkProgram) \
    X(PFNGLSHADERSOURCEPROC, glShaderSource) \
    X(PFNGLUNIFORM1IPROC, glUniform1i) \
    X(PFNGLUNIFORM2FPROC, glUniform2f) \
    X(PFNGLUSEPROGRAMPROC, glUseProgram) \
    X(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer) \
    /* GLES2/gl2ext.h */ \
    X(PFNGLDEBUGMESSAGECALLBACKKHRPROC, glDebugMessageCallbackKHR) \
    /* GL_VERSION_3_0 */ \
    X(PFNGLBINDBUFFERRANGEPROC, glBindBufferRange) \
    X(PFNGLBINDFRAMEBUFFERPROC, glBindFramebuffer) \
    X(PFNGLBINDRENDERBUFFERPROC, glBindRenderbuffer) \
    X(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray) \
    X(PFNGLCHECKFRAMEBUFFERSTATUSPROC, glCheckFramebufferStatus) \
    X(PFNGLDELETEFRAMEBUFFERSPROC, glDeleteFramebuffers) \
    X(PFNGLDELETERENDERBUFFERSPROC, glDeleteRenderbuffers) \
    X(PFNGLFRAMEBUFFERRENDERBUFFERPROC, glFramebufferRenderbuffer) \
    X(PFNGLFRAMEBUFFERTEXTURE2DPROC, glFramebufferTexture2D) \
    X(PFNGLGENERATEMIPMAPPROC, glGenerateMipmap) \
    X(PFNGLGENFRAMEBUFFERSPROC, glGenFramebuffers) \
    X(PFNGLGENRENDERBUFFERSPROC, glGenRenderbuffers) \
    X(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays) \
    X(PFNGLRENDERBUFFERSTORAGEPROC, glRenderbufferStorage) \
    X(PFNGLVERTEXATTRIBIPOINTERPROC, glVertexAttribIPointer) \
    /* GL_VERSION_3_1 */ \
    X(PFNGLDRAWARRAYSINSTANCEDPROC, glDrawArraysInstanced) \
    X(PFNGLDRAWELEMENTSINSTANCEDPROC, glDrawElementsInstanced) \
    X(PFNGLGETACTIVEUNIFORMBLOCKIVPROC, glGetActiveUniformBlockiv) \
    X(PFNGLGETUNIFORMBLOCKINDEXPROC, glGetUniformBlockIndex) \
    X(PFNGLUNIFORMBLOCKBINDINGPROC, glUniformBlockBinding) \
    /* GL_VERSION_3_3 */ \
    X(PFNGLVERTEXATTRIBDIVISORPROC, glVertexAttribDivisor) \

#if defined(BASE_PLATFORM_WASM)
    #define X(type, name) __typeof__(*(type)0) name;
#else
    #define X(type, name) static type name;
#endif
GLES3_FUNCTIONS
#undef X

typedef struct {
    u32 slot;
    bool was_found;
} GPU_Cache_Lookup;

typedef struct {
    GLuint program;
    GLuint arguments_block_index;
    bool uses_default_vertex_shader;
    GLint arguments_block_size;
} GLES3_Shader;

typedef struct {
    bool is_streamed;
    u64 stream_frame;
    u64 stream_vertices_offset;
    u64 stream_indices_offset;
    u64 byte_size;
    GLuint vertex_buffer;
    GLuint index_buffer;
    u32 vertex_count;
    u32 index_count;
    u32 vertex_stride;
    GPU_Index_Format index_format;
    GPU_Vertex_Format vertex_attributes[GPU_MAX_VERTEX_ATTRIBUTES];
} GLES3_Mesh;

typedef struct {
    GLuint texture;
    GLuint framebuffer;
    GLuint depth_buffer;
    u32 width;
    u32 height;
    GPU_Texture_Format format;
    GPU_Texture_Flags flags;
    u64 byte_size;
} GLES3_Texture;

typedef struct {
    u8 *data;
    u64 size;
    u64 capacity;
    GLuint buffer;
    GLenum target;
} GLES3_Stream;

typedef enum {
    GLES3_Stream_Kind_VERTEX,
    GLES3_Stream_Kind_INDEX,
    GLES3_Stream_Kind_UNIFORM,

    GLES3_Stream_Kind_COUNT,
} GLES3_Stream_Kind;

typedef enum {
    GLES3_Command_Kind_CLEAR,
    GLES3_Command_Kind_DRAW,
} GLES3_Command_Kind;

typedef struct {
    GLES3_Command_Kind kind;
    bool has_target;
    u32 target_slot;
    u32 clear_color;

    GPU_Draw draw;
    u32 shader_slot;
    bool has_mesh;
    u32 mesh_slot;
    u32 texture_slots[GPU_MAX_TEXTURES_PER_DRAW];
    u64 uniforms_offset;
    u64 arguments_offset;
    u64 instances_offset;
    u32 instance_stride;
} GLES3_Command;

typedef struct {
    M4 transform;
    V4 rectangle;
    V2 target_size;
    V2 padding;
} GLES3_Draw_Uniforms;
_Static_assert(sizeof(GLES3_Draw_Uniforms) == 96, "matches the std140 layout of gpu_draw_uniforms");

typedef enum {
    GLES3_Uniform_Binding_DRAW = 0,
    GLES3_Uniform_Binding_ARGUMENTS = 1,
} GLES3_Uniform_Binding;

typedef struct {
    bool is_ready;
    u64 frame_index;
    u64 uniform_offset_alignment;
    GLuint vertex_array;
    u32 enabled_vertex_attributes;
    u64 memory_limit_in_bytes;
    u64 cached_bytes;

    GPU_Cache shader_cache;
    GLES3_Shader *shaders;
    GPU_Cache mesh_cache;
    GLES3_Mesh *meshes;
    GPU_Cache texture_cache;
    GLES3_Texture *textures;

    GLES3_Command *commands;
    u64 command_count;
    u64 command_capacity;

    GLES3_Stream streams[GLES3_Stream_Kind_COUNT];

    GPU_Frame_Stats stats;
} GLES3_GPU;

static GLES3_GPU *gles3__gpu_from_platform(Platform *platform);

static V2 platform_measure_text(Platform *platform, String text, f32 font_size) {
    (void)platform;
    (void)text;
    (void)font_size;
    panic("TODO(felix)");
    return (V2){0};
}

static void gles3__command_push(Platform *platform, const GLES3_Command *command) {
    GLES3_GPU *gpu = gles3__gpu_from_platform(platform);
    gpu->commands = arena_array_reserve(platform->frame_arena, gpu->commands, gpu->command_count, &gpu->command_capacity, gpu->command_count + 1);
    gpu->commands[gpu->command_count] = *command;
    gpu->command_count += 1;
}

static void gpu_clear(Platform *platform, GPU_Texture_Id target, u32 color) {
    GLES3_GPU *gpu = gles3__gpu_from_platform(platform);
    assert(gpu->is_ready);

    bool target_ok = true;
    u32 target_slot = 0;
    bool has_target = target != 0;
    if (has_target) {
        target_slot = gpu__slot_from_id(&gpu->texture_cache, target, &target_ok);
        target_ok = target_ok && !!(gpu->textures[target_slot].flags & GPU_Texture_Flag_RENDER_TARGET);
    }

    if (target_ok) {
        if (has_target) gpu__cache_mark_used(&gpu->texture_cache, target_slot, gpu->frame_index);

        GLES3_Command command = {
            .kind = GLES3_Command_Kind_CLEAR,
            .has_target = has_target,
            .target_slot = target_slot,
            .clear_color = color,
        };
        gles3__command_push(platform, &command);
    } else {
        gpu->stats.failed_draws += 1;
    }
}

static u64 gles3__stream_push(Arena *frame_arena, GLES3_Stream *stream, const void *data, u64 size, u64 alignment) {
    u64 offset = (stream->size + alignment - 1) / alignment * alignment;
    u64 new_size = offset + size;
    stream->data = arena_array_reserve(frame_arena, stream->data, stream->size, &stream->capacity, new_size);
    memset(&stream->data[stream->size], 0, size_t_from_u64(offset - stream->size));
    memcpy(&stream->data[offset], data, size_t_from_u64(size));
    stream->size = new_size;
    return offset;
}

static void gpu_draw(Platform *platform, const GPU_Draw *draw) {
    GLES3_GPU *gpu = gles3__gpu_from_platform(platform);
    assert(gpu->is_ready);

    bool shader_ok = true;
    u32 shader_slot = gpu__slot_from_id(&gpu->shader_cache, draw->shader, &shader_ok);

    bool mesh_ok = true;
    u32 mesh_slot = 0;
    bool has_mesh = draw->mesh != 0;
    if (has_mesh) {
        mesh_slot = gpu__slot_from_id(&gpu->mesh_cache, draw->mesh, &mesh_ok);
        const GLES3_Mesh *mesh = &gpu->meshes[mesh_slot];
        bool stream_is_current = !mesh->is_streamed || mesh->stream_frame == gpu->frame_index;
        mesh_ok = mesh_ok && stream_is_current;
    }

    bool textures_ok = true;
    u32 texture_slots[GPU_MAX_TEXTURES_PER_DRAW] = {0};
    for (u32 i = 0; i < GPU_MAX_TEXTURES_PER_DRAW; i += 1) {
        if (draw->textures[i] != 0) {
            bool texture_ok = true;
            texture_slots[i] = gpu__slot_from_id(&gpu->texture_cache, draw->textures[i], &texture_ok);
            textures_ok = textures_ok && texture_ok;
        }
    }

    bool target_ok = true;
    u32 target_slot = 0;
    bool has_target = draw->target != 0;
    V2 target_size = platform->frame.window_size;
    if (has_target) {
        target_slot = gpu__slot_from_id(&gpu->texture_cache, draw->target, &target_ok);
        const GLES3_Texture *target = &gpu->textures[target_slot];
        target_ok = target_ok && !!(target->flags & GPU_Texture_Flag_RENDER_TARGET);
        target_size = (V2){ .x = (f32)target->width, .y = (f32)target->height };
    }

    u32 instance_stride = 0;
    for (u32 i = 0; i < GPU_MAX_INSTANCE_ATTRIBUTES; i += 1) instance_stride += gpu_vertex_format_size[draw->instance_attributes[i]];
    bool instances_ok = draw->instances.size == 0 || instance_stride > 0;

    bool draw_ok = shader_ok && mesh_ok && textures_ok && target_ok && instances_ok;
    if (draw_ok) {
        const GLES3_Shader *shader = &gpu->shaders[shader_slot];
        bool shader_has_arguments = shader->arguments_block_index != GL_INVALID_INDEX;
        u64 expected_arguments_size = shader_has_arguments ? (u64)shader->arguments_block_size : 0;
        assert(draw->arguments.size == expected_arguments_size);

        for (u32 i = 0; i < GPU_MAX_TEXTURES_PER_DRAW; i += 1) assert(draw->textures[i] == 0 || draw->textures[i] != draw->target);

        assert(!draw->depth_write || draw->depth_test != GPU_Depth_Test_NONE);
        bool target_has_depth_buffer = !has_target || !!(gpu->textures[target_slot].flags & GPU_Texture_Flag_DEPTH_BUFFER);
        assert(draw->depth_test == GPU_Depth_Test_NONE || target_has_depth_buffer);

        assert(has_mesh || draw->topology == GPU_Topology_TRIANGLES);
        assert(!has_mesh || !shader->uses_default_vertex_shader);

        bool has_instances = draw->instances.size > 0;
        assert(has_instances || instance_stride == 0);
        assert(!has_instances || draw->instances.size % instance_stride == 0);
        assert(!has_instances || draw->instance_count <= draw->instances.size / instance_stride);

        M4 transform = draw->transform;
        bool transform_is_zero = memcmp(&transform, &(M4){0}, sizeof transform) == 0;
        if (transform_is_zero) {
            f32 y_direction = has_target ? 1.f : -1.f;
            transform.columns[0] = (V4){ .x = 2.f / target_size.x, .y = 0, .z = 0, .w = 0 };
            transform.columns[1] = (V4){ .x = 0, .y = y_direction * 2.f / target_size.y, .z = 0, .w = 0 };
            transform.columns[2] = (V4){ .x = 0, .y = 0, .z = 1, .w = 0 };
            transform.columns[3] = (V4){ .x = -1, .y = -y_direction, .z = 0, .w = 1 };
        }

        GLES3_Draw_Uniforms uniforms = {
            .transform = transform,
            .rectangle = draw->rectangle,
            .target_size = target_size,
        };

        GLES3_Command command = {
            .kind = GLES3_Command_Kind_DRAW,
            .has_target = has_target,
            .target_slot = target_slot,
            .draw = *draw,
            .shader_slot = shader_slot,
            .has_mesh = has_mesh,
            .mesh_slot = mesh_slot,
            .instance_stride = instance_stride,
        };
        memcpy(command.texture_slots, texture_slots, sizeof texture_slots);
        command.draw.arguments.data = 0;
        command.draw.instances.data = 0;

        command.uniforms_offset = gles3__stream_push(platform->frame_arena, &gpu->streams[GLES3_Stream_Kind_UNIFORM], &uniforms, sizeof uniforms, gpu->uniform_offset_alignment);
        if (draw->arguments.size > 0) {
            command.arguments_offset = gles3__stream_push(platform->frame_arena, &gpu->streams[GLES3_Stream_Kind_UNIFORM], draw->arguments.data, draw->arguments.size, gpu->uniform_offset_alignment);
        }
        if (draw->instances.size > 0) {
            command.instances_offset = gles3__stream_push(platform->frame_arena, &gpu->streams[GLES3_Stream_Kind_VERTEX], draw->instances.data, draw->instances.size, 16);
        }
        gles3__command_push(platform, &command);

        gpu__cache_mark_used(&gpu->shader_cache, shader_slot, gpu->frame_index);
        if (has_mesh) gpu__cache_mark_used(&gpu->mesh_cache, mesh_slot, gpu->frame_index);
        if (has_target) gpu__cache_mark_used(&gpu->texture_cache, target_slot, gpu->frame_index);
        for (u32 i = 0; i < GPU_MAX_TEXTURES_PER_DRAW; i += 1) {
            if (draw->textures[i] != 0) gpu__cache_mark_used(&gpu->texture_cache, texture_slots[i], gpu->frame_index);
        }
    } else {
        gpu->stats.failed_draws += 1;
    }
}

static void gles3__texture_delete(GLES3_GPU *gpu, GLES3_Texture *texture) {
    if (texture->framebuffer != 0) glDeleteFramebuffers(1, &texture->framebuffer);
    if (texture->depth_buffer != 0) glDeleteRenderbuffers(1, &texture->depth_buffer);
    if (texture->texture != 0) glDeleteTextures(1, &texture->texture);
    gpu->cached_bytes -= texture->byte_size;
    *texture = (GLES3_Texture){0};
}

static void gles3__evict_oldest(GLES3_GPU *gpu, GPU_Cache *cache) {
    u32 slot = gpu__cache_oldest_evictable_slot(cache);
    bool can_evict = slot != 0 && cache->entries[slot].last_used_frame != gpu->frame_index;
    if (!can_evict) panic("every cached GPU resource is a render target or was used this frame, so none can be evicted; raise GPU_MAX_*_EXPONENT or gpu.memory_limit_in_bytes");

    if (cache == &gpu->shader_cache) {
        glDeleteProgram(gpu->shaders[slot].program);
        gpu->shaders[slot] = (GLES3_Shader){0};
    } else if (cache == &gpu->mesh_cache) {
        GLES3_Mesh *mesh = &gpu->meshes[slot];
        if (mesh->vertex_buffer != 0) glDeleteBuffers(1, &mesh->vertex_buffer);
        if (mesh->index_buffer != 0) glDeleteBuffers(1, &mesh->index_buffer);
        gpu->cached_bytes -= mesh->byte_size;
        *mesh = (GLES3_Mesh){0};
    } else {
        gles3__texture_delete(gpu, &gpu->textures[slot]);
    }

    gpu__cache_remove(cache, slot);
    gpu->stats.evictions += 1;
}

static void gles3__make_bytes_available(GLES3_GPU *gpu, u64 byte_count) {
    while (gpu->cached_bytes + byte_count > gpu->memory_limit_in_bytes) {
        u32 oldest_mesh = gpu__cache_oldest_evictable_slot(&gpu->mesh_cache);
        u32 oldest_texture = gpu__cache_oldest_evictable_slot(&gpu->texture_cache);
        u64 mesh_frame = oldest_mesh != 0 ? gpu->mesh_cache.entries[oldest_mesh].last_used_frame : ~0ull;
        u64 texture_frame = oldest_texture != 0 ? gpu->texture_cache.entries[oldest_texture].last_used_frame : ~0ull;
        gles3__evict_oldest(gpu, mesh_frame <= texture_frame ? &gpu->mesh_cache : &gpu->texture_cache);
    }
}

static GPU_Cache_Lookup gles3__cache_find_or_insert(GLES3_GPU *gpu, GPU_Cache *cache, u64 key, bool is_keyed_by_content, bool may_insert, bool *ok) {
    GPU_Cache_Lookup lookup = { .slot = gpu__cache_find(cache, key, is_keyed_by_content) };
    lookup.was_found = lookup.slot != 0;

    if (lookup.was_found) gpu__cache_mark_used(cache, lookup.slot, gpu->frame_index);
    else if (may_insert) {
        if (cache->entries[cache->oldest_slot].is_used) gles3__evict_oldest(gpu, cache);
        lookup.slot = gpu__cache_insert(cache, key, is_keyed_by_content, gpu->frame_index);
    }

    *ok = lookup.slot != 0;
    return lookup;
}

static GPU_Mesh_Id gpu_mesh_id(Platform *platform, const GPU_Mesh *mesh) {
    GLES3_GPU *gpu = gles3__gpu_from_platform(platform);
    assert(gpu->is_ready);

    u32 vertex_stride = 0;
    for (u32 i = 0; i < GPU_MAX_VERTEX_ATTRIBUTES; i += 1) vertex_stride += gpu_vertex_format_size[mesh->vertex_attributes[i]];
    u32 index_size = gpu_index_format_size[mesh->index_format];

    bool lookup_ok = mesh->vertices.size > 0 && vertex_stride > 0 && (mesh->indices.size == 0 || index_size > 0);

    GPU_Cache_Lookup lookup = {0};
    if (lookup_ok) {
        u64 content_hash = ds_hash_fnv1a_64(mesh->vertices.data, mesh->vertices.size);
        content_hash = ds_hash_fnv1a_64_update(content_hash, mesh->indices.data, mesh->indices.size);
        content_hash = ds_hash_fnv1a_64_update(content_hash, mesh->vertex_attributes, sizeof mesh->vertex_attributes);
        content_hash = ds_hash_fnv1a_64_update(content_hash, &mesh->index_format, sizeof mesh->index_format);
        lookup = gles3__cache_find_or_insert(gpu, &gpu->mesh_cache, content_hash, true, true, &lookup_ok);
    }

    GPU_Mesh_Id id = 0;
    if (lookup_ok) {
        GLES3_Mesh *gles3_mesh = &gpu->meshes[lookup.slot];
        if (!lookup.was_found) *gles3_mesh = (GLES3_Mesh){0};

        if (lookup.was_found) gpu->stats.cache_hits += 1;
        else gpu->stats.cache_misses += 1;

        bool should_promote = lookup.was_found && gles3_mesh->is_streamed && gles3_mesh->stream_frame != gpu->frame_index;

        if (should_promote) {
            u64 byte_size = mesh->vertices.size + mesh->indices.size;
            gles3__make_bytes_available(gpu, byte_size);
            gles3_mesh->byte_size = byte_size;
            gpu->cached_bytes += byte_size;

            glGenBuffers(1, &gles3_mesh->vertex_buffer);
            glBindBuffer(GL_ARRAY_BUFFER, gles3_mesh->vertex_buffer);
            glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)mesh->vertices.size, mesh->vertices.data, GL_STATIC_DRAW);

            if (mesh->indices.size > 0) {
                glGenBuffers(1, &gles3_mesh->index_buffer);
                glBindVertexArray(gpu->vertex_array);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gles3_mesh->index_buffer);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)mesh->indices.size, mesh->indices.data, GL_STATIC_DRAW);
            }

            gles3_mesh->is_streamed = false;
            gpu->stats.bytes_uploaded += byte_size;
            gpu->stats.promotions += 1;
        }

        if (!lookup.was_found) {
            gles3_mesh->vertex_count = (u32)(mesh->vertices.size / vertex_stride);
            gles3_mesh->index_count = index_size > 0 ? (u32)(mesh->indices.size / index_size) : 0;
            gles3_mesh->vertex_stride = vertex_stride;
            gles3_mesh->index_format = mesh->index_format;
            memcpy(gles3_mesh->vertex_attributes, mesh->vertex_attributes, sizeof mesh->vertex_attributes);
            gles3_mesh->is_streamed = true;
            gles3_mesh->stream_frame = gpu->frame_index;
            gles3_mesh->stream_vertices_offset = gles3__stream_push(platform->frame_arena, &gpu->streams[GLES3_Stream_Kind_VERTEX], mesh->vertices.data, mesh->vertices.size, 16);
            if (mesh->indices.size > 0) {
                gles3_mesh->stream_indices_offset = gles3__stream_push(platform->frame_arena, &gpu->streams[GLES3_Stream_Kind_INDEX], mesh->indices.data, mesh->indices.size, 4);
            }
        }

        id = gpu__id_from_slot(&gpu->mesh_cache, lookup.slot);
    }

    return id;
}

enum { GLES3_MAX_SHADER_STAGE_STRINGS = 5 };

typedef struct {
    const GLchar *strings[GLES3_MAX_SHADER_STAGE_STRINGS];
    GLint lengths[GLES3_MAX_SHADER_STAGE_STRINGS];
    GLsizei count;
} GLES3_Shader_Stage_Source;

static void gles3__shader_stage_source_add(GLES3_Shader_Stage_Source *source, String text) {
    assert(source->count < GLES3_MAX_SHADER_STAGE_STRINGS);
    source->strings[source->count] = text.data;
    source->lengths[source->count] = (GLint)text.count;
    source->count += 1;
}

static GPU_Shader_Id gpu_shader_id(Platform *platform, const GPU_Shader *shader, bool *ok) {
    GLES3_GPU *gpu = gles3__gpu_from_platform(platform);
    assert(gpu->is_ready);

    bool shader_ok = shader->fragment_source.count > 0;

    GPU_Cache_Lookup lookup = {0};
    if (shader_ok) {
        u64 content_hash = ds_hash_fnv1a_64(&shader->vertex_source.count, sizeof shader->vertex_source.count);
        content_hash = ds_hash_fnv1a_64_update(content_hash, shader->vertex_source.data, shader->vertex_source.count);
        content_hash = ds_hash_fnv1a_64_update(content_hash, shader->fragment_source.data, shader->fragment_source.count);
        lookup = gles3__cache_find_or_insert(gpu, &gpu->shader_cache, content_hash, true, true, &shader_ok);
    }

    bool should_compile = shader_ok && !lookup.was_found;
    if (shader_ok && !should_compile) gpu->stats.cache_hits += 1;

    if (should_compile) {
        gpu->stats.cache_misses += 1;
        gpu->stats.shader_compiles += 1;

        // TODO(felix): Cache linked programs on disk with glGetProgramBinary and glProgramBinary? (GLES 3.0 core, not in WebGL2). Could speed up time-to-first-frame.

        String prelude = S("#version 300 es\n" STRINGIFY(
            precision highp float;
            precision highp int;
            layout(std140) uniform gpu_draw_uniforms {
                mat4 gpu_transform;
                vec4 gpu_rectangle;
                vec2 gpu_target_size;
            };
            uniform sampler2D gpu_texture_0;
            uniform sampler2D gpu_texture_1;
            uniform sampler2D gpu_texture_2;
            uniform sampler2D gpu_texture_3;
        ) "\n");
        String default_vertex_source = S(STRINGIFY(
            out vec2 gpu_uv;
            void main() {
                vec2 corner = vec2(gl_VertexID & 1, gl_VertexID >> 1);
                gpu_uv = corner;
                gl_Position = gpu_transform * vec4(mix(gpu_rectangle.xy, gpu_rectangle.zw, corner), 0.0, 1.0);
            }
        ) "\n");
        String fragment_output = S("out vec4 gpu_color_out;\n");
        String default_vertex_fragment_input = S("in vec2 gpu_uv;\n");
        String line_reset = S("#line 1\n");
        _Static_assert(GPU_MAX_TEXTURES_PER_DRAW == 4, "the prelude declares gpu_texture_0 to gpu_texture_3");

        bool uses_default_vertex_shader = shader->vertex_source.count == 0;

        enum { STAGE_VERTEX = 0, STAGE_FRAGMENT = 1, STAGE_COUNT = 2 };
        GLES3_Shader_Stage_Source stage_sources[STAGE_COUNT] = {0};

        gles3__shader_stage_source_add(&stage_sources[STAGE_VERTEX], prelude);
        if (uses_default_vertex_shader) {
            gles3__shader_stage_source_add(&stage_sources[STAGE_VERTEX], default_vertex_source);
        } else {
            gles3__shader_stage_source_add(&stage_sources[STAGE_VERTEX], line_reset);
            gles3__shader_stage_source_add(&stage_sources[STAGE_VERTEX], shader->vertex_source);
        }
        gles3__shader_stage_source_add(&stage_sources[STAGE_FRAGMENT], prelude);
        gles3__shader_stage_source_add(&stage_sources[STAGE_FRAGMENT], fragment_output);
        if (uses_default_vertex_shader) gles3__shader_stage_source_add(&stage_sources[STAGE_FRAGMENT], default_vertex_fragment_input);
        gles3__shader_stage_source_add(&stage_sources[STAGE_FRAGMENT], line_reset);
        gles3__shader_stage_source_add(&stage_sources[STAGE_FRAGMENT], shader->fragment_source);

        static const GLenum gl_stage_kind[STAGE_COUNT] = { [STAGE_VERTEX] = GL_VERTEX_SHADER, [STAGE_FRAGMENT] = GL_FRAGMENT_SHADER };
        static const char *stage_name[STAGE_COUNT] = { [STAGE_VERTEX] = "vertex", [STAGE_FRAGMENT] = "fragment" };

        GLuint program = glCreateProgram();
        ensure(program != 0);

        bool compile_ok = true;
        GLuint stage_shaders[STAGE_COUNT] = {0};
        for (u32 stage = 0; stage < STAGE_COUNT; stage += 1) {
            GLuint stage_shader = glCreateShader(gl_stage_kind[stage]);
            ensure(stage_shader != 0);
            glShaderSource(stage_shader, stage_sources[stage].count, stage_sources[stage].strings, stage_sources[stage].lengths);
            glCompileShader(stage_shader);

            GLint status = 0;
            glGetShaderiv(stage_shader, GL_COMPILE_STATUS, &status);
            if (!status) {
                GLint log_length = 0;
                glGetShaderiv(stage_shader, GL_INFO_LOG_LENGTH, &log_length);
                char *log = arena_make(platform->frame_arena, (size_t)MAX(log_length, 1), char);
                glGetShaderInfoLog(stage_shader, MAX(log_length, 1), 0, log);
                log_error("%s shader failed to compile:\n%s", stage_name[stage], log);
                compile_ok = false;
            }

            glAttachShader(program, stage_shader);
            stage_shaders[stage] = stage_shader;
        }

        bool link_ok = false;
        if (compile_ok) {
            glLinkProgram(program);
            GLint status = 0;
            glGetProgramiv(program, GL_LINK_STATUS, &status);
            link_ok = !!status;
            if (!link_ok) {
                GLint log_length = 0;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
                char *log = arena_make(platform->frame_arena, (size_t)MAX(log_length, 1), char);
                glGetProgramInfoLog(program, MAX(log_length, 1), 0, log);
                log_error("shader failed to link:\n%s", log);
            }
        }
        for (u32 stage = 0; stage < STAGE_COUNT; stage += 1) glDeleteShader(stage_shaders[stage]);

        if (link_ok) {
            GLES3_Shader *gles3_shader = &gpu->shaders[lookup.slot];
            *gles3_shader = (GLES3_Shader){ .program = program, .uses_default_vertex_shader = uses_default_vertex_shader };

            GLuint draw_block_index = glGetUniformBlockIndex(program, "gpu_draw_uniforms");
            if (draw_block_index != GL_INVALID_INDEX) glUniformBlockBinding(program, draw_block_index, GLES3_Uniform_Binding_DRAW);

            gles3_shader->arguments_block_index = glGetUniformBlockIndex(program, "gpu_arguments");
            if (gles3_shader->arguments_block_index != GL_INVALID_INDEX) {
                glUniformBlockBinding(program, gles3_shader->arguments_block_index, GLES3_Uniform_Binding_ARGUMENTS);
                glGetActiveUniformBlockiv(program, gles3_shader->arguments_block_index, GL_UNIFORM_BLOCK_DATA_SIZE, &gles3_shader->arguments_block_size);
            }

            static const char *sampler_names[GPU_MAX_TEXTURES_PER_DRAW] = { "gpu_texture_0", "gpu_texture_1", "gpu_texture_2", "gpu_texture_3" };
            glUseProgram(program);
            for (GLint unit = 0; unit < GPU_MAX_TEXTURES_PER_DRAW; unit += 1) glUniform1i(glGetUniformLocation(program, sampler_names[unit]), unit);
        } else {
            glDeleteProgram(program);
            gpu__cache_remove(&gpu->shader_cache, lookup.slot);
            shader_ok = false;
        }
    }

    GPU_Shader_Id id = 0;
    if (shader_ok) {
        id = gpu__id_from_slot(&gpu->shader_cache, lookup.slot);
    }

    *ok = shader_ok;
    return id;
}

static GPU_Texture_Id gpu_texture_id(Platform *platform, const GPU_Texture *texture) {
    GLES3_GPU *gpu = gles3__gpu_from_platform(platform);
    assert(gpu->is_ready);

    bool has_pixels = texture->pixels.size > 0;
    bool is_keyed_by_content = texture->key == 0;
    bool has_description = texture->width > 0 && texture->height > 0;

    u32 pixels_per_row = texture->pixels_per_row != 0 ? texture->pixels_per_row : texture->width;
    u64 bytes_per_pixel = gpu_texture_bytes_per_pixel[texture->format];
    u64 expected_pixels_size = has_description ? ((u64)(texture->height - 1) * pixels_per_row + texture->width) * bytes_per_pixel : 0;
    bool pixels_ok = !has_pixels || (has_description && texture->pixels.size >= expected_pixels_size);

    u64 key = texture->key;
    if (is_keyed_by_content && has_pixels) {
        key = ds_hash_fnv1a_64(texture->pixels.data, texture->pixels.size);
        key = ds_hash_fnv1a_64_update(key, &texture->width, sizeof texture->width);
        key = ds_hash_fnv1a_64_update(key, &texture->height, sizeof texture->height);
        key = ds_hash_fnv1a_64_update(key, &texture->format, sizeof texture->format);
        key = ds_hash_fnv1a_64_update(key, &texture->flags, sizeof texture->flags);
        key = ds_hash_fnv1a_64_update(key, &texture->filter, sizeof texture->filter);
        key = ds_hash_fnv1a_64_update(key, &texture->wrap, sizeof texture->wrap);
        key = ds_hash_fnv1a_64_update(key, &pixels_per_row, sizeof pixels_per_row);
    }

    bool lookup_ok = pixels_ok && (!is_keyed_by_content || has_pixels);
    bool may_insert = has_description;
    GPU_Cache_Lookup lookup = {0};
    if (lookup_ok) lookup = gles3__cache_find_or_insert(gpu, &gpu->texture_cache, key, is_keyed_by_content, may_insert, &lookup_ok);

    GPU_Texture_Id id = 0;
    if (lookup_ok) {
        GLES3_Texture *gles3_texture = &gpu->textures[lookup.slot];
        if (!lookup.was_found) *gles3_texture = (GLES3_Texture){0};

        bool description_changed = has_description && (
            gles3_texture->texture == 0 ||
            gles3_texture->width != texture->width ||
            gles3_texture->height != texture->height ||
            gles3_texture->format != texture->format ||
            gles3_texture->flags != texture->flags
        );
        bool should_upload = has_pixels && (!is_keyed_by_content || !lookup.was_found);

        if (lookup.was_found && !description_changed && !should_upload) gpu->stats.cache_hits += 1;
        else gpu->stats.cache_misses += 1;

        static const GLint gl_internal_format[GPU_Texture_Format_COUNT] = {
            [GPU_Texture_Format_RGBA8] = GL_RGBA8,
            [GPU_Texture_Format_R8] = GL_R8,
            [GPU_Texture_Format_RGBA16F] = GL_RGBA16F,
        };
        static const GLenum gl_format[GPU_Texture_Format_COUNT] = {
            [GPU_Texture_Format_RGBA8] = GL_RGBA,
            [GPU_Texture_Format_R8] = GL_RED,
            [GPU_Texture_Format_RGBA16F] = GL_RGBA,
        };
        static const GLenum gl_type[GPU_Texture_Format_COUNT] = {
            [GPU_Texture_Format_RGBA8] = GL_UNSIGNED_BYTE,
            [GPU_Texture_Format_R8] = GL_UNSIGNED_BYTE,
            [GPU_Texture_Format_RGBA16F] = GL_HALF_FLOAT,
        };

        if (description_changed || should_upload) {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glPixelStorei(GL_UNPACK_ROW_LENGTH, (GLint)pixels_per_row);
        }

        if (description_changed) {
            gles3__texture_delete(gpu, gles3_texture);

            u64 color_byte_size = (u64)texture->width * texture->height * bytes_per_pixel;
            bool will_have_mipmaps = !!(texture->flags & GPU_Texture_Flag_MIPMAPS);
            bool will_have_depth_buffer = !!(texture->flags & GPU_Texture_Flag_DEPTH_BUFFER);
            u64 byte_size = color_byte_size + (will_have_mipmaps ? color_byte_size / 3 : 0) + (will_have_depth_buffer ? (u64)texture->width * texture->height * 4 : 0);
            gles3__make_bytes_available(gpu, byte_size);
            gles3_texture->byte_size = byte_size;
            gpu->cached_bytes += byte_size;

            gles3_texture->width = texture->width;
            gles3_texture->height = texture->height;
            gles3_texture->format = texture->format;
            gles3_texture->flags = texture->flags;

            glGenTextures(1, &gles3_texture->texture);
            glBindTexture(GL_TEXTURE_2D, gles3_texture->texture);
            const void *initial_pixels = has_pixels ? texture->pixels.data : 0;
            glTexImage2D(GL_TEXTURE_2D, 0, gl_internal_format[texture->format], (GLsizei)texture->width, (GLsizei)texture->height, 0, gl_format[texture->format], gl_type[texture->format], initial_pixels);

            bool is_render_target = !!(texture->flags & GPU_Texture_Flag_RENDER_TARGET);
            gpu->texture_cache.entries[lookup.slot].must_not_be_evicted = is_render_target;
            if (is_render_target) {
                glGenFramebuffers(1, &gles3_texture->framebuffer);
                glBindFramebuffer(GL_FRAMEBUFFER, gles3_texture->framebuffer);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gles3_texture->texture, 0);

                bool has_depth_buffer = !!(texture->flags & GPU_Texture_Flag_DEPTH_BUFFER);
                if (has_depth_buffer) {
                    glGenRenderbuffers(1, &gles3_texture->depth_buffer);
                    glBindRenderbuffer(GL_RENDERBUFFER, gles3_texture->depth_buffer);
                    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (GLsizei)texture->width, (GLsizei)texture->height);
                    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gles3_texture->depth_buffer);
                }

                ensure(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }
        } else if (should_upload) {
            glBindTexture(GL_TEXTURE_2D, gles3_texture->texture);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, (GLsizei)texture->width, (GLsizei)texture->height, gl_format[texture->format], gl_type[texture->format], texture->pixels.data);
        }

        if (description_changed || should_upload) {
            static const GLint gl_filter[GPU_Texture_Filter_COUNT] = {
                [GPU_Texture_Filter_LINEAR] = GL_LINEAR,
                [GPU_Texture_Filter_NEAREST] = GL_NEAREST,
            };
            static const GLint gl_wrap[GPU_Texture_Wrap_COUNT] = {
                [GPU_Texture_Wrap_CLAMP_TO_EDGE] = GL_CLAMP_TO_EDGE,
                [GPU_Texture_Wrap_REPEAT] = GL_REPEAT,
                [GPU_Texture_Wrap_MIRRORED_REPEAT] = GL_MIRRORED_REPEAT,
            };

            bool has_mipmaps = !!(texture->flags & GPU_Texture_Flag_MIPMAPS);
            GLint minification_filter = has_mipmaps ? GL_LINEAR_MIPMAP_LINEAR : gl_filter[texture->filter];
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minification_filter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter[texture->filter]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, gl_wrap[texture->wrap]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, gl_wrap[texture->wrap]);
            if (has_mipmaps) glGenerateMipmap(GL_TEXTURE_2D);

            glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
            if (has_pixels) gpu->stats.bytes_uploaded += (u64)texture->width * texture->height * bytes_per_pixel;
        }

        id = gpu__id_from_slot(&gpu->texture_cache, lookup.slot);
    }

    return id;
}

static void gles3__begin_frame(Platform *platform) {
    GLES3_GPU *gpu = gles3__gpu_from_platform(platform);

    gpu->frame_index += 1;
    gpu->commands = 0;
    gpu->command_count = 0;
    gpu->command_capacity = 0;
    for (u32 kind = 0; kind < GLES3_Stream_Kind_COUNT; kind += 1) {
        gpu->streams[kind].data = 0;
        gpu->streams[kind].size = 0;
        gpu->streams[kind].capacity = 0;
    }
    gpu->stats = (GPU_Frame_Stats){0};
}

static void gles3__set_vertex_attribute(GLuint location, GPU_Vertex_Format format, u32 stride, u64 offset, GLuint divisor) {
    static const GLint component_count[GPU_Vertex_Format_COUNT] = {
        [GPU_Vertex_Format_F32X1] = 1,
        [GPU_Vertex_Format_F32X2] = 2,
        [GPU_Vertex_Format_F32X3] = 3,
        [GPU_Vertex_Format_F32X4] = 4,
        [GPU_Vertex_Format_U8X4_NORMALISED] = 4,
        [GPU_Vertex_Format_U32X1] = 1,
    };
    const void *pointer = (const void *)size_t_from_u64(offset);
    switch (format) {
        case GPU_Vertex_Format_F32X1: case GPU_Vertex_Format_F32X2: case GPU_Vertex_Format_F32X3: case GPU_Vertex_Format_F32X4: {
            glVertexAttribPointer(location, component_count[format], GL_FLOAT, GL_FALSE, (GLsizei)stride, pointer);
        } break;
        case GPU_Vertex_Format_U8X4_NORMALISED: {
            glVertexAttribPointer(location, component_count[format], GL_UNSIGNED_BYTE, GL_TRUE, (GLsizei)stride, pointer);
        } break;
        case GPU_Vertex_Format_U32X1: {
            glVertexAttribIPointer(location, component_count[format], GL_UNSIGNED_INT, (GLsizei)stride, pointer);
        } break;
        default: unreachable;
    }
    glVertexAttribDivisor(location, divisor);
}

static void gles3__end_frame(Platform *platform) {
    GLES3_GPU *gpu = gles3__gpu_from_platform(platform);
    App_Frame_Info *frame = &platform->frame;

    bool should_render = frame->window_size.x > 0 && frame->window_size.y > 0;
    if (should_render) {
        glBindVertexArray(gpu->vertex_array);
        for (u32 kind = 0; kind < GLES3_Stream_Kind_COUNT; kind += 1) {
            GLES3_Stream *stream = &gpu->streams[kind];
            if (stream->size > 0) {
                glBindBuffer(stream->target, stream->buffer);
                glBufferData(stream->target, (GLsizeiptr)stream->size, stream->data, GL_STREAM_DRAW);
                gpu->stats.bytes_uploaded += stream->size;
            }
        }

        static const GLenum gl_topology[GPU_Topology_COUNT] = {
            [GPU_Topology_TRIANGLES] = GL_TRIANGLES,
            [GPU_Topology_TRIANGLE_STRIP] = GL_TRIANGLE_STRIP,
            [GPU_Topology_LINES] = GL_LINES,
            [GPU_Topology_POINTS] = GL_POINTS,
        };
        static const GLenum gl_depth_function[GPU_Depth_Test_COUNT] = {
            [GPU_Depth_Test_LESS] = GL_LESS,
            [GPU_Depth_Test_LESS_OR_EQUAL] = GL_LEQUAL,
        };
        static const GLenum gl_index_type[GPU_Index_Format_COUNT] = {
            [GPU_Index_Format_U16] = GL_UNSIGNED_SHORT,
            [GPU_Index_Format_U32] = GL_UNSIGNED_INT,
        };

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, (GLsizei)roundf(frame->window_size.x), (GLsizei)roundf(frame->window_size.y));
        glDisable(GL_SCISSOR_TEST);
        glDepthMask(GL_TRUE);
        V4 window_clear_color = rgba_float_from_hex(platform->clear_color);
        glClearColor(window_clear_color.x, window_clear_color.y, window_clear_color.z, window_clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        bool framebuffer_is_bound = true;
        GLuint bound_framebuffer = 0;
        GLuint bound_program = 0;
        for (u64 command_index = 0; command_index < gpu->command_count; command_index += 1) {
            const GLES3_Command *command = &gpu->commands[command_index];

            GLuint framebuffer = 0;
            V2 target_size = frame->window_size;
            if (command->has_target) {
                const GLES3_Texture *target = &gpu->textures[command->target_slot];
                framebuffer = target->framebuffer;
                target_size = (V2){ .x = (f32)target->width, .y = (f32)target->height };
            }
            if (!framebuffer_is_bound || framebuffer != bound_framebuffer) {
                glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
                glViewport(0, 0, (GLsizei)roundf(target_size.x), (GLsizei)roundf(target_size.y));
                framebuffer_is_bound = true;
                bound_framebuffer = framebuffer;
            }

            switch (command->kind) {
                case GLES3_Command_Kind_CLEAR: {
                    glDisable(GL_SCISSOR_TEST);
                    glDepthMask(GL_TRUE);
                    V4 color = rgba_float_from_hex(command->clear_color);
                    glClearColor(color.x, color.y, color.z, color.w);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
                } break;
                case GLES3_Command_Kind_DRAW: {
                    const GPU_Draw *draw = &command->draw;

                    GLuint program = gpu->shaders[command->shader_slot].program;
                    if (program != bound_program) {
                        glUseProgram(program);
                        bound_program = program;
                        gpu->stats.program_changes += 1;
                    }

                    switch (draw->blend_mode) {
                        case GPU_Blend_Mode_ALPHA: glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); break;
                        case GPU_Blend_Mode_PREMULTIPLIED_ALPHA: glEnable(GL_BLEND); glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); break;
                        case GPU_Blend_Mode_ADDITIVE: glEnable(GL_BLEND); glBlendFunc(GL_ONE, GL_ONE); break;
                        case GPU_Blend_Mode_NONE: glDisable(GL_BLEND); break;
                        default: unreachable;
                    }

                    bool has_depth_test = draw->depth_test != GPU_Depth_Test_NONE;
                    if (has_depth_test) {
                        glEnable(GL_DEPTH_TEST);
                        glDepthFunc(gl_depth_function[draw->depth_test]);
                    } else {
                        glDisable(GL_DEPTH_TEST);
                    }
                    glDepthMask(draw->depth_write ? GL_TRUE : GL_FALSE);

                    switch (draw->cull_mode) {
                        case GPU_Cull_Mode_NONE: glDisable(GL_CULL_FACE); break;
                        case GPU_Cull_Mode_BACK: glEnable(GL_CULL_FACE); glCullFace(GL_BACK); break;
                        case GPU_Cull_Mode_FRONT: glEnable(GL_CULL_FACE); glCullFace(GL_FRONT); break;
                        default: unreachable;
                    }

                    bool has_clip = !!memcmp(&draw->clip, &(V4){0}, sizeof draw->clip);
                    if (has_clip) {
                        GLint clip_width = (GLint)roundf(v4_right(draw->clip) - v4_left(draw->clip));
                        GLint clip_height = (GLint)roundf(v4_bottom(draw->clip) - v4_top(draw->clip));
                        f32 clip_y = command->has_target ? v4_top(draw->clip) : target_size.y - v4_bottom(draw->clip);
                        glEnable(GL_SCISSOR_TEST);
                        glScissor((GLint)roundf(v4_left(draw->clip)), (GLint)roundf(clip_y), MAX(clip_width, 0), MAX(clip_height, 0));
                    } else {
                        glDisable(GL_SCISSOR_TEST);
                    }

                    for (u32 unit = 0; unit < GPU_MAX_TEXTURES_PER_DRAW; unit += 1) {
                        if (draw->textures[unit] != 0) {
                            glActiveTexture(GL_TEXTURE0 + unit);
                            glBindTexture(GL_TEXTURE_2D, gpu->textures[command->texture_slots[unit]].texture);
                        }
                    }
                    glActiveTexture(GL_TEXTURE0);

                    glBindBufferRange(GL_UNIFORM_BUFFER, GLES3_Uniform_Binding_DRAW, gpu->streams[GLES3_Stream_Kind_UNIFORM].buffer, (GLintptr)command->uniforms_offset, sizeof(GLES3_Draw_Uniforms));
                    if (draw->arguments.size > 0) {
                        glBindBufferRange(GL_UNIFORM_BUFFER, GLES3_Uniform_Binding_ARGUMENTS, gpu->streams[GLES3_Stream_Kind_UNIFORM].buffer, (GLintptr)command->arguments_offset, (GLsizeiptr)draw->arguments.size);
                    }

                    u32 wanted_vertex_attributes = 0;
                    const GLES3_Mesh *mesh = &gpu->meshes[command->mesh_slot];
                    if (command->has_mesh) {
                        GLuint vertex_buffer = mesh->is_streamed ? gpu->streams[GLES3_Stream_Kind_VERTEX].buffer : mesh->vertex_buffer;
                        u64 attribute_offset = mesh->is_streamed ? mesh->stream_vertices_offset : 0;
                        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
                        for (u32 i = 0; i < GPU_MAX_VERTEX_ATTRIBUTES; i += 1) {
                            GPU_Vertex_Format format = mesh->vertex_attributes[i];
                            if (format != GPU_Vertex_Format_NONE) {
                                gles3__set_vertex_attribute(i, format, mesh->vertex_stride, attribute_offset, 0);
                                wanted_vertex_attributes |= 1u << i;
                            }
                            attribute_offset += gpu_vertex_format_size[format];
                        }
                    }

                    bool has_instances = draw->instances.size > 0;
                    if (has_instances) {
                        u64 attribute_offset = command->instances_offset;
                        glBindBuffer(GL_ARRAY_BUFFER, gpu->streams[GLES3_Stream_Kind_VERTEX].buffer);
                        for (u32 i = 0; i < GPU_MAX_INSTANCE_ATTRIBUTES; i += 1) {
                            GPU_Vertex_Format format = draw->instance_attributes[i];
                            #define GPU_FIRST_INSTANCE_ATTRIBUTE_LOCATION GPU_MAX_VERTEX_ATTRIBUTES
                            GLuint location = GPU_FIRST_INSTANCE_ATTRIBUTE_LOCATION + i;
                            if (format != GPU_Vertex_Format_NONE) {
                                gles3__set_vertex_attribute(location, format, command->instance_stride, attribute_offset, 1);
                                wanted_vertex_attributes |= 1u << location;
                            }
                            attribute_offset += gpu_vertex_format_size[format];
                        }
                    }

                    u32 changed_vertex_attributes = wanted_vertex_attributes ^ gpu->enabled_vertex_attributes;
                    for (GLuint location = 0; location < GPU_MAX_VERTEX_ATTRIBUTES + GPU_MAX_INSTANCE_ATTRIBUTES; location += 1) {
                        bool has_changed = !!(changed_vertex_attributes & (1u << location));
                        bool is_wanted = !!(wanted_vertex_attributes & (1u << location));
                        if (has_changed && is_wanted) glEnableVertexAttribArray(location);
                        if (has_changed && !is_wanted) glDisableVertexAttribArray(location);
                    }
                    gpu->enabled_vertex_attributes = wanted_vertex_attributes;

                    u32 instance_count = draw->instance_count;
                    if (instance_count == 0) instance_count = has_instances ? (u32)(draw->instances.size / command->instance_stride) : 1;

                    bool is_indexed = command->has_mesh && mesh->index_count > 0;
                    if (is_indexed) {
                        GLuint index_buffer = mesh->is_streamed ? gpu->streams[GLES3_Stream_Kind_INDEX].buffer : mesh->index_buffer;
                        u64 index_offset = mesh->is_streamed ? mesh->stream_indices_offset : 0;
                        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
                        glDrawElementsInstanced(gl_topology[draw->topology], (GLsizei)mesh->index_count, gl_index_type[mesh->index_format], (const void *)size_t_from_u64(index_offset), (GLsizei)instance_count);
                    } else {
                        GLenum mode = command->has_mesh ? gl_topology[draw->topology] : GL_TRIANGLE_STRIP;
                        u32 vertex_count = command->has_mesh ? mesh->vertex_count : 4;
                        glDrawArraysInstanced(mode, 0, (GLsizei)vertex_count, (GLsizei)instance_count);
                    }
                    gpu->stats.draw_calls += 1;
                } break;
                default: unreachable;
            }
        }

    }

    gpu->stats.commands = (u32)gpu->command_count;
    gpu->stats.cached_bytes = gpu->cached_bytes;
    platform->gpu_stats_of_last_frame = gpu->stats;
}

#define GLES3_DEFAULT_MEMORY_LIMIT_IN_BYTES (1ull << 30)

static void gles3__initialise(GLES3_GPU *gpu, Arena *persistent_arena) {
    glGenVertexArrays(1, &gpu->vertex_array);
    glBindVertexArray(gpu->vertex_array);

    static const GLenum stream_targets[GLES3_Stream_Kind_COUNT] = {
        [GLES3_Stream_Kind_VERTEX] = GL_ARRAY_BUFFER,
        [GLES3_Stream_Kind_INDEX] = GL_ELEMENT_ARRAY_BUFFER,
        [GLES3_Stream_Kind_UNIFORM] = GL_UNIFORM_BUFFER,
    };
    for (u32 kind = 0; kind < GLES3_Stream_Kind_COUNT; kind += 1) {
        glGenBuffers(1, &gpu->streams[kind].buffer);
        gpu->streams[kind].target = stream_targets[kind];
    }

    GLint uniform_offset_alignment = 0;
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uniform_offset_alignment);
    ensure(uniform_offset_alignment > 0);
    gpu->uniform_offset_alignment = (u64)uniform_offset_alignment;

    if (gpu->memory_limit_in_bytes == 0) gpu->memory_limit_in_bytes = GLES3_DEFAULT_MEMORY_LIMIT_IN_BYTES;

    gpu->shader_cache = gpu__cache_make(persistent_arena, GPU_MAX_SHADERS_EXPONENT);
    gpu->shaders = arena_make(persistent_arena, (1u << GPU_MAX_SHADERS_EXPONENT) + 1, GLES3_Shader);

    gpu->mesh_cache = gpu__cache_make(persistent_arena, GPU_MAX_MESHES_EXPONENT);
    gpu->meshes = arena_make(persistent_arena, (1u << GPU_MAX_MESHES_EXPONENT) + 1, GLES3_Mesh);

    gpu->texture_cache = gpu__cache_make(persistent_arena, GPU_MAX_TEXTURES_EXPONENT);
    gpu->textures = arena_make(persistent_arena, (1u << GPU_MAX_TEXTURES_EXPONENT) + 1, GLES3_Texture);

    gpu->is_ready = true;
}
