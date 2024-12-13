static Str8 file_read_bytes_relative_path(Arena *arena, char *path, usize max_bytes) {
    // NOTE(felix): this is because of ReadFile() taking a DWORD to specify the number of bytes to read, which is a u32
    assert(max_bytes <= UINT32_MAX);

    if (path == 0) return (Str8){0};

    // NOTE(felix): not sure about this. See https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea
    DWORD share_mode = FILE_SHARE_READ;

    HANDLE file = CreateFileA(path, GENERIC_READ, share_mode, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (file == INVALID_HANDLE_VALUE) {
        errf("unable to open file '%'", fmt(cstring, path));
        return (Str8){0};
    }

    Array_u8 bytes = {0};

    usize file_size = 0;
    if (!GetFileSizeEx(file, (PLARGE_INTEGER)&file_size)) {
        errf("unable to get size of file '%' after opening", fmt(cstring, path));
        goto end;
    }

    if (file_size > max_bytes) {
        errf("file '%' is % bytes, which is greater than the supplied maximum of % bytes", fmt(cstring, path), fmt(u64, file_size), fmt(u64, max_bytes));
        goto end;
    }

    arena_alloc_array(arena, &bytes, file_size);

    u32 num_bytes_read = 0;
    if (!ReadFile(file, bytes.ptr, (u32)file_size, (LPDWORD)&num_bytes_read, 0)) {
        errf("unable to read bytes of file '%' after opening", fmt(cstring, path));
        goto end;
    }
    assert(file_size == num_bytes_read);
    bytes.len = file_size;

    end:
    CloseHandle(file);
    return (Str8)slice_from_array(bytes);
}

static void log_internal_with_location(char *file, usize line, char *func, char *s) {
    logf_internal_with_location(file, line, func, "%", fmt(cstring, s));
}

static void logf_internal_with_location(char *file, usize line, char *func, char *format, ...) {
    // TODO(felix): might be a nicer way to do this
    static Arena log_arena = {0};
    if (log_arena.mem == 0) log_arena = arena_init(2048);

    Arena_Temp temp = arena_temp_begin(&log_arena);

    Str8_Builder output = { .arena = &log_arena };

    va_list args;
    va_start(args, format);
    str8_builder_printf_var_args(&output, format, args);
    va_end(args);

    #if BUILD_DEBUG
        str8_builder_printf(&output, "\n%:%:%(): first logged here\n", fmt(cstring, file), fmt(u64, line), fmt(cstring, func));
    #else
        discard(file); discard(line); discard(func);
    #endif

    Str8 str = str8_from_str8_builder(output);

    #if OS_WINDOWS
        OutputDebugStringA((char *)str.ptr);
    #endif

    // TODO(felix): stderr support
    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    assert(console_handle != INVALID_HANDLE_VALUE);

    u32 num_chars_written = 0;
    assert(str.len <= UINT32_MAX);
    assert(WriteConsole(console_handle, str.ptr, (u32)str.len, (LPDWORD)&num_chars_written, 0));
    discard(num_chars_written);

    arena_temp_end(temp);
}
