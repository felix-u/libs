static void _log_internal(FILE *out, char *file, usize line, const char *func, char *s) {
    _logf_internal(out, file, line, func, "%s", s);
}

static void _logf_internal(FILE *out, char *file, usize line, const char *func, char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    #if OS_EMSCRIPTEN
        char *log_string = 0;
        char buf[2048] = {0};

        #if BUILD_DEBUG
            int move_along = vsnprintf(buf, 2047, fmt, args);
            snprintf(buf + move_along, 2047 - move_along, "\n%s:%zu:%s(): first logged here\n", file, line, func);
            log_string = buf;
        #else
            Arena temp = { .mem = buf, .cap = 2047 };
            log_string = string8_printf(&temp, "%s", args).ptr;
        #endif // BUILD_DEBUG

        va_end(args);
        emscripten_console_log(log_string);
        return;
    #endif // OS_EMSCRIPTEN

    vfprintf(out, fmt, args);

    #if OS_WINDOWS && BUILD_DEBUG
        char buf[2048] = {0};
        int move_along = vsnprintf(buf, 2047, fmt, args);
        snprintf(buf + move_along, 2047 - move_along, "\n%s:%zu:%s(): first logged here\n", file, line, func);
        OutputDebugStringA(buf);
    #endif // OS_WINDOWS && BUILD_DEBUG

    va_end(args);

    fprintf(out, "\n");
    #if BUILD_DEBUG
        fprintf(out, "%s:%zu:%s(): first logged here\n", file, line, func);
    #else
        discard(file); discard(line); discard(func);
    #endif // BUILD_DEBUG
}

// TODO: simplify. current complexity is due to making valid capture to end of slice if end of slice is hit with no match.
// I think this makes sense - e.g. when splitting lines of a file, there may not be a newline between the last line and EOF.
static bool slice_split_scalar_explicit(Slice_void *slice, void *scalar, Slice_void *capture, usize size) {
    if (slice == 0 || scalar == 0 || capture == 0 || size == 0) return false;

    usize slice_len_bytes = slice->len * size;
    for (usize pos = 0; pos + size < slice_len_bytes;) {
        void *this = (char *)slice->ptr + pos;
        String8 this_bytes = { .ptr = this, .len = size };
        String8 scalar_bytes = { .ptr = scalar, .len = size };

        if (!string8_eql(this_bytes, scalar_bytes)) {
            pos += size;
            if (pos + size < slice_len_bytes) continue;
            pos = slice_len_bytes;
            *capture = (Slice_void)slice_range(*slice, 0, pos / size);
            *slice = (Slice_void){0};
        } else {
            *capture = (Slice_void)slice_range(*slice, 0, pos / size);
            *slice = (Slice_void)slice_range(*slice, pos / size + 1, slice->len);
        }

        return true;
    }

    return false;
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

static inline usize _next_power_of_2(usize n) {
    usize result = 1;
    while (result < n) result *= 2;
    return result;
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

static force_inline u32 byte_swap_u32(u32 bytes) {
    return ((bytes             ) << 24) |
           ((bytes & 0x0000ff00) <<  8) |
           ((bytes & 0x00ff0000) >>  8) |
           ((bytes             ) >> 24);
}
