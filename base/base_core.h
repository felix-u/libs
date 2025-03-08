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
    void abort(void); // TODO(felix): own implementation to not depend on libc
    void *calloc(size_t item_count, size_t item_size); // TODO(felix): remove once using virtual alloc arena
    void free(void *pointer); // TODO(felix): remove once using virtual alloc arena
    // TODO(felix): which of these can be removed in favour of doing direct syscalls?
        #include <fcntl.h>
        #include <unistd.h>
    #include <stdio.h> // TODO(felix): only needed for FILE. remove!

#elif OS_MACOS
    // TODO(felix): same notes as above
    void abort(void);
    void *calloc(size_t item_count, size_t item_size);
    void free(void *pointer);
    #include <fcntl.h>
    #include <unistd.h>
    #include <stdio.h>

#elif OS_WINDOWS
    #define WIN32_LEAN_AND_MEAN
    #define VC_EXTRALEAN
    #if !BASE_GRAPHICS
        #define NOGDI
        #define NOUSER
    #endif
    #include "windows.h"
    #define abort() ExitProcess(1) // TODO(felix): is this ok?
    #define _CRT_SECURE_NO_WARNINGS
    #undef near
    #undef far
    #undef min
    #undef max
    // TODO(felix): these win32_asserts should probably be in base_gfx (at the very least, win32_assert_d3d_compile should be)
    #define win32_assert_not_0(win32_fn_result) { if ((win32_fn_result) == 0) { panic("win32: %", fmt(u64, GetLastError())); } }
    #define win32_assert_hr(hresult) { HRESULT hr_ = (hresult); if (hr_ != S_OK) { panic("win32: %", fmt(u64, hr_, .base = 16, .prefix = true, .uppercase = true)); } }
    #define win32_assert_d3d_compile(hresult, err_blob) { if (hresult != S_OK) { panic("D3D compile:\n%", fmt(cstring, err_blob->lpVtbl->GetBufferPointer(err_blob))); } }

#endif // OS


#if COMPILER_CLANG || COMPILER_GCC // they share many builtins
    #define builtin_unreachable __builtin_unreachable()
    #define force_inline inline __attribute__((always_inline))
    #if COMPILER_GCC && !BUILD_RELEASE // TODO(felix): can I avoid this by using nonstandard names for my mem_ defines?
        #include <string.h>
    #endif
    #define memcmp __builtin_memcmp
    #define memcpy __builtin_memcpy
    #define memmove __builtin_memmove
    #define memset __builtin_memset
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

#define Slice(type) struct { type *data; usize count; }
typedef Slice(u8) Slice_u8;
typedef Slice(void) Slice_void;

struct Arena;
#define Array(type) struct { type *data; usize count, capacity; struct Arena *arena; }
typedef Array(void) Array_void;
typedef Array(u8) Array_u8;

#define for_slice(ptr_type, name, slice)\
    for (ptr_type name = slice.data; name < slice.data + slice.count; name += 1)

#define array_count(arr) (sizeof(arr) / sizeof((arr)[0]))
#define array_size(arr) ((arr).capacity * sizeof(*((arr).data)))

#define discard(expression) (void)(expression)

#define enumdef(Name, type)\
    typedef type Name;\
    typedef Slice(Name) Slice_##Name;\
    typedef Array(Name) Array_##Name;\
    enum

#define structdef(Name) \
    typedef struct Name Name; \
    typedef Slice(Name) Slice_##Name; \
    typedef Array(Name) Array_##Name; \
    struct Name

#define uniondef(Name) \
    typedef union Name Name; \
    typedef Slice(Name) Slice_##Name; \
    typedef Array(Name) Array_##Name; \
    union Name

#define err(...) log_internal("error: " __VA_ARGS__)

#define panic(...) {\
    log_internal_with_location(__FILE__, __LINE__, (char *)__func__, "panic: " __VA_ARGS__);\
    breakpoint; abort();\
}

#define slice_from_c_array(c_array) { .data = c_array, .count = array_count(c_array) }
#define slice_get_last_assume_not_empty(s) ((s).data[(s).count - 1])
#define slice_pop(slice) (slice).data[--(slice).count]
#define slice_range(slice, beg, end) { .data = (void *)((uptr)(slice).data + (beg)), .count = (end) - (beg) }
#define slice_remove(slice_pointer, idx) (slice_pointer)->data[idx] = (slice_pointer)->data[--(slice_pointer)->count]
#define slice_size(slice) ((slice).count * sizeof(*((slice).data)))

#define array_ensure_capacity_non_zero(array_pointer, item_count) \
    array_ensure_capacity_explicit_item_size((Array_void *)(array_pointer), item_count, sizeof(*((array_pointer)->data)), true)
#define array_ensure_capacity(array_pointer, item_count) \
    array_ensure_capacity_explicit_item_size((Array_void *)(array_pointer), item_count, sizeof(*((array_pointer)->data)), false)
static void array_ensure_capacity_explicit_item_size(Array_void *array, usize item_count, usize item_size, bool non_zero);

#define array_from_slice(s) { .data = (s).data, .count = (s).count, .capacity = (s).count }

#define array_push(array_pointer, item_ptr) \
    array_push_explicit_item_size((Array_void *)(array_pointer), item_ptr, sizeof(*(item_ptr)))
static inline void array_push_explicit_item_size(Array_void *array, void *item, usize item_size);

#define array_push_assume_capacity(array_pointer, item_ptr) \
    array_push_explicit_item_size_assume_capacity((Array_void *)(array_pointer), item_ptr, sizeof(*(item_ptr)))
static inline void array_push_explicit_item_size_assume_capacity(Array_void *array, void *item, usize item_size);

#define array_push_slice(array_pointer, slice_pointer) \
    array_push_slice_explicit_item_size(\
        (Array_void *)(array_pointer),\
        (Slice_void *)(slice_pointer),\
        sizeof(*((array_pointer)->data))\
    )
static void array_push_slice_explicit_item_size(Array_void *array, Slice_void *slice, usize item_size);

#define array_push_slice_assume_capacity(array_pointer, slice_pointer) \
    array_push_slice_explicit_item_size_assume_capacity(\
        (Array_void *)(array_pointer), (Slice_void *)(slice_pointer), sizeof(*((array_pointer)->data))\
    )
static void array_push_slice_explicit_item_size_assume_capacity(Array_void *array, Slice_void *slice, usize item_size);

#define array_unused_capacity(array) ((array).capacity - (array).count)

#define bit_cast(type) *(type *)&

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
#define clamp_low max
#define clamp_high min
#define clamp(value, low, high) clamp_high(clamp_low(value, low), high)
