#if defined(BASE_NO_IMPLEMENTATION) || defined(BASE_NO_IMPLEMENTATION_IO)

structdef(Os_Read_Entire_File_Arguments) { Arena *arena; String path; usize max_bytes; };
#define os_read_entire_file(...) os_read_entire_file_argument_struct((Os_Read_Entire_File_Arguments){ __VA_ARGS__ })
static String os_read_entire_file_argument_struct(Os_Read_Entire_File_Arguments);
static bool file_write_bytes_to_relative_path(char *path, String bytes);

#define log_info(...) log_internal("info: " __VA_ARGS__)
#define log_internal(...) log_internal_with_location(__FILE__, __LINE__, (char *)__func__, __VA_ARGS__)
static void log_internal_with_location(char *file, usize line, char *func, char *format, ...);

static void os_write(String bytes);
static void print(char *format, ...);
static void print_var_args(char *format, va_list args);


#else // IMPLEMENTATION


static String os_read_entire_file_argument_struct(Os_Read_Entire_File_Arguments arguments) {
    String path = arguments.path;
    usize max_bytes = arguments.max_bytes;
    if (max_bytes == 0) max_bytes = UINT32_MAX;

    if (path.count == 0) return (String){0};

    char *path_cstring = cstring_from_string(arguments.arena, path);
    Array_u8 bytes = { .arena = arguments.arena };

    #if OS_WINDOWS
        // NOTE(felix): this is because of ReadFile() taking a DWORD to specify the number of bytes to read, which is a u32
        assert(max_bytes <= UINT32_MAX);

        // NOTE(felix): not sure about this. See https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea
        DWORD share_mode = FILE_SHARE_READ;

        HANDLE file = CreateFileA(path_cstring, GENERIC_READ, share_mode, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        bool file_ok = file != INVALID_HANDLE_VALUE;
        if (!file_ok) log_error("unable to open file '%'", fmt(String, path));

        bool file_size_ok = false;
        usize file_size = 0;
        if (file_ok) {
            BOOL ok = GetFileSizeEx(file, (PLARGE_INTEGER)&file_size);
            file_size_ok = ok != false;
            if (!file_size_ok) log_error("unable to get size of file '%' after opening", fmt(String, path));
        }

        if (file_size > max_bytes) {
            log_error("file '%' is % bytes, which is greater than the supplied maximum of % bytes", fmt(String, path), fmt(u64, file_size), fmt(u64, max_bytes));
            file_size_ok = false;
        }

        bool read_ok = false;
        if (file_size_ok) {
            array_ensure_capacity(&bytes, file_size);

            u32 num_bytes_read = 0;
            BOOL ok = ReadFile(file, bytes.data, (u32)file_size, (LPDWORD)&num_bytes_read, 0);
            read_ok = ok != false;
            if (!read_ok) log_error("unable to read bytes of file '%' after opening", fmt(String, path));

            assert(file_size == num_bytes_read);
            bytes.count = file_size;
        }


        if (file_ok) CloseHandle(file);
    #elif OS_LINUX || OS_MACOS || OS_EMSCRIPTEN
        // TODO(felix): use open & read instead of the libc filesystem API
        FILE *file_handle = fopen(path_cstring, "rb");
        bool file_ok = file_handle != 0;
        if (!file_ok) log_error("unable to open file '%'", fmt(String, path));

        bool seek_ok = false;
        if (file_ok) {
            seek_ok = fseek(file_handle, 0, SEEK_END) != 0;
            if (!seek_ok) log_error("unable to seek file '%'", fmt(String, path));
        }

        bool file_size_ok = false;
        u64 file_size = 0;
        if (seek_ok) {
            i64 file_size_signed = ftell(file_handle);
            file_size_ok = file_size_signed != -1;
            if (!file_size_ok) log_error("error reading offset after seeking file '%'", fmt(String, path));
            else {
                file_size = (u64)file_size_signed;
                if (file_size > max_bytes) {
                    log_error("file '%' is % bytes, which is greater than the supplied maximum of % bytes", fmt(String, path), fmt(u64, file_size), fmt(u64, max_bytes));
                    goto end;
                }
            }
        }

        bool read_ok = false;
        if (file_size_ok) {
            rewind(file_handle);

            array_ensure_capacity(&bytes, file_size);

            usize num_bytes_read = fread(bytes.data, 1, file_size, file_handle);
            bytes.count = num_bytes_read;
            read_ok = num_bytes_read == file_size;
            if (!read_ok) log_error("unable to read entire file '%'; could only read %/% bytes", fmt(String, path), fmt(u64, num_bytes_read), fmt(u64, file_size));
        }

        if (file_ok) fclose(file_handle);
    #else
        #error "unsupported OS"
    #endif

    return bytes.slice;
}

static bool file_write_bytes_to_relative_path(char *path, String bytes) {
    #if OS_WINDOWS
        usize dword_max = UINT32_MAX;
        assert(bytes.count <= dword_max);

        // NOTE(felix): not sure about this. See https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea
        DWORD share_mode = 0;
        HANDLE file_handle = CreateFileA(path, GENERIC_WRITE, share_mode, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
        if (file_handle == INVALID_HANDLE_VALUE) {
            log_error("unable to open file '%'", fmt(cstring, path));
            return false;
        }

        bool ok = WriteFile(file_handle, bytes.data, (DWORD)bytes.count, 0, 0);
        if (!ok) {
            log_error("error writing to file '%'", fmt(cstring, path));
        }

        CloseHandle(file_handle);
        return ok;
    #elif OS_LINUX || OS_MACOS || OS_EMSCRIPTEN
        // TODO(felix): use syscalls instead of libc filesystem API

        int open_flags = O_WRONLY | O_CREAT | O_TRUNC;
        mode_t open_permissions = 0644;
        int file_handle = open(path, open_flags, open_permissions);
        if (file_handle == -1) {
            log_error("unable to open file '%'", fmt(cstring, path));
            return false;
        }

        bool ok = false;
        for (usize written_bytes = 0; written_bytes < bytes.count;) {
            isize wrote_this_time = write(file_handle, bytes.data + written_bytes, bytes.count - written_bytes);
            if (wrote_this_time == -1) {
                log_error("error writing to file '%'", fmt(cstring, path));
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

    print("\n");

    #if BUILD_DEBUG
        print("%:%:%(): logged here\n", fmt(cstring, file), fmt(u64, line), fmt(cstring, func));
    #else
        discard(file); discard(line); discard(func);
    #endif
}

static void os_write(String string) {
    // TODO(felix): stderr support
    // NOTE(felix): can't use assert in this function because panic() will call os_write, so we'll end up with a recursively failing assert and stack overflow. Instead, use `if (!condition) { breakpoint; abort(); }`
    #if OS_WINDOWS
        #if WINDOWS_SUBSYSTEM_WINDOWS
            discard(string);
        #else
            if (string.count > UINT32_MAX) { breakpoint; os_abort(); }

            HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
            if (console_handle == INVALID_HANDLE_VALUE) { breakpoint; os_abort(); }

            u32 num_chars_written = 0;
            BOOL ok = WriteFile(console_handle, string.data, (u32)string.count, (LPDWORD)&num_chars_written, 0);
            if (!ok) { breakpoint; os_abort(); }
            if (num_chars_written != string.count) { breakpoint; os_abort(); }
        #endif

    #elif OS_LINUX || OS_MACOS || OS_EMSCRIPTEN
        int stdout_handle = 1;
        isize bytes_written = write(stdout_handle, string.data, string.count);
        discard(bytes_written);

    #else
        #error "unimplemented"

    #endif
}

static void print(char *format, ...) {
    va_list args;
    va_start(args, format);
    print_var_args(format, args);
    va_end(args);
}

static void print_var_args(char *format, va_list args) {
    // TODO(felix): this should use the thread_local arena once we add that system
    static Arena arena = {0};
    if (arena.mem == 0) arena = arena_init(8096);

    Scratch temp = scratch_begin(&arena);

    String_Builder output = { .arena = &arena };
    string_builder_print_var_args(&output, format, args);

    string_builder_null_terminate(&output);
    String string = output.slice;

    #if OS_WINDOWS && BUILD_DEBUG
        OutputDebugStringA((char *)string.data);
    #endif

    os_write(string);

    scratch_end(temp);
}

#endif // defined(BASE_NO_IMPLEMENTATION) || defined(BASE_NO_IMPLEMENTATION_IO)

