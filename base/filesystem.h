// https://github.com/felix-u 2026-11-07
// Public domain. NO WARRANTY - use at your own risk.

#if !defined(FS_H)
#define FS_H


#if defined(__wasm__) || defined(__wasm32__)
    #define FS_OS_WASM
#elif defined(_WIN32)
    #define FS_OS_WINDOWS
#else
    // NOTE(felix): it may be good to distinguish between non-Windows OSes
    #define FS_OS_POSIX
#endif

#if !defined(FS_NO_SYSTEM_INCLUDE) && defined(FS_OS_POSIX)
    #include <stdio.h>
#endif

typedef struct {
    #if defined(FS_OS_WINDOWS)
        void *handle;
    #elif defined(FS_OS_POSIX)
        FILE *handle;
    #else
        void *handle;
    #endif
} fs_File;

typedef struct {
    const char *path;
    const char *name;
    unsigned long long size;
    unsigned long long modification_time;
    unsigned depth;
    _Bool is_folder;
} fs_File_Info;

typedef enum {
    fs_Folder_Iterator_Flag_RECURSIVE = 1 << 0,
} fs_Folder_Iterator_Flags;

#if !defined(FS_FOLDER_ITERATOR_MAX_DEPTH)
    #define FS_FOLDER_ITERATOR_MAX_DEPTH 64
#endif

enum {
    FS__NAME_MAX_UTF16_UNITS = 255,
    FS__UTF8_UNITS_PER_UTF16_UNIT_MAX = 3,
    FS__DIRECTORY_ENTRY_FIXED_SIZE = 64,
};

typedef struct {
    fs_File_Info info;
    fs_Folder_Iterator_Flags flags;
    _Bool had_error;

    #if defined(FS_OS_WINDOWS)
        void *handles[FS_FOLDER_ITERATOR_MAX_DEPTH];
        unsigned path_lengths[FS_FOLDER_ITERATOR_MAX_DEPTH];
        unsigned open_folder_count;
        unsigned long long entry[(FS__DIRECTORY_ENTRY_FIXED_SIZE + FS__NAME_MAX_UTF16_UNITS * 2) / sizeof(unsigned long long) + 1];

        char path[4096];
        char name[FS__NAME_MAX_UTF16_UNITS * FS__UTF8_UNITS_PER_UTF16_UNIT_MAX + 1];
    #endif
} fs_Folder_Iterator;

typedef enum {
    fs_File_Flag_READ  = 1 << 0,
    fs_File_Flag_WRITE = 1 << 1,
} fs_File_Flags;

#if !defined(FS_FUNCTION)
    #define FS_FUNCTION
#endif

FS_FUNCTION _Bool              fs_absolute_path(const char *path, char *buffer, unsigned long long buffer_size);
FS_FUNCTION _Bool              fs_close(fs_File file);
FS_FUNCTION _Bool              fs_executable_path(char *buffer, unsigned long long buffer_size);
FS_FUNCTION _Bool              fs_exists(const char *path);
FS_FUNCTION _Bool              fs_file_info(const char *path, fs_File_Info *info);
FS_FUNCTION fs_File            fs_file_open_and_get_size(const char *path, fs_File_Flags flags, unsigned long long *size);
FS_FUNCTION unsigned long long fs_file_size(const char *path);
FS_FUNCTION _Bool              fs_file_write(fs_File file, const void *bytes, unsigned long long size);
FS_FUNCTION _Bool              fs_folder_iterate(fs_Folder_Iterator *it);
FS_FUNCTION fs_Folder_Iterator fs_folder_iterator(const char *relative_path, fs_Folder_Iterator_Flags flags);
FS_FUNCTION void               fs_folder_iterator_end(fs_Folder_Iterator *it);
FS_FUNCTION _Bool              fs_make_folder(const char *relative_path);
FS_FUNCTION fs_File            fs_open(const char *path, fs_File_Flags flags);
FS_FUNCTION unsigned long long fs_read_entire_file(fs_File file, char *buffer, unsigned long long size);
FS_FUNCTION _Bool              fs_remove_file(const char *relative_path);
FS_FUNCTION _Bool              fs_rename(const char *old_path, const char *new_path);


#endif // FS_H


#if defined(FS_IMPLEMENTATION)


#if !defined(FS_ASSERT)
    #include <assert.h>
    #define FS_ASSERT assert
#endif

#if !defined(FS_NO_SYSTEM_INCLUDE)
    #if defined(FS_OS_WINDOWS)
        #if defined(_MSC_VER)
            #pragma comment(lib, "Ntdll.lib")
        #endif
        #include <windows.h>
        #include <winternl.h>
    #elif defined(FS_OS_POSIX)
        #include <stdio.h>
        #include <stdlib.h>
    #endif
#endif

#if defined(FS_OS_WINDOWS)
    typedef struct {
        long long CreationTime, LastAccessTime, LastWriteTime, ChangeTime;
        unsigned long FileAttributes;
    } FILE_BASIC_INFORMATION;
    typedef struct {
        long long AllocationSize, EndOfFile;
        unsigned long NumberOfLinks;
        unsigned char DeletePending, Directory;
    } FILE_STANDARD_INFORMATION;
    typedef struct {
        long long CreationTime, LastAccessTime, LastWriteTime, ChangeTime, AllocationSize, EndOfFile;
        unsigned long FileAttributes;
    } FILE_NETWORK_OPEN_INFORMATION;
    typedef struct {
        unsigned long NextEntryOffset, FileIndex;
        long long CreationTime, LastAccessTime, LastWriteTime, ChangeTime, EndOfFile, AllocationSize;
        unsigned long FileAttributes, FileNameLength;
        unsigned short FileName[1];
    } FILE_DIRECTORY_INFORMATION;
    typedef struct {
        unsigned long FileNameLength;
        unsigned short FileName[1];
    } FILE_NAME_INFORMATION;
    typedef struct {
        UNICODE_STRING Name;
    } OBJECT_NAME_INFORMATION;

    long NtOpenSymbolicLinkObject(void **LinkHandle, unsigned long DesiredAccess, OBJECT_ATTRIBUTES *ObjectAttributes);
    long NtQueryAttributesFile(OBJECT_ATTRIBUTES *ObjectAttributes, FILE_BASIC_INFORMATION *FileInformation);
    long NtQueryDirectoryFile(void *FileHandle, void *Event, void *ApcRoutine, void *ApcContext, IO_STATUS_BLOCK *IoStatusBlock, void *FileInformation, unsigned long Length, int FileInformationClass, unsigned char ReturnSingleEntry, UNICODE_STRING *FileName, unsigned char RestartScan);
    long NtQueryFullAttributesFile(OBJECT_ATTRIBUTES *ObjectAttributes, FILE_NETWORK_OPEN_INFORMATION *FileInformation);
    long NtQueryInformationFile(void *FileHandle, IO_STATUS_BLOCK *IoStatusBlock, void *FileInformation, unsigned long Length, int FileInformationClass);
    long NtQuerySymbolicLinkObject(void *LinkHandle, UNICODE_STRING *LinkTarget, unsigned long *ReturnedLength);
    long NtReadFile(void *FileHandle, void *Event, void *ApcRoutine, void *ApcContext, IO_STATUS_BLOCK *IoStatusBlock, void *Buffer, unsigned long Length, long long *ByteOffset, unsigned long *Key);
    long NtSetInformationFile(void *FileHandle, IO_STATUS_BLOCK *IoStatusBlock, void *FileInformation, unsigned long Length, int FileInformationClass);
    long NtWriteFile(void *FileHandle, void *Event, void *ApcRoutine, void *ApcContext, IO_STATUS_BLOCK *IoStatusBlock, const void *Buffer, unsigned long Length, long long *ByteOffset, unsigned long *Key);

    unsigned long RtlGetFullPathName_U(const unsigned short *FileName, unsigned long BufferLength, unsigned short *Buffer, unsigned short **FilePart);
#endif

#if defined(FS_OS_WINDOWS)
    #define FS_PATH_SEPARATOR '\\'
#else
    #define FS_PATH_SEPARATOR '/'
#endif

typedef enum {
    fs__Nt_Access_FILE_LIST_DIRECTORY  = 0x00000001,
    fs__Nt_Access_SYMBOLIC_LINK_QUERY  = 0x00000001,
    fs__Nt_Access_DELETE               = 0x00010000,
    fs__Nt_Access_SYNCHRONIZE          = 0x00100000,
    fs__Nt_Access_FILE_GENERIC_READ    = 0x00120089,
    fs__Nt_Access_FILE_GENERIC_WRITE   = 0x00120116,
} fs__Nt_Access;

typedef enum {
    fs__Nt_Share_READ   = 0x1,
    fs__Nt_Share_WRITE  = 0x2,
    fs__Nt_Share_DELETE = 0x4,
} fs__Nt_Share;

typedef enum {
    fs__Nt_Disposition_OPEN         = 1,
    fs__Nt_Disposition_CREATE       = 2,
    fs__Nt_Disposition_OVERWRITE_IF = 5,
} fs__Nt_Disposition;

typedef enum {
    fs__Nt_Create_Option_DIRECTORY_FILE           = 0x0001,
    fs__Nt_Create_Option_SYNCHRONOUS_IO_NONALERT  = 0x0020,
    fs__Nt_Create_Option_NON_DIRECTORY_FILE       = 0x0040,
    fs__Nt_Create_Option_DELETE_ON_CLOSE          = 0x1000,
} fs__Nt_Create_Option;

typedef enum {
    fs__Nt_Information_Class_FILE_DIRECTORY       = 1,
    fs__Nt_Information_Class_FILE_STANDARD        = 5,
    fs__Nt_Information_Class_FILE_NAME            = 9,
    fs__Nt_Information_Class_FILE_RENAME          = 10,
    fs__Nt_Information_Class_FILE_NORMALIZED_NAME = 48,
    fs__Nt_Information_Class_OBJECT_NAME          = 1,
} fs__Nt_Information_Class;

#define FS__NT_PATH_CAPACITY 32768 // UTF16 units

#if defined(FS_OS_WINDOWS)
    typedef union {
        FILE_NAME_INFORMATION file_name;
        OBJECT_NAME_INFORMATION object_name;
        unsigned char bytes[sizeof(OBJECT_NAME_INFORMATION) + FS__NT_PATH_CAPACITY * 2];
    } fs__Name_Storage;

    typedef union {
        struct {
            unsigned short wide_path[FS__NT_PATH_CAPACITY];
            unsigned short nt_path[FS__NT_PATH_CAPACITY];
        } paths;
        struct {
            fs__Name_Storage normalised_name;
            fs__Name_Storage object_name;
        } names;
    } fs__Path_Storage;
#endif

// NOTE(felix): WTF8 conversions: an unpaired UTF16 surrogate, which Windows allows in file names, becomes the corresponding 3-byte sequence and converts back.
static _Bool fs__utf16_from_utf8(unsigned short *destination, unsigned destination_capacity, unsigned *destination_count, const char *source, unsigned source_count) {
    _Bool ok = 1;
    const unsigned char *bytes = (const unsigned char *)source;
    unsigned read = 0;
    unsigned written = 0;

    while (ok && read < source_count) {
        unsigned char leading = bytes[read];
        unsigned sequence_length = 1;
        unsigned codepoint = leading;
        unsigned char second_minimum = 0x80;
        unsigned char second_maximum = 0xbf;

        if (0xc2 <= leading && leading <= 0xdf) {
            sequence_length = 2;
            codepoint = leading & 0x1f;
        } else if (0xe0 <= leading && leading <= 0xef) {
            sequence_length = 3;
            codepoint = leading & 0x0f;
            if (leading == 0xe0) second_minimum = 0xa0;
        } else if (0xf0 <= leading && leading <= 0xf4) {
            sequence_length = 4;
            codepoint = leading & 0x07;
            if (leading == 0xf0) second_minimum = 0x90;
            if (leading == 0xf4) second_maximum = 0x8f;
        }

        ok = leading < 0x80 || sequence_length > 1;
        ok = ok && read + sequence_length <= source_count;

        for (unsigned i = 1; ok && i < sequence_length; i += 1) {
            unsigned char minimum = i == 1 ? second_minimum : 0x80;
            unsigned char maximum = i == 1 ? second_maximum : 0xbf;
            unsigned char continuation = bytes[read + i];

            ok = minimum <= continuation && continuation <= maximum;
            codepoint = (codepoint << 6) | (continuation & 0x3f);
        }

        unsigned unit_count = codepoint >= 0x10000 ? 2 : 1;
        ok = ok && written + unit_count <= destination_capacity;
        if (ok) {
            if (unit_count == 2) {
                unsigned offset = codepoint - 0x10000;
                destination[written] = (unsigned short)(0xd800 | (offset >> 10));
                destination[written + 1] = (unsigned short)(0xdc00 | (offset & 0x3ff));
            } else destination[written] = (unsigned short)codepoint;

            written += unit_count;
            read += sequence_length;
        }
    }

    *destination_count = written;
    return ok;
}

// Converts a UTF8 path, whether absolute or relative, to an NT path such as \??\C:\folder.
static _Bool fs__nt_path_from_path(fs__Path_Storage *path_storage, const char *path, UNICODE_STRING *result) {
    _Bool ok = 0;
    unsigned short *wide_path = path_storage->paths.wide_path;
    unsigned short *nt_path = path_storage->paths.nt_path;

    unsigned path_size = 0;
    while (path[path_size] != 0) path_size += 1;

    unsigned wide_path_count = 0;
    _Bool converted = fs__utf16_from_utf8(wide_path, FS__NT_PATH_CAPACITY - 1, &wide_path_count, path, path_size);
    wide_path[wide_path_count] = 0;

    unsigned full_path_index = 6;
    unsigned short *full_path = nt_path + full_path_index;
    unsigned full_path_capacity = (FS__NT_PATH_CAPACITY - full_path_index) * 2;
    unsigned full_path_size = converted ? RtlGetFullPathName_U(wide_path, full_path_capacity, full_path, 0) : 0;
    _Bool full_path_fits = full_path_size != 0 && full_path_size < full_path_capacity;

    _Bool fits_unicode_string = 0;
    unsigned short *start = 0;
    unsigned long long nt_path_size = 0;
    if (full_path_fits) {
        _Bool is_device_path = full_path[0] == '\\' && full_path[1] == '\\' && (full_path[2] == '?' || full_path[2] == '.') && full_path[3] == '\\';
        _Bool is_unc_path = !is_device_path && full_path[0] == '\\' && full_path[1] == '\\';

        if (is_device_path) {
            start = full_path;
            start[1] = '?';
            start[2] = '?';
        } else if (is_unc_path) {
            start = nt_path;
            const char unc_prefix[] = "\\??\\UNC";
            for (int i = 0; i < 7; i += 1) start[i] = (unsigned short)unc_prefix[i];
        } else {
            start = nt_path + 2;
            const char prefix[] = "\\??\\";
            for (int i = 0; i < 4; i += 1) start[i] = (unsigned short)prefix[i];
        }

        nt_path_size = (unsigned long long)(full_path - start) * 2 + full_path_size;
        fits_unicode_string = nt_path_size <= 0xfffe;
    }

    if (fits_unicode_string) {
        result->Buffer = start;
        result->Length = (unsigned short)nt_path_size;
        result->MaximumLength = (unsigned short)nt_path_size;
        ok = 1;
    }

    return ok;
}

static OBJECT_ATTRIBUTES fs__object_attributes(UNICODE_STRING *name) {
    enum { FS__OBJ_CASE_INSENSITIVE = 0x40 };
    OBJECT_ATTRIBUTES attributes = {
        .Length = sizeof attributes,
        .ObjectName = name,
        .Attributes = FS__OBJ_CASE_INSENSITIVE,
    };
    return attributes;
}

static void *fs__nt_create_file(fs__Path_Storage *path_storage, const char *path, unsigned desired_access, unsigned share_access, unsigned create_disposition, unsigned create_options) {
    void *handle = 0;
    UNICODE_STRING name = {0};

    if (fs__nt_path_from_path(path_storage, path, &name)) {
        OBJECT_ATTRIBUTES attributes = fs__object_attributes(&name);
        IO_STATUS_BLOCK status_block = {0};
        enum { FS__FILE_ATTRIBUTE_NORMAL = 0x80 };
        int status = NtCreateFile(&handle, desired_access, &attributes, &status_block, 0, FS__FILE_ATTRIBUTE_NORMAL, share_access, create_disposition, create_options | fs__Nt_Create_Option_SYNCHRONOUS_IO_NONALERT, 0, 0);
        if (status < 0) handle = 0;
    }

    return handle;
}

static void *fs__nt_open(fs__Path_Storage *path_storage, const char *path, fs_File_Flags flags) {
    _Bool read = !!(flags & fs_File_Flag_READ);
    _Bool write = !!(flags & fs_File_Flag_WRITE);
    unsigned desired_access = (read ? fs__Nt_Access_FILE_GENERIC_READ : 0) | (write ? fs__Nt_Access_FILE_GENERIC_WRITE : 0);
    unsigned create_disposition = read ? fs__Nt_Disposition_OPEN : fs__Nt_Disposition_OVERWRITE_IF;
    unsigned create_options = write * fs__Nt_Create_Option_NON_DIRECTORY_FILE;
    return fs__nt_create_file(path_storage, path, desired_access, fs__Nt_Share_READ | fs__Nt_Share_DELETE, create_disposition, create_options);
}

static _Bool fs__utf8_from_utf16(char *destination, unsigned long long destination_capacity, unsigned long long *destination_count, const unsigned short *source, unsigned source_count) {
    _Bool ok = 1;
    unsigned char *bytes = (unsigned char *)destination;
    unsigned read = 0;
    unsigned long long written = 0;

    while (ok && read < source_count) {
        unsigned codepoint = source[read];
        unsigned unit_count = 1;
        _Bool is_high_surrogate = 0xd800 <= codepoint && codepoint <= 0xdbff;
        _Bool has_low_surrogate = read + 1 < source_count && 0xdc00 <= source[read + 1] && source[read + 1] <= 0xdfff;
        if (is_high_surrogate && has_low_surrogate) {
            codepoint = 0x10000 + ((codepoint - 0xd800) << 10) + (source[read + 1] - 0xdc00u);
            unit_count = 2;
        }

        unsigned byte_count = codepoint < 0x80 ? 1 : codepoint < 0x800 ? 2 : codepoint < 0x10000 ? 3 : 4;
        ok = written + byte_count <= destination_capacity;
        if (ok) {
            switch (byte_count) {
                case 1: {
                    bytes[written] = (unsigned char)codepoint;
                } break;
                case 2: {
                    bytes[written + 0] = (unsigned char)(0xc0 | (codepoint >> 6));
                    bytes[written + 1] = (unsigned char)(0x80 | (codepoint & 0x3f));
                } break;
                case 3: {
                    bytes[written + 0] = (unsigned char)(0xe0 | (codepoint >> 12));
                    bytes[written + 1] = (unsigned char)(0x80 | ((codepoint >> 6) & 0x3f));
                    bytes[written + 2] = (unsigned char)(0x80 | (codepoint & 0x3f));
                } break;
                default: {
                    bytes[written + 0] = (unsigned char)(0xf0 | (codepoint >> 18));
                    bytes[written + 1] = (unsigned char)(0x80 | ((codepoint >> 12) & 0x3f));
                    bytes[written + 2] = (unsigned char)(0x80 | ((codepoint >> 6) & 0x3f));
                    bytes[written + 3] = (unsigned char)(0x80 | (codepoint & 0x3f));
                } break;
            }
            written += byte_count;
            read += unit_count;
        }
    }

    *destination_count = written;
    return ok;
}

FS_FUNCTION _Bool fs_absolute_path(const char *path, char *buffer, unsigned long long buffer_size) {
    FS_ASSERT(buffer_size > 0);
    _Bool ok = 0;

    #if defined(FS_OS_WINDOWS)
    {
        fs__Path_Storage path_storage = {0};
        void *file = fs__nt_open(&path_storage, path, fs_File_Flag_READ);
        if (file != 0) {
            unsigned name_storage_size = sizeof(fs__Name_Storage);
            IO_STATUS_BLOCK status_block = {0};

            FILE_NAME_INFORMATION *normalised_name = &path_storage.names.normalised_name.file_name;
            int normalised_status = NtQueryInformationFile(file, &status_block, normalised_name, name_storage_size, fs__Nt_Information_Class_FILE_NORMALIZED_NAME);
            if (normalised_status < 0) normalised_status = NtQueryInformationFile(file, &status_block, normalised_name, name_storage_size, fs__Nt_Information_Class_FILE_NAME);

            FILE_NAME_INFORMATION *opened_name = &path_storage.names.object_name.file_name;
            int opened_status = NtQueryInformationFile(file, &status_block, opened_name, name_storage_size, fs__Nt_Information_Class_FILE_NAME);
            unsigned opened_name_size = opened_name->FileNameLength;

            OBJECT_NAME_INFORMATION *object_name = &path_storage.names.object_name.object_name;
            int object_status = opened_status >= 0 ? NtQueryObject(file, (int)fs__Nt_Information_Class_OBJECT_NAME, object_name, name_storage_size, 0) : -1;

            _Bool have_names = normalised_status >= 0 && object_status >= 0 && object_name->Name.Length >= opened_name_size;
            if (have_names) {
                unsigned short *device = object_name->Name.Buffer;
                unsigned device_size = object_name->Name.Length - opened_name_size;

                char prefix[2] = {0};
                unsigned prefix_size = 0;

                const char network_device[] = "\\Device\\Mup";
                _Bool is_network_device = device_size == (sizeof network_device - 1) * 2;
                for (unsigned i = 0; is_network_device && i < sizeof network_device - 1; i += 1) is_network_device = device[i] == (unsigned short)network_device[i];

                if (is_network_device) {
                    prefix[0] = '\\';
                    prefix_size = 1;
                } else for (char letter = 'A'; letter <= 'Z' && prefix_size == 0; letter += 1) {
                    unsigned short link_path[] = { '\\', '?', '?', '\\', (unsigned short)letter, ':' };
                    UNICODE_STRING link_name = { .Length = sizeof link_path, .MaximumLength = sizeof link_path, .Buffer = link_path };
                    OBJECT_ATTRIBUTES link_attributes = fs__object_attributes(&link_name);

                    void *link = 0;
                    if (NtOpenSymbolicLinkObject(&link, fs__Nt_Access_SYMBOLIC_LINK_QUERY, &link_attributes) >= 0) {
                        unsigned short target_buffer[256] = {0};
                        UNICODE_STRING target = { .Length = 0, .MaximumLength = sizeof target_buffer, .Buffer = target_buffer };
                        _Bool is_device_of_letter = NtQuerySymbolicLinkObject(link, &target, 0) >= 0 && target.Length == device_size;
                        for (unsigned i = 0; is_device_of_letter && i < device_size / 2; i += 1) is_device_of_letter = target_buffer[i] == device[i];
                        NtClose(link);

                        if (is_device_of_letter) {
                            prefix[0] = letter;
                            prefix[1] = ':';
                            prefix_size = 2;
                        }
                    }
                }

                _Bool prefix_fits = prefix_size != 0 && prefix_size < buffer_size;
                if (prefix_fits) {
                    for (unsigned i = 0; i < prefix_size; i += 1) buffer[i] = prefix[i];

                    unsigned long long name_capacity = buffer_size - prefix_size - 1;
                    unsigned long long name_size = 0;
                    ok = fs__utf8_from_utf16(buffer + prefix_size, name_capacity, &name_size, normalised_name->FileName, normalised_name->FileNameLength / 2);
                    if (ok) buffer[prefix_size + name_size] = 0;
                }
            }

            NtClose(file);
        }
    }
    #elif defined(FS_OS_POSIX)
    {
        FS_ASSERT(buffer_size >= 4096);
        ok = realpath(path, buffer) != 0;
    }
    #else
        #error "UNSUPPORTED OS"
    #endif

    if (!ok) buffer[0] = 0;
    return ok;
}

FS_FUNCTION _Bool fs_close(fs_File file) {
    _Bool ok = 0;
    if (file.handle == 0) return ok;

    #if defined(FS_OS_WINDOWS)
    {
        ok = NtClose(file.handle) >= 0;
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

FS_FUNCTION _Bool fs_executable_path(char *buffer, unsigned long long buffer_size) {
    FS_ASSERT(buffer_size > 0);
    _Bool ok = 0;

    #if defined(FS_OS_WINDOWS)
    {
        enum { FS__PROCESS_IMAGE_FILE_NAME_WIN32 = 43 };
        void *current_process = (void *)-1;
        fs__Name_Storage image_name_storage = {0};
        unsigned long returned_size = 0;
        int status = NtQueryInformationProcess(current_process, FS__PROCESS_IMAGE_FILE_NAME_WIN32, &image_name_storage, sizeof image_name_storage, &returned_size);

        UNICODE_STRING *image_name = &image_name_storage.object_name.Name;
        unsigned long long path_size = 0;
        ok = status >= 0 && fs__utf8_from_utf16(buffer, buffer_size - 1, &path_size, image_name->Buffer, image_name->Length / 2);
        buffer[ok ? path_size : 0] = 0;
    }
    #else
    {
        buffer[0] = 0;
        FS_ASSERT(0 && "unimplemented for this OS");
    }
    #endif

    return ok;
}

FS_FUNCTION _Bool fs_exists(const char *path) {
    _Bool exists = 0;

    #if defined(FS_OS_WINDOWS)
    {
        fs__Path_Storage path_storage = {0};
        UNICODE_STRING name = {0};
        if (fs__nt_path_from_path(&path_storage, path, &name)) {
            OBJECT_ATTRIBUTES attributes = fs__object_attributes(&name);
            FILE_BASIC_INFORMATION information = {0};
            exists = NtQueryAttributesFile(&attributes, &information) >= 0;
        }
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

FS_FUNCTION _Bool fs_file_info(const char *path, fs_File_Info *info) {
    _Bool ok = 0;
    *info = (fs_File_Info){ .path = path, .name = path };
    for (const char *c = path; *c != 0; c += 1) {
        _Bool is_separator = *c == '/' || *c == '\\';
        if (is_separator) info->name = c + 1;
    }

    #if defined(FS_OS_WINDOWS)
    {
        fs__Path_Storage path_storage = {0};
        UNICODE_STRING name = {0};
        if (fs__nt_path_from_path(&path_storage, path, &name)) {
            OBJECT_ATTRIBUTES attributes = fs__object_attributes(&name);
            FILE_NETWORK_OPEN_INFORMATION information = {0};
            ok = NtQueryFullAttributesFile(&attributes, &information) >= 0;
            if (ok) {
                enum { FS__FILE_ATTRIBUTE_DIRECTORY = 0x10 };
                info->size = (unsigned long long)information.EndOfFile;
                info->modification_time = (unsigned long long)information.LastWriteTime;
                info->is_folder = !!(information.FileAttributes & FS__FILE_ATTRIBUTE_DIRECTORY);
            }
        }
    }
    #else
    {
        FS_ASSERT(0 && "unimplemented for this OS");
    }
    #endif

    return ok;
}

FS_FUNCTION fs_File fs_file_open_and_get_size(const char *path, fs_File_Flags flags, unsigned long long *size) {
    fs_File file = fs_open(path, flags);
    *size = 0;
    if (file.handle != 0) {
        #if defined(FS_OS_WINDOWS)
        {
            IO_STATUS_BLOCK status_block = {0};
            FILE_STANDARD_INFORMATION information = {0};
            int status = NtQueryInformationFile(file.handle, &status_block, &information, sizeof information, fs__Nt_Information_Class_FILE_STANDARD);
            if (status >= 0) *size = (unsigned long long)information.EndOfFile;
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

FS_FUNCTION unsigned long long fs_file_size(const char *path) {
    unsigned long long result = 0;

    #if defined(FS_OS_WINDOWS)
    {
        fs__Path_Storage path_storage = {0};
        UNICODE_STRING name = {0};
        if (fs__nt_path_from_path(&path_storage, path, &name)) {
            OBJECT_ATTRIBUTES attributes = fs__object_attributes(&name);
            FILE_NETWORK_OPEN_INFORMATION information = {0};
            if (NtQueryFullAttributesFile(&attributes, &information) >= 0) result = (unsigned long long)information.EndOfFile;
        }
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

FS_FUNCTION _Bool fs_file_write(fs_File file, const void *bytes, unsigned long long size) {
    _Bool ok = 0;
    if (file.handle == 0) return ok;

    #if defined(FS_OS_WINDOWS)
    {
        ok = 1;
        for (unsigned long long total_written = 0; ok && total_written < size;) {
            unsigned long long remaining = size - total_written;
            unsigned count_to_write = remaining > 0xffffffff ? 0xffffffff : (unsigned)remaining;
            IO_STATUS_BLOCK status_block = {0};
            int status = NtWriteFile(file.handle, 0, 0, 0, &status_block, (const char *)bytes + total_written, count_to_write, 0, 0);
            ok = status >= 0 && status_block.Information != 0;
            total_written += status_block.Information;
        }
    }
    #elif defined(FS_OS_POSIX)
    {
        enum { FS__SEEK_CUR = 1 };
        ok = fseek(file.handle, 0, FS__SEEK_CUR) == 0;
        for (unsigned long long total_written = 0; ok && total_written < size;) {
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

#if defined(FS_OS_WINDOWS)
    static void *fs__nt_open_folder(const char *path) {
        fs__Path_Storage path_storage = {0};
        unsigned share_access = fs__Nt_Share_READ | fs__Nt_Share_WRITE | fs__Nt_Share_DELETE;
        void *handle = fs__nt_create_file(&path_storage, path, fs__Nt_Access_FILE_LIST_DIRECTORY | fs__Nt_Access_SYNCHRONIZE, share_access, fs__Nt_Disposition_OPEN, fs__Nt_Create_Option_DIRECTORY_FILE);
        return handle;
    }
#endif

FS_FUNCTION _Bool fs_folder_iterate(fs_Folder_Iterator *it) {
    _Bool found = 0;

    #if defined(FS_OS_WINDOWS)
    {
        enum { FS__FILE_ATTRIBUTE_DIRECTORY = 0x10, FS__FILE_ATTRIBUTE_REPARSE_POINT = 0x400 };
        unsigned char return_single_entry = 1;

        while (!found && it->open_folder_count > 0) {
            unsigned level = it->open_folder_count - 1;
            IO_STATUS_BLOCK status_block = {0};
            int status = NtQueryDirectoryFile(it->handles[level], 0, 0, 0, &status_block, it->entry, sizeof it->entry, fs__Nt_Information_Class_FILE_DIRECTORY, return_single_entry, 0, 0);
            _Bool folder_has_entry = status == 0;

            if (!folder_has_entry) {
                NtClose(it->handles[level]);
                it->handles[level] = 0;
                it->open_folder_count -= 1;
            } else {
                FILE_DIRECTORY_INFORMATION *entry = (FILE_DIRECTORY_INFORMATION *)it->entry;
                unsigned long long name_size = 0;
                _Bool name_converted = fs__utf8_from_utf16(it->name, sizeof it->name - 1, &name_size, entry->FileName, entry->FileNameLength / 2);
                if (!name_converted) name_size = 0;
                it->name[name_size] = 0;

                _Bool is_self_or_parent = it->name[0] == '.' && (name_size == 1 || (name_size == 2 && it->name[1] == '.'));
                unsigned folder_path_length = it->path_lengths[level];
                _Bool path_fits = folder_path_length + 1 + name_size < sizeof it->path;
                _Bool entry_usable = name_converted && path_fits;
                if (!entry_usable) it->had_error = 1;

                if (entry_usable && !is_self_or_parent) {
                    it->path[folder_path_length] = FS_PATH_SEPARATOR;
                    for (unsigned long long i = 0; i < name_size; i += 1) it->path[folder_path_length + 1 + i] = it->name[i];
                    it->path[folder_path_length + 1 + name_size] = 0;

                    _Bool is_folder = !!(entry->FileAttributes & FS__FILE_ATTRIBUTE_DIRECTORY);
                    _Bool is_reparse_point = !!(entry->FileAttributes & FS__FILE_ATTRIBUTE_REPARSE_POINT);
                    it->info = (fs_File_Info){
                        .path = it->path,
                        .name = it->name,
                        .size = (unsigned long long)entry->EndOfFile,
                        .modification_time = (unsigned long long)entry->LastWriteTime,
                        .depth = level,
                        .is_folder = is_folder,
                    };
                    found = 1;

                    _Bool should_enter_folder = (it->flags & fs_Folder_Iterator_Flag_RECURSIVE) && is_folder && !is_reparse_point;
                    if (should_enter_folder) {
                        _Bool depth_fits = it->open_folder_count < FS_FOLDER_ITERATOR_MAX_DEPTH;
                        void *child = depth_fits ? fs__nt_open_folder(it->path) : 0;
                        if (child != 0) {
                            it->handles[it->open_folder_count] = child;
                            it->path_lengths[it->open_folder_count] = folder_path_length + 1 + (unsigned)name_size;
                            it->open_folder_count += 1;
                        } else it->had_error = 1;
                    }
                }
            }
        }
    }
    #else
    {
        (void)it;
        FS_ASSERT(0 && "unimplemented for this OS");
    }
    #endif

    return found;
}

FS_FUNCTION fs_Folder_Iterator fs_folder_iterator(const char *relative_path, fs_Folder_Iterator_Flags flags) {
    fs_Folder_Iterator it = { .flags = flags };

    #if defined(FS_OS_WINDOWS)
    {
        unsigned path_length = 0;
        while (relative_path[path_length] != 0) path_length += 1;

        _Bool path_fits = path_length < sizeof it.path;
        void *handle = path_fits ? fs__nt_open_folder(relative_path) : 0;
        if (handle != 0) {
            for (unsigned i = 0; i <= path_length; i += 1) it.path[i] = relative_path[i];
            it.handles[0] = handle;
            it.path_lengths[0] = path_length;
            it.open_folder_count = 1;
        } else it.had_error = 1;
    }
    #else
    {
        (void)relative_path;
        FS_ASSERT(0 && "unimplemented for this OS");
    }
    #endif

    return it;
}

FS_FUNCTION void fs_folder_iterator_end(fs_Folder_Iterator *it) {
    #if defined(FS_OS_WINDOWS)
    {
        for (unsigned level = 0; level < it->open_folder_count; level += 1) NtClose(it->handles[level]);
        it->open_folder_count = 0;
    }
    #else
    {
        (void)it;
        FS_ASSERT(0 && "unimplemented for this OS");
    }
    #endif
}

FS_FUNCTION _Bool fs_make_folder(const char *relative_path) {
    _Bool ok = 0;

    #if defined(FS_OS_WINDOWS)
    {
        fs__Path_Storage path_storage = {0};
        unsigned share_access = fs__Nt_Share_READ | fs__Nt_Share_WRITE;
        void *handle = fs__nt_create_file(&path_storage, relative_path, fs__Nt_Access_FILE_LIST_DIRECTORY | fs__Nt_Access_SYNCHRONIZE, share_access, fs__Nt_Disposition_CREATE, fs__Nt_Create_Option_DIRECTORY_FILE);
        ok = handle != 0;
        if (ok) NtClose(handle);
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

FS_FUNCTION fs_File fs_open(const char *path, fs_File_Flags flags) {
    FS_ASSERT(flags != 0);
    fs_File result = {0};

    _Bool read = !!(flags & fs_File_Flag_READ);
    _Bool write = !!(flags & fs_File_Flag_WRITE);
    FS_ASSERT(read || write);

    #if defined(FS_OS_WINDOWS)
    {
        fs__Path_Storage path_storage = {0};
        result.handle = fs__nt_open(&path_storage, path, flags);
    }
    #elif defined(FS_OS_POSIX)
    {
        const char *mode = 0;
        if (read) mode = "rb";
        if (write) mode = "wb";
        if (read && write) mode = "r+b";
        FILE *handle = fopen(path, mode);
        result.handle = handle;
    }
    #else
        #error "UNSUPPORTED OS"
    #endif

    return result;
}

FS_FUNCTION unsigned long long fs_read_entire_file(fs_File file, char *buffer, unsigned long long size) {
    unsigned long long read = 0;
    if (file.handle == 0) return read;
    FS_ASSERT(buffer != 0);

    #if defined(FS_OS_WINDOWS)
    {
        _Bool ok = 1;
        while (ok && read < size) {
            unsigned long long remaining = size - read;
            unsigned count_to_read = remaining > 0xffffffff ? 0xffffffff : (unsigned)remaining;
            long long byte_offset = (long long)read;
            IO_STATUS_BLOCK status_block = {0};
            int status = NtReadFile(file.handle, 0, 0, 0, &status_block, buffer + read, count_to_read, &byte_offset, 0);
            _Bool reached_end_of_file = status_block.Information == 0;
            ok = status >= 0 && !reached_end_of_file;
            read += status_block.Information;
        }

        if (!ok) read = 0;
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

FS_FUNCTION _Bool fs_remove_file(const char *relative_path) {
    _Bool ok = 0;

    #if defined(FS_OS_WINDOWS)
    {
        fs__Path_Storage path_storage = {0};
        unsigned share_access = fs__Nt_Share_READ | fs__Nt_Share_WRITE | fs__Nt_Share_DELETE;
        unsigned create_options = fs__Nt_Create_Option_NON_DIRECTORY_FILE | fs__Nt_Create_Option_DELETE_ON_CLOSE;
        void *handle = fs__nt_create_file(&path_storage, relative_path, fs__Nt_Access_DELETE | fs__Nt_Access_SYNCHRONIZE, share_access, fs__Nt_Disposition_OPEN, create_options);
        ok = handle != 0 && NtClose(handle) >= 0;
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

FS_FUNCTION _Bool fs_rename(const char *old_path, const char *new_path) {
    _Bool ok = 0;

    #if defined(FS_OS_WINDOWS)
    {
        fs__Path_Storage path_storage = {0};
        unsigned share_access = fs__Nt_Share_READ | fs__Nt_Share_WRITE | fs__Nt_Share_DELETE;
        void *handle = fs__nt_create_file(&path_storage, old_path, fs__Nt_Access_DELETE | fs__Nt_Access_SYNCHRONIZE, share_access, fs__Nt_Disposition_OPEN, 0);

        UNICODE_STRING new_name = {0};
        _Bool new_name_ok = handle != 0 && fs__nt_path_from_path(&path_storage, new_path, &new_name);
        if (new_name_ok) {
            typedef struct {
                unsigned char ReplaceIfExists;
                void *RootDirectory;
                unsigned long FileNameLength;
                unsigned short FileName[FS__NT_PATH_CAPACITY];
            } FILE_RENAME_INFORMATION;
            FILE_RENAME_INFORMATION rename_information = { .ReplaceIfExists = 1, .FileNameLength = new_name.Length };
            for (unsigned i = 0; i < new_name.Length / 2u; i += 1) rename_information.FileName[i] = new_name.Buffer[i];

            IO_STATUS_BLOCK status_block = {0};
            ok = NtSetInformationFile(handle, &status_block, &rename_information, sizeof rename_information, fs__Nt_Information_Class_FILE_RENAME) >= 0;
        }

        if (handle != 0) NtClose(handle);
    }
    #else
    {
        (void)old_path;
        (void)new_path;
        FS_ASSERT(0 && "unimplemented for this OS");
    }
    #endif

    return ok;
}

#endif // FS_IMPLEMENTATION
