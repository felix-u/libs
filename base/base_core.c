static void _log_internal(FILE *out, char *file, usize line, const char *func, char *s) {
    fprintf(out, "%s\n", s);
    #if BUILD_DEBUG
        #if OS_WINDOWS
            char buf[2048] = {0};
            snprintf(buf, 2047, "%s\n%s:%zu:%s(): first logged here\n", s, file, line, func);
            OutputDebugStringA(buf);
        #endif // OS_WINDOWS
        fprintf(out, "%s:%zu:%s(): first logged here\n", file, line, func);
    #else
        discard(file); discard(line); discard(func);
    #endif // BUILD_DEBUG
}

static void _logf_internal(FILE *out, char *file, usize line, const char *func, char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(out, fmt, args);

    #if defined(BUILD_DEBUG) && OS_WINDOWS
        char buf[2048] = {0};
        int move_along = vsnprintf(buf, 2047, fmt, args);
        snprintf(buf + move_along, 2047 - move_along, "\n%s:%zu:%s(): first logged here\n", file, line, func);
        OutputDebugStringA(buf);
    #endif // BUILD_DEBUG && OS_WINDOWS

    va_end(args);
    fprintf(out, "\n");

    #if BUILD_DEBUG
        fprintf(out, "%s:%zu:%s(): first logged here\n", file, line, func);
    #else
        discard(file); discard(line); discard(func);
    #endif // BUILD_DEBUG
}

static inline usize _next_power_of_2(usize n) {
    usize result = 1;
    while (result < n) result *= 2;
    return result;
}

static inline void _array_push(Arena *arena, Array_void *array, void *item, usize size) {
    Slice_void slice = { .ptr = item, .len = 1 };
    _array_push_slice(arena, array, &slice, size);
}

static inline void _array_push_assume_capacity(Array_void *array, void *item, usize size) {
    usize new_len = array->len + 1;
    assume(new_len <= array->cap);
    memmove((u8 *)array->ptr + (array->len * size), item, size);
    array->len = new_len;
}

static void _array_push_slice(Arena *arena, Array_void *array, Slice_void *slice, usize size) {
    usize new_len = array->len + slice->len;
    if (new_len > array->cap) {
        usize new_cap = _next_power_of_2(new_len);
        _arena_realloc_array(arena, array, new_cap, size);
        if (array->cap == 0) return;
    }
    memmove((u8 *)array->ptr + (array->len * size), slice->ptr, slice->len * size);
    array->len = new_len;
}

static void _array_push_slice_assume_capacity(Array_void *array, Slice_void *slice, usize size) {
    usize new_len = array->len + slice->len;
    assume(new_len <= array->cap);
    memmove((u8 *)array->ptr + (array->len * size), slice->ptr, slice->len * size);
    array->len = new_len;
}
