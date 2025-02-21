static String file_read_bytes_relative_path(Arena *arena, char *path, usize max_bytes) {
    if (path == 0) return (String){0};
    Array_u8 bytes = { .arena = arena };

    #if OS_WINDOWS
        // NOTE(felix): this is because of ReadFile() taking a DWORD to specify the number of bytes to read, which is a u32
        assert(max_bytes <= UINT32_MAX);

        // NOTE(felix): not sure about this. See https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea
        DWORD share_mode = FILE_SHARE_READ;

        HANDLE file = CreateFileA(path, GENERIC_READ, share_mode, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        if (file == INVALID_HANDLE_VALUE) {
            err("unable to open file '%'", fmt(cstring, path));
            return (String){0};
        }

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
        if (!ReadFile(file, bytes.data, (u32)file_size, (LPDWORD)&num_bytes_read, 0)) {
            err("unable to read bytes of file '%' after opening", fmt(cstring, path));
            goto end;
        }
        assert(file_size == num_bytes_read);
        bytes.count = file_size;

        end:
        CloseHandle(file);
        return bit_cast(String) bytes;
    #elif OS_LINUX
        // TODO(felix): use open & read instead of the libc filesystem API
        FILE *file_handle = fopen(path, "rb");
        if (file_handle == 0) {
            err("unable to open file '%'", fmt(cstring, path));
            return (String){0};
        }

        int ok = 0;
        if (fseek(file_handle, 0, SEEK_END) != ok) {
            err("unable to seek file '%'", fmt(cstring, path)); 
            goto end;
        }

        i64 file_size_signed = ftell(file_handle);
        i64 not_ok = -1;
        if (file_size_signed == not_ok)  {
            err("error reading offset after seeking file '%'", fmt(cstring, path));
            goto end;
        }
        u64 file_size = (u64)file_size_signed;

        if (file_size > max_bytes) {
            err("file '%' is % bytes, which is greater than the supplied maximum of % bytes", fmt(cstring, path), fmt(u64, file_size), fmt(u64, max_bytes));
            goto end;
        }

        usize num_bytes_read = fread(bytes.data, 1, file_size, file_handle);
        if (num_bytes_read != file_size) {
            err("unable to read entire file '%'", fmt(cstring, path));
            goto end;
        }
        bytes.count = file_size;

        end:
        fclose(file_handle);
        return bit_cast(String) bytes;
    #else
        #error "unsupported OS"
    #endif
}

static bool file_write_bytes_to_relative_path(char *path, String bytes) {
    #if OS_WINDOWS
        usize dword_max = UINT32_MAX;
        assert(bytes.count <= dword_max);

        // NOTE(felix): not sure about this. See https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea
        DWORD share_mode = 0;
        HANDLE file_handle = CreateFileA(path, GENERIC_WRITE, share_mode, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
        if (file_handle == INVALID_HANDLE_VALUE) {
            err("unable to open file '%'", fmt(cstring, path));
            return false;
        }

        bool ok = WriteFile(file_handle, bytes.data, (DWORD)bytes.count, 0, 0);
        if (!ok) {
            err("error writing to file '%'", fmt(cstring, path));
        }

        CloseHandle(file_handle);
        return ok;
    #elif OS_LINUX
        // TODO(felix): use syscalls instead of libc filesystem API

        int open_flags = O_WRONLY | O_CREAT | O_TRUNC;
        mode_t open_permissions = 0644;
        int file_handle = open(path, open_flags, open_permissions);
        if (file_handle == -1) {
            err("unable to open file '%'", fmt(cstring, path));
            return false;
        }

        bool ok = false;
        for (usize written_bytes = 0; written_bytes < bytes.count;) {
            isize wrote_this_time = write(file_handle, bytes.data + written_bytes, bytes.count - written_bytes);
            if (wrote_this_time == -1) {
                err("error writing to file '%'", fmt(cstring, path));
                goto end;
            }
            written_bytes += (usize)wrote_this_time;
        }
        ok = true;

        end:
        close(file_handle);
        return ok;
    #else
        #error "unsupported OS"
    #endif
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
    string_builder_print_var_args(&output, format, args);

    String str = bit_cast(String) output;

    #if OS_WINDOWS
        OutputDebugStringA((char *)str.data);

        // TODO(felix): stderr support
        HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
        assert(console_handle != INVALID_HANDLE_VALUE);

        u32 num_chars_written = 0;
        assert(str.count <= UINT32_MAX);
        assert(WriteConsole(console_handle, str.data, (u32)str.count, (LPDWORD)&num_chars_written, 0));
        discard(num_chars_written);
    #elif OS_LINUX
        int stdout_handle = 1;
        isize bytes_written = write(stdout_handle, str.data, str.count);
        discard(bytes_written);
    #else
        #error "unimplemented"
    #endif

    arena_temp_end(temp);
}
