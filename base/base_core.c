static void _err(char *file, usize line, const char *func, char *s) {
    fprintf(stderr, "error: %s\n", s);
    #ifdef DEBUG
        fprintf(
            stderr,
            "%s:%zu:%s(): error first returned here\n",
            file, line, func
        );
    #else
        discard(file);
        discard(line);
        discard(func);
    #endif // DEBUG
}

static void _errf(char *file, usize line, const char *func, char *fmt, ...) {
    fprintf(stderr, "error: ");
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
    #ifdef DEBUG
        fprintf(
            stderr,
            "%s:%zu:%s(): error first returned here\n",
            file, line, func
        );
    #else
        discard(file);
        discard(line);
        discard(func);
    #endif // DEBUG
}

static inline usize _next_power_of_2(usize n) {
    usize result = 1;
    while (result < n) result *= 2;
    return result;
}

static void _array_push_slice(
    Arena *arena, Array_void *array, Slice_void *slice, usize size
) {
    usize new_len = array->len + slice->len;
    if (new_len >= array->cap) {
        usize new_cap = _next_power_of_2(new_len);
        _arena_realloc_array(arena, array, new_cap, size);
        if (array->cap == 0) return;
    }
    memmove(
        (u8 *)array->ptr + (array->len * size),
        slice->ptr,
        slice->len * size
    );
    array->len = new_len;
}

static inline void
_array_push(Arena *arena, Array_void *array, void *item, usize size) {
    Slice_void slice = { .ptr = item, .len = 1 };
    _array_push_slice(arena, array, &slice, size);
}
