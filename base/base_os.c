#if defined(BASE_NO_IMPLEMENTATION) || defined(BASE_NO_IMPLEMENTATION_IO)


structdef(Os_File_Info) {
    bool exists;
    String full_path;
    u64 size;
    bool is_directory;
    // TODO(felix): creation, modification, and access time
};

static Os_File_Info os_file_info(Arena *arena, String relative_path);

static bool os_make_directory(Arena arena, String relative_path, u32 mode);

structdef(Os_Read_Entire_File_Arguments) { Arena *arena; String path; u64 max_bytes; };
#define os_read_entire_file(...) os_read_entire_file_((Os_Read_Entire_File_Arguments){ __VA_ARGS__ })
static String os_read_entire_file_(Os_Read_Entire_File_Arguments);

static void os_remove_file(Arena arena, String relative_path);
static bool os_write_entire_file(Arena scratch, String path, String bytes);

#define log_info(...) log_internal("info: " __VA_ARGS__)
#define log_internal(format, ...) log_internal_with_location(__FILE__, __LINE__, (char *)__func__, (format), formats_from_va_args(__VA_ARGS__))
static void log_internal_with_location(char *file, u64 line, char *func, char *format, Slice_Format arguments);

static void *os_heap_allocate(u64 byte_count);
static void  os_heap_free(void *pointer);

structdef(Os_Process_Run_Arguments) {
    Arena arena;
    Slice_String arguments;
    String directory;
    bool print_command_before_running;
    bool print_exit_code;
};
#define os_process_run(...) os_process_run_((Os_Process_Run_Arguments){ __VA_ARGS__ })
static u32 os_process_run_(Os_Process_Run_Arguments);

static void  os_write(String bytes);

#define print(format, ...) print_((format), formats_from_va_args(__VA_ARGS__))
static void  print_(char *format, Slice_Format arguments);


#else // IMPLEMENTATION


static void *os_heap_allocate(u64 byte_count) {
    void *pointer = 0;

    #if OS_WINDOWS
        pointer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, byte_count);
    #else
        pointer = malloc(byte_count);
    #endif

    ensure(pointer != 0);
    return pointer;
}

static void os_heap_free(void *pointer) {
    #if OS_WINDOWS
        HeapFree(GetProcessHeap(), 0, pointer);
    #else
        free(pointer);
    #endif
}

static Os_File_Info os_file_info(Arena *arena, String relative_path) {
    Os_File_Info info = {0};

    #if OS_WINDOWS
    {
        Scratch scratch = scratch_begin(arena);
        cstring path_as_cstring = cstring_from_string(scratch.arena, relative_path);
        HANDLE handle = CreateFileA(
            path_as_cstring,
            GENERIC_READ,
            FILE_SHARE_READ,
            0,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS,
            0
        );
        info.exists = handle != INVALID_HANDLE_VALUE;
        if (info.exists) {
            u32 attributes = GetFileAttributesA(path_as_cstring);
            info.is_directory = !!(attributes & FILE_ATTRIBUTE_DIRECTORY);
        }
        scratch_end(scratch);

        if (info.exists) {
            u8 full_path[MAX_PATH];
            u32 bytes = GetFinalPathNameByHandleA(handle, cast(char *) full_path, sizeof full_path, 0);
            info.full_path = arena_push(arena, (String){ .data = full_path, .count = bytes });

            GetFileSizeEx(handle, (PLARGE_INTEGER)&info.size);

            CloseHandle(handle);
        }
    }
    #else
        discard arena;
        discard relative_path;
        panic("unimplemented");
    #endif

    return info;
}

static bool os_make_directory(Arena arena, String relative_path, u32 mode) {
    bool ok = false;
    Scratch scratch = scratch_begin(&arena);

    #if OS_WINDOWS
    {
        discard mode;
        BOOL win32_ok = CreateDirectoryA(cstring_from_string(scratch.arena, relative_path), 0);
        ok = !!win32_ok;

        // TODO(felix): actually get error from win32
        log_error("error creating directory '%'", fmt(String, relative_path));
    }
    #else
        discard arena;
        discard relative_path;
        discard mode;
        panic("unimplemented");
    #endif

    scratch_end(scratch);
    return ok;
}

static String os_read_entire_file_(Os_Read_Entire_File_Arguments arguments) {
    String path = arguments.path;
    u64 max_bytes = arguments.max_bytes;
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
        u64 file_size = 0;
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
            reserve(&bytes, file_size);

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
            seek_ok = fseek(file_handle, 0, SEEK_END) == 0;
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
                    // goto end; // TODO(felix): is it correct not to have a goto here?
                }
            }
        }

        bool read_ok = false;
        if (file_size_ok) {
            rewind(file_handle);

            reserve(&bytes, file_size);

            u64 num_bytes_read = fread(bytes.data, 1, file_size, file_handle);
            bytes.count = num_bytes_read;
            read_ok = num_bytes_read == file_size;
            if (!read_ok) log_error("unable to read entire file '%'; could only read %/% bytes", fmt(String, path), fmt(u64, num_bytes_read), fmt(u64, file_size));
        }

        if (file_ok) fclose(file_handle);
    #else
        #error "unsupported OS"
    #endif

    return bit_cast(String) bytes;
}

static void os_remove_file(Arena arena, String relative_path) {
    Scratch scratch = scratch_begin(&arena);

    #if OS_WINDOWS
    {
        BOOL ok = DeleteFileA(cstring_from_string(scratch.arena, relative_path));
        if (!ok) {
            // TODO(felix): actually get win32 error
            log_error("error removing file '%'", fmt(String, relative_path));
        }
    }
    #else
        discard relative_path;
        panic("unimplemented");
    #endif

    scratch_end(scratch);
}

static bool os_write_entire_file(Arena scratch, String path, String bytes) {
    #if OS_WINDOWS
        u64 dword_max = UINT32_MAX;
        assert(bytes.count <= dword_max);

        // NOTE(felix): not sure about this. See https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea
        DWORD share_mode = 0;
        Scratch scratch_ = scratch_begin(&scratch);
        char *path_as_cstring = cstring_from_string(scratch_.arena, path);
        HANDLE file_handle = CreateFileA(path_as_cstring, GENERIC_WRITE, share_mode, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
        scratch_end(scratch_);
        if (file_handle == INVALID_HANDLE_VALUE) {
            log_error("unable to open file '%'", fmt(String, path));
            return false;
        }

        bool ok = WriteFile(file_handle, bytes.data, (DWORD)bytes.count, 0, 0);
        if (!ok) {
            log_error("error writing to file '%'", fmt(String, path));
        }

        CloseHandle(file_handle);
        return ok;
    #elif OS_LINUX || OS_MACOS || OS_EMSCRIPTEN
        // TODO(felix): use syscalls instead of libc filesystem API

        int open_flags = O_WRONLY | O_CREAT | O_TRUNC;
        mode_t open_permissions = 0644;
        Scratch scratch_ = scratch_begin(&scratch);
        char *path_as_cstring = cstring_from_string(scratch_.arena, path);
        int file_handle = open(path_as_cstring, open_flags, open_permissions);
        scratch_end(scratch_);
        if (file_handle == -1) {
            log_error("unable to open file '%'", fmt(String, path));
            return false;
        }

        bool ok = false;
        for (u64 written_bytes = 0; written_bytes < bytes.count;) {
            i64 wrote_this_time = write(file_handle, bytes.data + written_bytes, bytes.count - written_bytes);
            if (wrote_this_time == -1) {
                log_error("error writing to file '%'", fmt(String, path));
                goto end;
            }
            written_bytes += (u64)wrote_this_time;
        }
        ok = true;

        end:
        close(file_handle);
        return ok;
    #else
        #error "unsupported OS"
    #endif
}

static void log_internal_with_location(char *file, u64 line, char *func, char *format, Slice_Format arguments) {
    print_(format, arguments);
    print("\n");

    #if BUILD_DEBUG
        print("%:%:%(): logged here\n", fmt(cstring, file), fmt(u64, line), fmt(cstring, func));
    #else
        discard(file); discard(line); discard(func);
    #endif
}

static u32 os_process_run_(Os_Process_Run_Arguments arguments) {
    Scratch scratch = scratch_begin(&arguments.arena);
    u32 exit_code = 1;

    #if OS_WINDOWS
    {
        cstring application_name = 0;

        String_Builder command_line = { .arena = scratch.arena };
        for (u64 i = 0; i < arguments.arguments.count; i += 1) {
            String argument = arguments.arguments.data[i];
            if (i > 0) string_builder_push(&command_line, char, ' ');
            string_builder_push(&command_line, String, argument);
        }
        string_builder_null_terminate(&command_line);

        if (arguments.print_command_before_running) {
            if (arguments.directory.count > 0) print("[.\\%] ", fmt(String, arguments.directory));
            print("%\n", fmt(String, command_line.string));
        }

        SECURITY_ATTRIBUTES *process_attributes = 0;
        SECURITY_ATTRIBUTES *thread_attributes = 0;
        BOOL inherit_handles = false;
        u32 creation_flags = 0;
        void *environment = 0;
        cstring current_directory = arguments.directory.count > 0 ? cstring_from_string(scratch.arena, arguments.directory) : 0;
        STARTUPINFOA startup_info = {0};
        PROCESS_INFORMATION process_info = {0};

        BOOL ok = CreateProcessA(
            application_name,
            (char *)command_line.data,
            process_attributes,
            thread_attributes,
            inherit_handles,
            creation_flags,
            environment,
            current_directory,
            &startup_info,
            &process_info
        );

        if (!ok) {
            log_error("unable to start process '%'; CreateProcessA() returned %",
                fmt(String, arguments.arguments.data[0]), fmt(u32, GetLastError())
            );
        } else {
            WaitForSingleObject(process_info.hProcess, INFINITE);
            GetExitCodeProcess(process_info.hProcess, (LPDWORD)&exit_code);
            if (arguments.print_exit_code) print("'%' exited: %\n", fmt(String, arguments.arguments.data[0]), fmt(u32, exit_code));
            CloseHandle(process_info.hProcess);
            CloseHandle(process_info.hThread);
        }
    }
    #else
        panic("unimplemented");
    #endif

    scratch_end(scratch);
    return exit_code;
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
        i64 bytes_written = write(stdout_handle, string.data, string.count);
        discard(bytes_written);

    #else
        #error "unimplemented"

    #endif
}

static void print_(char *format, Slice_Format arguments) {
    // TODO(felix): this should use the thread_local arena once we add that system
    static Arena arena = {0};
    if (arena.mem == 0) arena = arena_init(8096);

    Scratch temp = scratch_begin(&arena);

    String_Builder output = { .arena = &arena };
    string_builder_print_(&output, format, arguments);

    string_builder_null_terminate(&output);

    #if OS_WINDOWS && BUILD_DEBUG
        OutputDebugStringA((char *)output.data);
    #endif

    os_write(output.string);

    scratch_end(temp);
}

#endif // defined(BASE_NO_IMPLEMENTATION) || defined(BASE_NO_IMPLEMENTATION_IO)
