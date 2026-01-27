// https://github.com/felix-u 2026-01-20
// Public domain. NO WARRANTY - use at your own risk.

#if !defined(FILESYSTEM_H)
#define FILESYSTEM_H


#if defined(_WIN32)
    #define FS_OS_WINDOWS
#else
    // NOTE(felix): it may be good to distinguish between non-Windows OSes
    #define FS_OS_POSIX
#endif

typedef struct {
    #if defined(FS_OS_WINDOWS)
        void *handle;
    #elif defined(FS_OS_POSIX)
        FILE *handle;
    #else
        #error "UNSUPPORTED OS"
    #endif
} fs_File;

#if !defined(FILESYSTEM_FUNCTION)
    #define FILESYSTEM_FUNCTION
#endif

typedef enum {
    fs_File_Flag_READ = 1 << 0,
    fs_File_Flag_WRITE = 1 << 1,
} fs_File_Flags;

FILESYSTEM_FUNCTION              _Bool fs_close(fs_File file);
FILESYSTEM_FUNCTION               void fs_absolute_path(const char *path, char *buffer, unsigned long long buffer_size);
FILESYSTEM_FUNCTION              _Bool fs_exists(const char *path);
FILESYSTEM_FUNCTION            fs_File fs_file_open_and_get_size(const char *path, fs_File_Flags flags, unsigned long long *size);
FILESYSTEM_FUNCTION unsigned long long fs_file_size(const char *path);
FILESYSTEM_FUNCTION              _Bool fs_file_write(fs_File file, void *bytes, unsigned long long size);
FILESYSTEM_FUNCTION              _Bool fs_make_directory(const char *relative_path);
FILESYSTEM_FUNCTION            fs_File fs_open(const char *path, fs_File_Flags flags);
FILESYSTEM_FUNCTION unsigned long long fs_read_entire_file(fs_File file, char *buffer, unsigned long long size);
FILESYSTEM_FUNCTION              _Bool fs_remove_file(const char *relative_path);


#endif // FILESYSTEM_H


#if defined(FILESYSTEM_IMPLEMENTATION)


#if !defined(FILESYSTEM_ASSERT)
    #include <assert.h>
    #define FILESYSTEM_ASSERT assert
#endif

#if !defined(FILESYSTEM_NO_SYSTEM_INCLUDE)
    #if defined(FS_OS_WINDOWS)
        #if defined(_MSC_VER)
            #pragma comment(lib, "Kernel32.lib")
        #endif
        #include <windows.h>
    #elif defined(FS_OS_POSIX)
        #include <stdio.h>
        #include <stdlib.h>
    #endif
#endif

FILESYSTEM_FUNCTION _Bool fs_close(fs_File file) {
    _Bool ok = 0;
    if (file.handle == 0) return ok;

    #if defined(FS_OS_WINDOWS)
    {
        ok = !!CloseHandle(file.handle);
    }
    #elif defined(FS_OS_POSIX)
    {
        fclose(file.handle);
        ok = 1;
    }
    #else
        #error "UNSUPPORTED OS"
    #endif

    return ok;
}

FILESYSTEM_FUNCTION void fs_absolute_path(const char *path, char *buffer, unsigned long long buffer_size) {
    #if defined(FS_OS_WINDOWS)
    {
        fs_File temp = fs_open(path, 0);
        if (temp.handle != 0) {
            FILESYSTEM_ASSERT(buffer_size <= 0xffffffff);
            unsigned long size = (unsigned long)buffer_size;
            GetFinalPathNameByHandleA(temp.handle, buffer, size, 0);
        }
        fs_close(temp);
    }
    #elif defined(FS_OS_POSIX)
    {
        FILESYSTEM_ASSERT(buffer_size >= 4096);
        realpath(path, buffer);
    }
    #else
        #error "UNSUPPORTED OS"
    #endif
}

FILESYSTEM_FUNCTION _Bool fs_exists(const char *path) {
    _Bool exists = 0;

    #if defined(FS_OS_WINDOWS)
    {
        unsigned long attributes = GetFileAttributesA(path);
        exists = attributes != INVALID_FILE_ATTRIBUTES;
    }
    #elif defined(FS_OS_POSIX)
    {
        struct stat status = {0};
        exists = stat(path, &status) == 0;
    }
    #else
        #error "UNSUPPORTED OS"
    #endif

    return exists;
}

FILESYSTEM_FUNCTION fs_File fs_file_open_and_get_size(const char *path, fs_File_Flags flags, unsigned long long *size) {
    fs_File file = fs_open(path, flags);
    if (file.handle != 0) {
        #if defined(FS_OS_WINDOWS)
        {
            int ok = GetFileSizeEx(file.handle, (LARGE_INTEGER *)size);
            if (!ok) *size = 0;
        }
        #elif defined(FS_OS_POSIX)
        {
            *size = fs_file_size(path);
        }
        #else
            #error "UNSUPPORTED OS"
        #endif
    }
    return file;
}

FILESYSTEM_FUNCTION unsigned long long fs_file_size(const char *path) {
    unsigned long long result = 0;

    #if defined(FS_OS_WINDOWS)
    {
        fs_File temp = fs_file_open_and_get_size(path, 0, &result);
        fs_close(temp);
    }
    #elif defined(FS_OS_POSIX)
    {
        struct stat status = {0};
        if (lstat(path, &status) == 0) {
            result = (unsigned long long)status.st_size;
        }
    }
    #else
        #error "UNSUPPORTED OS"
    #endif

    return result;
}

FILESYSTEM_FUNCTION _Bool fs_file_write(fs_File file, void *bytes, unsigned long long size) {
    _Bool ok = 0;
    if (file.handle == 0) return ok;

    #if defined(FS_OS_WINDOWS)
    {
        FILESYSTEM_ASSERT(size <= 0xffffffff);
        ok = !!WriteFile(file.handle, bytes, (unsigned long)size, 0, 0);
    }
    #elif defined(FS_OS_POSIX)
    {
        ok = 1;
        for (unsigned long long total_written = 0; total_written < size;) {
            unsigned long long written = fwrite((char *)bytes + total_written, 1, size - total_written, file.handle);
            if (written == 0) {
                ok = 0;
                break;
            }
            total_written += written;
        }
    }
    #else
        #error "UNSUPPORTED OS"
    #endif

    return ok;
}

FILESYSTEM_FUNCTION _Bool fs_make_directory(const char *relative_path) {
    _Bool ok = 0;

    #if defined(FS_OS_WINDOWS)
    {
        ok = !!CreateDirectoryA(relative_path, 0);
    }
    #elif defined(FS_OS_POSIX)
    {
        mode_t mode = 0755;
        int result = mkdir(relative_path, mode);
        ok = result == 0;
    }
    #else
        #error "UNSUPPORTED OS"
    #endif

    return ok;
}

FILESYSTEM_FUNCTION fs_File fs_open(const char *path, fs_File_Flags flags) {
    fs_File result = {0};

    flags += !flags * fs_File_Flag_READ;

    _Bool read = !!(flags & fs_File_Flag_READ);
    _Bool write = !!(flags & fs_File_Flag_WRITE);
    FILESYSTEM_ASSERT(read || write);
    FILESYSTEM_ASSERT(read ^ write); // TODO(felix): correctly handle read+write

    #if defined(FS_OS_WINDOWS)
    {
        unsigned long desired_access = (read * GENERIC_READ) | (write * GENERIC_WRITE);
        unsigned long creation_disposition = (read * OPEN_EXISTING) | (write * CREATE_ALWAYS);
        void *handle = CreateFileA(path, desired_access, FILE_SHARE_READ | FILE_SHARE_DELETE, 0, creation_disposition, FILE_ATTRIBUTE_NORMAL, 0);
        if (handle != INVALID_HANDLE_VALUE) result.handle = handle;
    }
    #elif defined(FS_OS_POSIX)
    {
        const char *mode = 0;
        if (read) mode = "rb";
        if (write) mode = "wb";
        FILE *handle = fopen(path, mode);
        result.handle = handle;
    }
    #else
        #error "UNSUPPORTED OS"
    #endif

    return result;
}

FILESYSTEM_FUNCTION unsigned long long fs_read_entire_file(fs_File file, char *buffer, unsigned long long size) {
    unsigned long long read = 0;
    if (file.handle == 0) return read;
    FILESYSTEM_ASSERT(buffer != 0);

    #if defined(FS_OS_WINDOWS)
    {
        unsigned long bytes_read = 0;
        (void)bytes_read;

        int ok = !!ReadFile(file.handle, buffer, (unsigned)size, &bytes_read, 0);
        if (ok) read = (unsigned long long)bytes_read;
    }
    #elif defined(FS_OS_POSIX)
    {
        rewind(file.handle);
        unsigned long long bytes_read = fread(buffer, 1, size, file.handle);
        _Bool ok = bytes_read == size;
        if (ok) read = bytes_read;
    }
    #else
        #error "UNSUPPORTED OS"
    #endif

    return read;
}

FILESYSTEM_FUNCTION _Bool fs_remove_file(const char *relative_path) {
    _Bool ok = 0;

    #if defined(FS_OS_WINDOWS)
    {
        ok = !!DeleteFileA(relative_path);
    }
    #elif defined(FS_OS_POSIX)
    {
        int result = remove(relative_path);
        ok = result == 0;
    }
    #else
        #error "UNSUPPORTED OS"
    #endif

    return ok;
}

#endif // FILESYSTEM_IMPLEMENTATION
