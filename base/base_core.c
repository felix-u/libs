#if defined(BASE_NO_IMPLEMENTATION) || defined(BASE_NO_IMPLEMENTATION_CORE)

#include <stdarg.h> // TODO(felix): look into removing
#include <stdint.h> // TODO(felix): look into removing
#include <stddef.h> // TODO(felix): look into removing

#if OS_EMSCRIPTEN
    // TODO(felix): idk
    #include<stdlib.h>
    #include<stdio.h>
    #include <fcntl.h>
    #include <unistd.h>

#elif OS_LINUX
    #define static_assert _Static_assert
    void exit(int); // TODO(felix): can remove?
    void abort(void); // TODO(felix): own implementation to not depend on libc
    void *calloc(size_t item_count, size_t item_size); // TODO(felix): remove once using virtual alloc arena
    void free(void *pointer); // TODO(felix): remove once using virtual alloc arena
    // TODO(felix): which of these can be removed in favour of doing direct syscalls?
        #include <fcntl.h>
        #include <unistd.h>
    #include <stdio.h> // TODO(felix): only needed for FILE. remove!

#elif OS_MACOS
    #define static_assert _Static_assert
    // TODO(felix): same notes as above
    void exit(int);
    void abort(void);
    void *calloc(size_t item_count, size_t item_size);
    void free(void *pointer);
    #include <fcntl.h>
    #include <unistd.h>
    #include <stdio.h>

#elif OS_WINDOWS
    #define WIN32_LEAN_AND_MEAN
    #define VC_EXTRALEAN
    #define os_abort() ExitProcess(1)
    #define os_exit(code) ExitProcess(code)
    #if !BASE_GRAPHICS
        #define NOGDI
        #define NOUSER
        #define NOMINMAX
        // #undef near
        // #undef far // TODO(felix): figure out what to do with this - needed by combaseapi.h
    #endif
    #include "windows.h"
    #include "shellapi.h"
    #define _CRT_SECURE_NO_WARNINGS
    #undef min
    #undef max
    // TODO(felix): these win32_asserts should probably be in base_gfx (at the very least, win32_assert_d3d_compile should be)
    #define win32_ensure_not_0(win32_fn_result) statement_macro( if ((win32_fn_result) == 0) { panic("win32: %", fmt(u64, GetLastError())); } )

#endif // OS


#if COMPILER_CLANG || COMPILER_GCC // they share many builtins
    #define builtin_unreachable __builtin_unreachable()
    #define force_inline inline __attribute__((always_inline))
    // TODO(felix): this is only for memcmp, etc. fix!
    #include <string.h>
#endif


#if COMPILER_CLANG
    #define builtin_assume(expr) __builtin_assume(expr)
    #define breakpoint __builtin_debugtrap()

#elif COMPILER_GCC
    #define builtin_assume(expr) { if (!(expr)) unreachable; }
    #define breakpoint __builtin_trap()

#elif COMPILER_MSVC
    #define builtin_assume(expr) __assume(expr)
    #define breakpoint __debugbreak()
    #define builtin_unreachable assert(false)
    #define force_inline inline __forceinline

#elif COMPILER_STANDARD
    // NOTE(felix): probably broken in many ways. Getting this working with a non-major compiler is not a priority
    #include <assert.h>
    #define builtin_assume(expr) assert(expr)
    #define breakpoint builtin_assume(false)
    #define builtin_unreachable assert(false)
    #define force_inline inline

#endif // COMPILER


#if BUILD_DEBUG
    #define assert(expr) { if(!(expr)) panic("failed assertion `"#expr"`"); }
    #define ensure(expression) assert(expression)
    #define unreachable panic("reached unreachable code")
#else
    #define assert(expr) builtin_assume(expr)
    #define ensure(expression) { if (!(expression)) panic("fatal condition"); }
    #define unreachable builtin_unreachable
#endif // BUILD_DEBUG


// TODO(felix): no dependency on C stdlib!
    #include <math.h> // TODO(felix): look into

#define bool _Bool
#define true 1
#define false 0
#define __bool_true_false_are_defined 1

typedef  uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef   int8_t  i8;
typedef  int16_t i16;
typedef  int32_t i32;
typedef  int64_t i64;
typedef    float f32;
typedef   double f64;

typedef unsigned char uchar;
typedef        size_t usize;
#if OS_WINDOWS
    typedef SSIZE_T isize; // TODO(felix): is this from windows.h? should I use ptrdiff_t or intptr_t instead?
#elif OS_LINUX
    typedef ssize_t isize;
#elif OS_MACOS
    typedef i64 isize;
#elif OS_EMSCRIPTEN
    typedef i64 isize;
#else
    #error "haven't figured out an isize typedef for this OS yet"
#endif
typedef     uintptr_t upointer;
typedef      intptr_t ipointer;

#if BUILD_DEBUG
    #pragma section(".raddbg", read, write)
    #define raddbg_exe_data __declspec(allocate(".raddbg"))
    raddbg_exe_data unsigned char raddbg_is_attached_byte_marker[1];
    #define raddbg_glue_(a, b) a##b
    #define raddbg_glue(a, b) raddbg_glue_(a, b)
    #define raddbg_gen_data_id() raddbg_glue(raddbg_data__, __COUNTER__)

    #define raddbg_type_view(type, ...) raddbg_exe_data char raddbg_gen_data_id()[] = ("type_view: {type: ```" #type "```, expr: ```" #__VA_ARGS__ "```}");
    #define raddbg_entry_point(...) raddbg_exe_data char raddbg_gen_data_id()[] = ("entry_point: \"" #__VA_ARGS__ "\"");
#else
    #define raddbg_type_view(...)
    #define raddbg_entry_point(...)
#endif

raddbg_type_view(Slice_?, $.slice())
raddbg_type_view(Array_?, $.slice())

#define Slice(type) struct { type *data; usize count; }

struct Arena;
#define Array(type) struct { \
    union { \
        Slice_##type slice; \
        struct { type *data; usize count; }; \
    }; \
    usize capacity; \
    struct Arena *arena; \
}

typedef Slice(u8) Slice_u8;
typedef Slice_u8 String;
typedef Slice(String) Slice_String;
typedef Array(String) Array_String;

struct Arena;
#define Map(type) struct { \
    struct Arena *arena; \
    Array_String keys; \
    Array_##type values; \
}

#define for_slice(ptr_type, name, slice)\
    for (ptr_type name = (slice).data; (name == 0 ? false : name < (slice).data + (slice).count); name += 1)

#define array_count(arr) (sizeof(arr) / sizeof((arr)[0]))
#define array_size(arr) ((arr).capacity * sizeof(*((arr).data)))

#define discard(expression) (void)(expression)

#define define_container_types(type) \
    typedef Slice(type) Slice_##type; \
    typedef Array(type) Array_##type; \
    typedef Map(type) Map_##type;

define_container_types(void);

#define enumdef(Name, type)\
    typedef type Name;\
    define_container_types(Name);\
    enum

#define structdef(Name) \
    typedef struct Name Name; \
    define_container_types(Name);\
    struct Name

#define uniondef(Name) \
    typedef union Name Name; \
    define_container_types(Name);\
    union Name

#define stringc(s)  { .data = (u8 *)s, .count = sizeof(s) - 1 }
#define string(s) (String)stringc(s)
raddbg_type_view(String, view:text($.data, size=count))

structdef(String16) { u16 *data; usize count; };
#define string16c(s) { .data = (u16 *)s, .count = sizeof(s) / sizeof(u16) - 1 }
#define string16(s) (String16)string16c(s)

typedef Array(u8) Array_u8;
typedef Array_u8 String_Builder;
define_container_types(String_Builder);


#define log_error(...) log_internal("error: " __VA_ARGS__)

#define panic(...) {\
    log_internal_with_location(__FILE__, __LINE__, (char *)__func__, "panic: " __VA_ARGS__);\
    breakpoint; os_abort();\
}

#define statement_macro(...) do { __VA_ARGS__ } while (0)

// TODO(felix): a number of these would benefit from assert-as-expression using comma operator
#define slice_from_c_array(c_array) { .data = c_array, .count = array_count(c_array) }
#define slice_of(type, ...) slice_from_c_array(((type[]){ __VA_ARGS__ }))
#define slice_get_last(s) (assert_expression((s).count > 0) ? &(s).data[(s).count - 1] : 0)
#define pop(s) (*(assert_expression((s).count > 0) ? &(s).data[--(s).count] : 0))
#define slice_range(slice, beg, end) { .data = (slice).data + (beg), .count = (end) - (beg) }
#define slice_swap_remove(slice_pointer, idx) (slice_pointer)->data[idx] = (slice_pointer)->data[--(slice_pointer)->count]
#define slice_as_bytes(slice) (String){ .data = (u8 *)(slice).data, .count = sizeof(*((slice).data)) * (slice).count }

#define array_from_c_array_c(type, capacity_) { .data = (type[capacity_]){0}, .capacity = capacity_ }
#define array_from_c_array(type, capacity_) ((Array_##type)array_from_c_array_c(type, capacity_))

#define array_ensure_capacity_non_zero(array_pointer, item_count) \
    array_ensure_capacity_explicit_item_size((Array_void *)(array_pointer), item_count, sizeof(*((array_pointer)->data)), true)
#define array_ensure_capacity(array_pointer, item_count) \
    array_ensure_capacity_explicit_item_size((Array_void *)(array_pointer), item_count, sizeof(*((array_pointer)->data)), false)
static void array_ensure_capacity_explicit_item_size(Array_void *array, usize item_count, usize item_size, bool non_zero);

#define array_from_slice(s) { .data = (s).data, .count = (s).count, .capacity = (s).count }

#define push(array_pointer, item) statement_macro( \
    array_ensure_capacity_explicit_item_size((Array_void *)array_pointer, (array_pointer)->count + 1, sizeof(*((array_pointer)->data)), false); \
    push_assume_capacity(array_pointer, item); \
)

#define push_assume_capacity(array_pointer, item) statement_macro( \
    assert((array_pointer)->count < (array_pointer)->capacity); \
    (array_pointer)->data[(array_pointer)->count++] = item; \
)

#define push_slice(array_pointer, slice) statement_macro( \
    array_ensure_capacity_explicit_item_size((Array_void *)array_pointer, (array_pointer)->count + (slice).count, sizeof(*((array_pointer)->data)), false); \
    push_slice_assume_capacity(array_pointer, slice); \
)

#define push_slice_assume_capacity(array_pointer, slice) statement_macro( \
    assert((array_pointer)->count + (slice).count <= (array_pointer)->capacity); \
    for (usize __psac_i = 0; __psac_i < (slice).count; __psac_i += 1) { \
        (array_pointer)->data[(array_pointer)->count + __psac_i] = (slice).data[__psac_i]; \
    } \
    (array_pointer)->count += (slice).count; \
)

#define array_unused_capacity(array) ((array).capacity - (array).count)

#define bit_cast(type) *(type *)&

// TODO(felix): figure out how to solve min() and max() conflicting with windows headers, even when BASE_GRAPHICS is 1
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
#define clamp_low max
#define clamp_high min
#define clamp(value, low, high) clamp_high(clamp_low(value, low), high)

#define swap(type, a, b) statement_macro( \
    type temp_a__ = *(a); *(a) = *(b); *(b) = temp_a__; \
)

// TODO(felix): rename/replace
static inline int memcmp_(void *a_, void *b_, usize byte_count);
#if OS_WINDOWS
    #pragma function(memcpy)
#endif
void *memcpy(void *destination_, const void *source_, usize byte_count);
#if OS_WINDOWS
    #pragma function(memset)
#endif
extern void *memset(void *destination_, int byte_, usize byte_count);

struct Arena;
static Slice_String os_get_arguments(struct Arena *arena);


#else // IMPLEMENTATION


static force_inline bool assert_expression(bool value) {
    assert(value);
    return true;
}

// TODO(felix): replace with metaprogram lib specifier
#if OS_WINDOWS
    #pragma comment(lib, "kernel32.lib")
    #pragma comment(lib, "shell32.lib") // needed by os_get_arguments
#endif

// TODO(felix): add slice_equal/slice_compare

static void array_ensure_capacity_explicit_item_size(Array_void *array, usize item_count, usize item_size, bool non_zero) {
    if (array->capacity >= item_count) return;

    // TODO(felix): should this be a power of 2?
    usize new_capacity = clamp_low(1, array->capacity * 2);
    while (new_capacity < item_count) new_capacity *= 2;

    u8 *new_memory = arena_make(array->arena, new_capacity, item_size);
    if (array->count > 0) memcpy(new_memory, array->data, array->count * item_size);

    if (!non_zero) {
        usize old_capacity = array->capacity;
        usize growth_byte_count = item_size * (new_capacity - old_capacity);
        u8 *beginning_of_new_memory = new_memory + (item_size * old_capacity);
        memset(beginning_of_new_memory, 0, growth_byte_count);
    }

    array->data = new_memory;
    array->capacity = new_capacity;
}

// TODO(felix): rename/replace (see base_core.h)
static inline int memcmp_(void *a_, void *b_, usize byte_count) {
    u8 *a = a_, *b = b_;
    assert(a != 0);
    assert(b != 0);
    for (usize i = 0; i < byte_count; i += 1) {
        if (a[i] != b[i]) return a[i] - b[i];
    }
    return 0;
}

void *memcpy(void *destination_, const void *source_, usize byte_count) {
    u8 *destination = destination_;
    const u8 *source = source_;
    if (byte_count != 0) {
        assert(destination != 0);
        assert(source != 0);
    }
    for (usize i = 0; i < byte_count; i += 1) destination[i] = source[i];
    return destination;
}

extern void *memset(void *destination_, int byte_, usize byte_count) {
    assert(byte_ < 256);
    u8 byte = (u8)byte_;
    u8 *destination = destination_;
    assert(destination != 0);
    for (usize i = 0; i < byte_count; i += 1) destination[i] = byte;
    return destination;
}

raddbg_entry_point(program)
static u8 program(void);

#if OS_WINDOWS
    void entrypoint(void) {
        u8 exit_code = program();
        ExitProcess(exit_code);
    }
#elif OS_LINUX || OS_MACOS
    // TODO(felix): replace with something that feels less hacky

    static int argument_count_;
    static char **arguments_;

    int main(int argument_count, char **arguments) {
        argument_count_ = argument_count;
        arguments_ = arguments;
        u8 exit_code = entrypoint();
        return exit_code;
    }
#endif

static Slice_String os_get_arguments(Arena *arena) {
    Array_String arguments = { .arena = arena };

    #if OS_WINDOWS
        int argument_count = 0;
        u16 **arguments_utf16 = CommandLineToArgvW(GetCommandLineW(), &argument_count);
        if (arguments_utf16 == 0) {
            log_error("unable to get command line arguments");
            return (Slice_String){0};
        }

        array_ensure_capacity(&arguments, (usize)argument_count);

        for (usize i = 0; i < (usize)argument_count; i += 1) {
            u16 *argument_utf16 = arguments_utf16[i];

            usize length = 0;
            while (argument_utf16[length] != 0) length += 1;

            // TODO(felix): convert to UTF-8, not ascii
            Array_u8 argument = { .arena = arena };
            array_ensure_capacity(&argument, length);
            for (usize j = 0; j < length; j += 1) {
                u16 wide_character = argument_utf16[j];
                assert(wide_character < 128);
                push_assume_capacity(&argument, (u8)wide_character);
            }
            push_assume_capacity(&arguments, argument.slice);
        }
    #elif OS_LINUX || OS_MACOS
        array_ensure_capacity(&arguments, argument_count__);
        for (usize i = 0; i < (usize)argument_count__; i += 1) {
            String argument = string_from_cstring(arguments__[i]);
            array_push_assume_capacity(&arguments, &argument);
        }
    #endif

    return arguments.slice;
}

#endif // !defined(BASE_NO_IMPLEMENTATION_CORE) && !defined(BASE_NO_IMPLEMENTATION)
