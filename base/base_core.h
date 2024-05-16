#if OS_WINDOWS
    #define WIN32_LEAN_AND_MEAN
    #define VC_EXTRALEAN
    #include "windows.h"
    #define _CRT_SECURE_NO_WARNINGS
#endif // OS_...

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef   uint8_t    u8;
typedef  uint16_t   u16;
typedef  uint32_t   u32;
typedef  uint64_t   u64;
typedef    int8_t    i8;
typedef   int16_t   i16;
typedef   int32_t   i32;
typedef   int64_t   i64;
typedef     float   f32;
typedef    double   f64;

typedef unsigned char uchar;
typedef        size_t usize;
typedef     uintptr_t  uptr;
typedef      intptr_t  iptr;

struct Arena;

#define Slice(type) struct { type *ptr; usize len; }
typedef Slice(void) Slice_void;

#define Array(type) struct { type *ptr; usize len, cap; }
typedef Array(void) Array_void;

#define for_slice(ptr_type, name, slice)\
    for (ptr_type name = slice.ptr; name < slice.ptr + slice.len; name += 1)

#define array_count(arr) (sizeof(arr) / sizeof((arr)[0]))

#define discard(expression) (void)(expression)

#define enumdef(Name, type) typedef type Name; enum Name

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

#if COMPILER_MSVC
    #define force_inline __forceinline
#else
    #define force_inline inline
#endif // COMPILER_...

#define err(s) _err(__FILE__, __LINE__, __func__, s)
static void _err(char *file, usize line, const char *func, char *s);

#define errf(fmt, ...) _errf(__FILE__, __LINE__, __func__, fmt, __VA_ARGS__)
static void _errf(char *file, usize line, const char *func, char *fmt, ...);

#define slice(c_array) { .ptr = c_array, .len = array_count(c_array) }
#define slice_c_array(c_array) { .ptr = c_array, .len = array_count(c_array) }
#define slice_from_array(a) { .ptr = (a).ptr, .len = (a).len }
#define slice_push(slice, item) (slice).ptr[(slice).len++] = item
#define slice_remove(slice_ptr, idx)\
    (slice_ptr)->ptr[idx] = (slice_ptr)->ptr[--(slice_ptr)->len]

#define array_from_slice(s) { .ptr = (s).ptr, .len = (s).len, .cap = (s).len }

#define array_push_slice(arena_ptr, array_ptr, slice_ptr) \
    _array_push_slice(\
        arena_ptr,\
        (Array_void *)array_ptr,\
        (Slice_void *)slice_ptr,\
        sizeof(*((array_ptr)->ptr))\
    )
static void _array_push_slice(
    struct Arena *arena, Array_void *array, Slice_void *slice, usize size
); 

#define array_push(arena_ptr, array_ptr, item_ptr) \
    _array_push(\
        arena_ptr, (Array_void *)(array_ptr), item_ptr, sizeof(*(item_ptr))\
    )
static inline void
_array_push(struct Arena *arena, Array_void *array, void *item, usize size); 

#define min_(a, b) (((a) < (b)) ? (a) : (b))
#define max_(a, b) (((a) > (b)) ? (a) : (b))
#define clamp(x, min_val, max_val) {\
    x = min_((min_val), (x));\
    x = max_((max_val), (x));\
}
