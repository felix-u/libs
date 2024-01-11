#ifndef BASE
#define BASE

#define _CRT_SECURE_NO_WARNINGS

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
typedef    size_t usize;
typedef uintptr_t  uptr;
typedef  intptr_t  iptr;
typedef     float   f32;
typedef    double   f64;

typedef  i8  b8;
typedef i16 b16;
typedef i32 b32;
typedef i64 b64;

typedef int error;

static error fatal(char *file, usize line, const char *func) {
    #ifdef COMPILE_DEBUG
        fprintf(stderr, "%s:%d:%s() fatal error\n", file, line, func);
    #else
        (void)file;
        (void)line;
        (void)func;
    #endif // COMPILE_DEBUG
    return 1;
}

#define try(expression) \
    if ((expression) != 0) return fatal(__FILE__, __LINE__, __func__)

#define err(s) _err(__func__, s)
static error _err(const char *func, char *s) {
    fprintf(stderr, "error: %s\n", s);

    #ifdef COMPILE_DEBUG
        fprintf(stderr, "note: in function '%s'\n", func);
    #else
        (void)func;
    #endif // COMPILE_DEBUG

    return 1;
}

#define errf(fmt, ...) _errf(__func__, fmt, __VA_ARGS__)
static error _errf(const char *func, char *fmt, ...) {
    fprintf(stderr, "error: ");
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");

    #ifdef COMPILE_DEBUG
        fprintf(stderr, "note: in function '%s'\n", func);
    #else
        (void)func;
    #endif // COMPILE_DEBUG

    return 1;
}

typedef struct Arena {
    void *mem;
    usize offset;
    usize cap;
    usize last_offset;
} Arena;

static error arena_init(Arena *arena, usize size) {
    arena->offset = 0;
    arena->cap = size; 
    arena->mem = calloc(1, size);
    if (arena->mem == NULL) return errf("allocation of %d bytes failed", size);
    return 0;
}

static void arena_align(Arena *arena, usize align) {
    usize modulo = arena->offset & (align - 1);
    if (modulo != 0) arena->offset += align - modulo;
}

#define ARENA_DEFAULT_ALIGNMENT (2 * sizeof(void *))
#define arena_alloc(arena, size, out) _arena_alloc(arena, size, (void **)(out))
static error _arena_alloc(Arena *arena, usize size, void **out) {
    arena_align(arena, ARENA_DEFAULT_ALIGNMENT);
    if (arena->offset + size >= arena->cap) return err("allocation failure");
    *out = (u8 *)arena->mem + arena->offset;
    arena->last_offset = arena->offset;
    arena->offset += size;
    return 0;
}

#define arena_realloc(arena, size, out) \
    _arena_realloc(arena, size, (void **)(out))
static error _arena_realloc(Arena *arena, usize size, void **out) {
    void *last_allocation = (u8 *)arena->mem + arena->last_offset;
    if (*out == last_allocation && arena->last_offset + size <= arena->cap) {
        return 0;
    }
    try (arena_alloc(arena, size, out));
    return 0;
}

static void arena_deinit(Arena *arena) {
    free(arena->mem);
    arena->offset = 0;
    arena->cap = 0;
}

// typedef struct Hashmap {
//     void *table;
// } Hashmap;

// static usize hash_djb2(void *_data, usize len) { 
//     usize hash = 5381;
//     u8 *data = _data;
//     for (usize i = 0; i < len; i += 1, data += 1) {
//         u8 byte = data[i];
//         hash += (hash << 5) + byte;
//     }
//     return hash;
// }

#define Slice(type) struct { type *ptr; usize len; }
#define slice_push(slice, item) (slice).ptr[(slice).len++] = (item)
#define slice_pop(slice) (slice).ptr[--(slice).len]

typedef Slice(u8) Str8;

#define str8(s) (Str8){ .ptr = (u8 *)s, .len = sizeof(s) - 1 }
#define str8_expand(s) (s).ptr, (s).len
#define str8_fmt(s) (int)(s).len, (s).ptr

static bool str8_eql(Str8 s1, Str8 s2) {
    if (s1.len != s2.len) return false;
    for (usize i = 0; i < s1.len; i += 1) {
        if (s1.ptr[i] != s2.ptr[i]) return false;
    }
    return true;
}

static Str8 str8_from_cstr(char *s) {
    if (s == NULL) return (Str8){ 0 };
    usize len = 0;
    while (s[len] != '\0') len += 1;
    return (Str8){ .ptr = (u8 *)s, .len = len };
}

static char *cstr_from_str8(Arena *arena, Str8 s) {
    char *cstr; arena_alloc(arena, s.len + 1, &cstr);
    for (usize i = 0; i < s.len; i += 1) cstr[i] = s.ptr[i];
    cstr[s.len] = '\0';
    return cstr;
}

// Only bases <= 10
static error str8_from_int_base(
    Arena *arena, 
    usize _num, 
    usize base, 
    Str8 *out
) {
    out->len = 0;
    usize num = _num;

    do {
        num /= base;
        out->len += 1;
    } while (num > 0);
    
    try (arena_alloc(arena, out->len, &out->ptr));

    num = _num;
    for (i64 i = out->len - 1; i >= 0; i -= 1) {
        out->ptr[i] = (num % base) + '0';
        num /= base;
    }

    return 0;
}

static void str8_offset(Str8 *s, usize offset) {
    s->ptr += offset;
    s->len -= offset;
}

static Str8 str8_range(Str8 s, usize beg, usize end) {
    return (Str8){
        .ptr = s.ptr + beg,
        .len = end - beg,
    };
}

static error read_file(Arena *arena, Str8 path, char *mode, Str8 *out) {
    FILE *fp = NULL;
    if (path.len == 0) return err("empty path");
    if (mode == NULL) return err("invalid mode");

    if ((fp = fopen((char *)path.ptr, mode)) == NULL) {
        return err("failed to open file");
    }
    
    fseek(fp, 0L, SEEK_END);
    usize filesize = ftell(fp);
    try (arena_alloc(arena, filesize + 1, &out->ptr));

    fseek(fp, 0L, SEEK_SET);
    out->len = fread(out->ptr, sizeof(u8), filesize, fp);
    out->ptr[out->len] = '\0';

    if (ferror(fp)) {
        fclose(fp);
        return err("error reading file");
    }

    fclose(fp);
    return 0;
}

#endif // BASE
