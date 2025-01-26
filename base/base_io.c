static String file_read_bytes_relative_path(Arena *arena, char *path, usize max_bytes) {
    // NOTE(felix): this is because of ReadFile() taking a DWORD to specify the number of bytes to read, which is a u32
    assert(max_bytes <= UINT32_MAX);

    if (path == 0) return (String){0};

    // NOTE(felix): not sure about this. See https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea
    DWORD share_mode = FILE_SHARE_READ;

    HANDLE file = CreateFileA(path, GENERIC_READ, share_mode, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (file == INVALID_HANDLE_VALUE) {
        err("unable to open file '%'", fmt(cstring, path));
        return (String){0};
    }

    Array_u8 bytes = { .arena = arena };

    usize file_size = 0;
    if (!GetFileSizeEx(file, (PLARGE_INTEGER)&file_size)) {
        err("unable to get size of file '%' after opening", fmt(cstring, path));
        goto end;
    }

    if (file_size > max_bytes) {
        err("file '%' is % bytes, which is greater than the supplied maximum of % bytes", fmt(cstring, path), fmt(u64, file_size), fmt(u64, max_bytes));
        goto end;
    }

    array_ensure_capacity(&bytes, file_size);

    u32 num_bytes_read = 0;
    if (!ReadFile(file, bytes.ptr, (u32)file_size, (LPDWORD)&num_bytes_read, 0)) {
        err("unable to read bytes of file '%' after opening", fmt(cstring, path));
        goto end;
    }
    assert(file_size == num_bytes_read);
    bytes.len = file_size;

    end:
    CloseHandle(file);
    return bit_cast(String) bytes;
}

static void file_write_bytes_to_relative_path(char *path, String bytes) {
    discard(path);
    discard(bytes);
    panic("TODO");
}

static void log_internal_with_location(char *file, usize line, char *func, char *format, ...) {
    va_list args;
    va_start(args, format);
    print_var_args(format, args);
    va_end(args);

    #if BUILD_DEBUG
        print("\n%:%:%(): first logged here\n", fmt(cstring, file), fmt(u64, line), fmt(cstring, func));
    #else
        discard(file); discard(line); discard(func);
    #endif
}

static void print(char *format, ...) {
    va_list args;
    va_start(args, format);
    print_var_args(format, args);
    va_end(args);
}

static void print_var_args(char *format, va_list args) {
    // TODO(felix); might be a nicer way to do this
    static Arena arena = {0};
    if (arena.mem == 0) arena = arena_init(2048);

    Arena_Temp temp = arena_temp_begin(&arena);

    String_Builder output = { .arena = &arena };
    string_builder_printf_var_args(&output, format, args);

    String str = bit_cast(String) output;

    #if OS_WINDOWS
        OutputDebugStringA((char *)str.ptr);

        // TODO(felix): stderr support
        HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
        assert(console_handle != INVALID_HANDLE_VALUE);

        u32 num_chars_written = 0;
        assert(str.len <= UINT32_MAX);
        assert(WriteConsole(console_handle, str.ptr, (u32)str.len, (LPDWORD)&num_chars_written, 0));
        discard(num_chars_written);
    #else
        #error "unimplemented"
    #endif

    arena_temp_end(temp);
}
