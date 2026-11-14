#if !defined(BASE_H)
#define BASE_H


#include "base/base_context.h"

#define META(...)

#if COMPILER_CLANG || COMPILER_GCC
    typedef __builtin_va_list va_list;
    #define va_start(arguments, last_parameter)  __builtin_va_start(arguments, last_parameter)
    #define va_arg(arguments, T)                 __builtin_va_arg(arguments, T)
    #define va_end(arguments)                    __builtin_va_end(arguments)
    #define va_copy(destination, source)         __builtin_va_copy(destination, source)
#elif COMPILER_MSVC && BASE_ARCH == BASE_ARCH_X86_64
    typedef char *va_list;
    void __va_start(va_list *arguments, ...);
    #define va_start(arguments, last_parameter) ((void)__va_start(&(arguments), last_parameter))
    #define va_arg(arguments, T) \
        ((sizeof(T) > 8 || (sizeof(T) & (sizeof(T) - 1)) != 0) \
            ? **(T **)(((arguments) += 8) - 8) \
            :  *(T * )(((arguments) += 8) - 8))
    #define va_end(arguments) ((void)((arguments) = (va_list)0))
    #define va_copy(destination, source) ((destination) = (source))
#else
    #error "UNIMPLEMENTED"
#endif

#if !defined(__bool_true_false_are_defined)
    #if defined(bool)
        #undef bool
    #endif
    #if BASE_C_VERSION < 2023
        typedef _Bool bool;
    #endif
    #if !defined(true)
        #define true 1
    #endif
    #if !defined(false)
        #define false 0
    #endif
    #define __bool_true_false_are_defined 1
#endif

#define BASE__GLUE(a, b) a##b
#define BASE__GLUE_GLUE(a, b) BASE__GLUE(a, b)

#if BASE_C_VERSION < 2011
    #define _Static_assert(expression, message) typedef char BASE__GLUE_GLUE(BASE__STATIC_ASSERT_LINE_, __LINE__)[(expression) ? 1 : -1]
#endif

typedef unsigned char       u8;
typedef unsigned short     u16;
typedef unsigned           u32;
typedef unsigned long long u64;
typedef signed char         i8;
typedef short              i16;
typedef int                i32;
typedef long long          i64;
typedef float              f32;
typedef double             f64;
_Static_assert(sizeof( u8) == 1, "");
_Static_assert(sizeof(u16) == 2, "");
_Static_assert(sizeof(u32) == 4, "");
_Static_assert(sizeof(u64) == 8, "");
_Static_assert(sizeof( i8) == 1, "");
_Static_assert(sizeof(i16) == 2, "");
_Static_assert(sizeof(i32) == 4, "");
_Static_assert(sizeof(i64) == 8, "");
_Static_assert(sizeof(f32) == 4, "");
_Static_assert(sizeof(f64) == 8, "");
#define UINT32_MAX ((u32)-1)
#define UINT64_MAX ((u64)-1)
#define INT64_MIN ((i64)0x8000000000000000)
#define INT64_MAX ((i64)0x7fffffffffffffff)
#define INT32_MAX ((i32)0x7fffffff)


#if !defined(LINK_CRT)
    #define LINK_CRT 0
#endif

#if BASE_OS == BASE_OS_LINUX
    #error "TODO(felix)"
#elif BASE_OS & BASE_OS_ANY_APPLE
    #define _WSTATUS(x)    ((x) & 0177)
    #define	_WSTOPPED      0177
    #define WEXITSTATUS(x) (int)(((unsigned)(x) >> 8) & 0xff)
    #define WIFEXITED(x)   (_WSTATUS(x) == 0)
    #define WIFSIGNALED(x) (_WSTATUS(x) != _WSTOPPED && _WSTATUS(x) != 0)
    #define WTERMSIG(x)	   (_WSTATUS(x))

    extern int errno;

    typedef unsigned long uintptr_t;

    typedef i64 __darwin_blkcnt_t;
    typedef i32 __darwin_blksize_t;
    typedef i32 __darwin_dev_t;
    typedef u32 __darwin_gid_t;
    typedef u64 __darwin_ino_t; // assumes __DARWIN_64_BIT_INO_T
    typedef u16 __darwin_mode_t;
    typedef i64 __darwin_off_t;
    typedef i32 __darwin_pid_t;
    #define __PTHREAD_ATTR_SIZE__ 56 // assumes __LP64__, meaning we're building for 64-bit
    typedef struct {
        long sig;
        char opaque[__PTHREAD_ATTR_SIZE__];
    } __darwin_pthread_attr_t;
    typedef struct __darwin_pthread_handler_rec {
        void (*routine)(void *);
        void *arg;
        struct __darwin_pthread_handler_rec *next;
    } __darwin_pthread_handler_rec;
    #define __PTHREAD_SIZE__ 8176 // assumes __LP64__, meaning we're building for 64-bit
    typedef struct {
        long sig;
        struct __darwin_pthread_handler_rec *cleanup_stack;
        char opaque[__PTHREAD_SIZE__];
    } __darwin_pthread_t;
    typedef unsigned long __darwin_size_t; _Static_assert(sizeof(__darwin_size_t) == 8, "assumes 64-bit");
    typedef long __darwin_ssize_t; _Static_assert(sizeof(__darwin_ssize_t) == 8, "assumes 64-bit");
    typedef u32 __darwin_uid_t;
    typedef long __darwin_time_t;

    typedef void                    FILE;
    typedef __darwin_blkcnt_t       blkcnt_t;
    typedef __darwin_blksize_t      blksize_t;
    typedef int                     clockid_t; // system enum
    typedef __darwin_dev_t          dev_t;
    typedef __darwin_gid_t          gid_t;
    typedef __darwin_ino_t          ino_t;
    typedef __darwin_mode_t         mode_t;
    typedef u16                     nlink_t;
    typedef __darwin_off_t          off_t;
    typedef __darwin_pid_t          pid_t;
    typedef __darwin_pthread_attr_t pthread_attr_t;
    typedef __darwin_pthread_t      pthread_t;
    typedef __darwin_size_t         size_t;
    typedef __darwin_ssize_t        ssize_t;
    typedef __darwin_uid_t          uid_t;
    struct timespec {
        __darwin_time_t tv_sec;
        long tv_nsec;
    };
    struct stat { // assume _DARWIN_FEATURE_64_BIT_INODE
        dev_t           st_dev;           /* ID of device containing file */
        mode_t          st_mode;          /* Mode of file (see below) */
        nlink_t         st_nlink;         /* Number of hard links */
        ino_t           st_ino;           /* File serial number */
        uid_t           st_uid;           /* User ID of the file */
        gid_t           st_gid;           /* Group ID of the file */
        dev_t           st_rdev;          /* Device ID */
        struct timespec st_atimespec;     /* time of last access */
        struct timespec st_mtimespec;     /* time of last data modification */
        struct timespec st_ctimespec;     /* time of last status change */
        struct timespec st_birthtimespec; /* time of file creation(birth) */
        off_t           st_size;          /* file size, in bytes */
        blkcnt_t        st_blocks;        /* blocks allocated for file */
        blksize_t       st_blksize;       /* optimal blocksize for I/O */
        u32             st_flags;         /* user defined flags for file */
        u32             st_gen;           /* file generation number */
        i32             st_lspare;        /* RESERVED: DO NOT USE! */
        i64             st_qspare[2];     /* RESERVED: DO NOT USE! */
    };

    void    abort(void);
    int     chdir(const char *path);
    int     clock_gettime(clockid_t, struct timespec *);
    int     execvp(const char *file, char *const argv[]);
    void    exit(int);
    int     fclose(FILE *file);
    FILE   *fopen(const char *path, const char *mode);
    pid_t   fork(void);
    size_t  fread(void *pointer, size_t size, size_t number_of_items, FILE *file);
    int     fseek(FILE *file, long offset, int whence);
    size_t  fwrite(const void *pointer, size_t size, size_t number_of_items, FILE *file);
    int     lstat(const char *path, struct stat *buffer);
    int     mkdir(const char *path, mode_t mode);
    int     pthread_create(pthread_t *, const pthread_attr_t *, void *(*start_routine)(void *), void *argument);
    char   *realpath(const char *file_name, char *resolved_name); //  __DARWIN_EXTSN(realpath); // NOTE(felix): `__DARWIN_EXTSN` needed to silence "error: cannot apply asm label to function after its first use"
    int     close(int descriptor);
    int     dup2(int descriptor, int new_descriptor);
    int     pipe(int descriptors[2]);
    ssize_t read(int descriptor, void *buffer, size_t number_of_bytes);
    int     remove(const char *path);
    int     stat(const char *path, struct stat *buf);
    void    rewind(FILE *file);
    pid_t   waitpid(pid_t, int *status, int options);
    ssize_t write(int descriptor, const void *buffer, size_t number_of_bytes);

    void *memcpy(void *destination, const void *source, size_t number_of_bytes);
    void *memset(void *destination, int byte, size_t length);
    int   memcmp(const void *s1, const void *s2, size_t number_of_bytes);

    #define os_abort() abort()
    #define os_exit(code) exit(code)
#elif BASE_OS == BASE_OS_WINDOWS
    #if BASE_ARCH_BITS == 64
        typedef u64 size_t;
    #else
        #error "UNIMPLEMENTED"
    #endif

    #define os_abort() NtTerminateProcess((void *)-1, 1)
    #define os_exit(code) RtlExitUserProcess((int)(code))

    META(link "Kernel32")

    #if LINK_CRT
        void *memcpy(void *destination, const void *source, size_t number_of_bytes);
        void *memset(void *destination, int byte, size_t length);
        int   memcmp(const void *s1, const void *s2, size_t number_of_bytes);
    #endif

    struct _SECURITY_ATTRIBUTES { // https://learn.microsoft.com/en-us/windows/win32/api/wtypesbase/ns-wtypesbase-security_attributes
        unsigned long nLength;
        void *lpSecurityDescriptor;
        int bInheritHandle;
    };
    typedef struct { // https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/ns-processthreadsapi-startupinfow
        unsigned long cb;
        unsigned short *lpReserved;
        unsigned short *lpDesktop;
        unsigned short *lpTitle;
        unsigned long dwX;
        unsigned long dwY;
        unsigned long dwXSize;
        unsigned long dwYSize;
        unsigned long dwXCountChars;
        unsigned long dwYCountChars;
        unsigned long dwFillAttribute;
        unsigned long dwFlags;
        unsigned short wShowWindow;
        unsigned short cbReserved2;
        unsigned char *lpReserved2;
        void *hStdInput;
        void *hStdOutput;
        void *hStdError;
    } STARTUPINFOW;
    typedef struct { // https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/ns-processthreadsapi-process_information
        void *hProcess;
        void *hThread;
        unsigned long dwProcessId;
        unsigned long dwThreadId;
    } PROCESS_INFORMATION;
    int  CreatePipe(void **read_pipe, void **write_pipe, struct _SECURITY_ATTRIBUTES *pipe_attributes, unsigned long size);
    int  CreateProcessW(const unsigned short *application_name, unsigned short *command_line, struct _SECURITY_ATTRIBUTES *process_attributes, struct _SECURITY_ATTRIBUTES *thread_attributes, int should_inherit_handles, unsigned long creation_flags, void *environment, const unsigned short *current_directory, STARTUPINFOW *, PROCESS_INFORMATION *);
    int  SetHandleInformation(void *object, unsigned long mask, unsigned long flags);
    void OutputDebugStringW(const unsigned short *output_string);
    int  WriteConsoleW(void *console_output, const void *buffer, unsigned long number_of_characters_to_write, unsigned long *number_of_characters_written, void *reserved);

    META(link "User32")
    int MessageBoxW(void *window, const unsigned short *text, const unsigned short *caption, unsigned type);

    META(link "Ntdll")
    typedef struct {
        unsigned short Length;
        unsigned short MaximumLength;
        unsigned short *Buffer;
    } UNICODE_STRING;
    typedef struct {
        unsigned long Length;
        void *RootDirectory;
        UNICODE_STRING *ObjectName;
        unsigned long Attributes;
        void *SecurityDescriptor;
        void *SecurityQualityOfService;
    } OBJECT_ATTRIBUTES;
    typedef struct {
        union {
            long Status;
            void *Pointer;
        };
        unsigned long long Information;
    } IO_STATUS_BLOCK;
    long NtClose(void *Handle);
    long NtCreateFile(void **FileHandle, unsigned long DesiredAccess, OBJECT_ATTRIBUTES *ObjectAttributes, IO_STATUS_BLOCK *IoStatusBlock, long long *AllocationSize, unsigned long FileAttributes, unsigned long ShareAccess, unsigned long CreateDisposition, unsigned long CreateOptions, void *EaBuffer, unsigned long EaLength);
    long NtQueryObject(void *Handle, int ObjectInformationClass, void *ObjectInformation, unsigned long ObjectInformationLength, unsigned long *ReturnLength);

    typedef struct {
        UNICODE_STRING DosPath;
        void *Handle;
    } CURDIR;
    typedef struct {
        unsigned long  MaximumLength;
        unsigned long  Length;
        unsigned long  Flags;
        unsigned long  DebugFlags;
        void          *ConsoleHandle;
        unsigned long  ConsoleFlags;
        void          *StandardInput;
        void          *StandardOutput;
        void          *StandardError;
        CURDIR         CurrentDirectory;
        UNICODE_STRING DllPath;
        UNICODE_STRING ImagePathName;
        UNICODE_STRING CommandLine;
    } RTL_USER_PROCESS_PARAMETERS;
    typedef struct {
        unsigned char  InheritedAddressSpace;
        unsigned char  ReadImageFileExecOptions;
        unsigned char  BeingDebugged;
        unsigned char  BitField;
        void          *Mutant;
        void          *ImageBaseAddress;
        void          *Ldr;
        RTL_USER_PROCESS_PARAMETERS *ProcessParameters;
    } PEB;
    typedef struct {
        long               ExitStatus;
        PEB               *PebBaseAddress;
        unsigned long long AffinityMask;
        long               BasePriority;
        unsigned long long UniqueProcessId;
        unsigned long long InheritedFromUniqueProcessId;
    } PROCESS_BASIC_INFORMATION;
    typedef struct {
        unsigned long DeviceType;
        unsigned long Characteristics;
    } FILE_FS_DEVICE_INFORMATION;
    long          NtQueryInformationProcess(void *ProcessHandle, int ProcessInformationClass, void *ProcessInformation, unsigned long ProcessInformationLength, unsigned long *ReturnLength);
    long          NtQueryVolumeInformationFile(void *FileHandle, IO_STATUS_BLOCK *IoStatusBlock, void *FsInformation, unsigned long Length, int FsInformationClass);
    long          NtReadFile(void *FileHandle, void *Event, void *ApcRoutine, void *ApcContext, IO_STATUS_BLOCK *, void *Buffer, unsigned long Length, long long *ByteOffset, unsigned long *Key);
    long          NtTerminateProcess(void *ProcessHandle, long ExitStatus);
    long          NtWaitForSingleObject(void *Handle, unsigned char Alertable, long long *Timeout);
    long          NtWriteFile(void *FileHandle, void *Event, void *ApcRoutine, void *ApcContext, IO_STATUS_BLOCK *IoStatusBlock, const void *Buffer, unsigned long Length, long long *ByteOffset, unsigned long *Key);
    void          RtlExitUserProcess(long ExitStatus);
    PEB          *RtlGetCurrentPeb(void);
    unsigned long RtlGetLastWin32Error(void);
#elif BASE_OS == BASE_OS_WASM
    typedef __SIZE_TYPE__ size_t; _Static_assert(sizeof(size_t) == 4, "Assumes wasm32");
    #define os_abort() // TODO(felix)
    #define os_exit(code) // TODO(felix)
#endif // BASE_OS

#if COMPILER_CLANG || COMPILER_GCC // they share some builtins
    #define builtin_unreachable __builtin_unreachable()
#endif

#if COMPILER_CLANG
    #define builtin_assume(expr) __builtin_assume(expr)
    #define breakpoint __builtin_debugtrap()

#elif COMPILER_GCC
    #define builtin_assume(expr) { if (!(expr)) unreachable; }
    #define breakpoint __builtin_trap()

#elif COMPILER_MSVC
    #define builtin_assume(expr) __assume(expr)
    #define breakpoint __debugbreak()
    #define builtin_unreachable assert(false)

#elif COMPILER_STANDARD
    // NOTE(felix): Not tested at all.
    #include <assert.h>
    #define builtin_assume(expr) assert(expr)
    #define breakpoint builtin_assume(false)
    #define builtin_unreachable assert(false)

#endif // COMPILER

#if defined(unreachable)
    #undef unreachable
#endif

#if BUILD_DEBUG
    #define assert(expr) do { if((expr) == 0) panic("failed assertion `%s`", #expr); } while (0)
    #define ensure(expression) assert(expression)
    #define unreachable panic("reached unreachable code")
#else
    #define assert(expr) builtin_assume(expr)
    #define ensure(expression) { if (!(expression)) panic("fatal condition"); }
    #define unreachable builtin_unreachable
#endif // BUILD_DEBUG

#define STR_FUNCTION static
#include "base/str.h"
#define S STRING_LITERAL
#define Sc STRING_LITERAL_CONSTANT

#define SMALL_SNPRINTF_FUNCTION static
#define SMALL_SNPRINTF_NO_STD_INCLUDE
#include "base/small_snprintf.h"

#define UTF_FUNCTION static
#define UTF_NO_SYSTEM_INCLUDE
#include "base/utf.h"

#define TIME_FUNCTION static
#define TIME_NO_SYSTEM_INCLUDE
#include "base/time.h"

#define PRNG_FUNCTION static
#include "base/prng.h"

#define ARENA_FUNCTION static
#define ARENA_NO_SYSTEM_INCLUDE
#include "base/arena.h"
typedef arena_Scratch Scratch;

#define DS_FUNCTION static
#define ds_u32 u32
#define ds_u64 u64
#define ds_size_t size_t
#include "base/data_structures.h"

#define log_error(...) log_internal("error: " __VA_ARGS__)
#define log_info(...) log_internal("info: " __VA_ARGS__)
#define log_internal(...) log_internal_with_location(__FILE__, __LINE__, __func__, __VA_ARGS__)
#define log_internal_v(format, arguments) log_internal_with_location_v(__FILE__, __LINE__, __func__, format, arguments)
static void log_internal_with_location(const char *file, u64 line, const char *func, const char *format, ...);
static void log_internal_with_location_v(const char *file, u64 line, const char *func, const char *format, va_list arguments);
static void os_show_fatal_error(const char *file, u64 line, const char *function, const char *format, ...);

#define panic(...) panic_(__FILE__, __LINE__, __func__, __VA_ARGS__)
#define panic_(file, line, function, ...) do { \
    log_internal_with_location((file), (line), (function), "panic: " __VA_ARGS__); \
    os_show_fatal_error((file), (line), (function), "panic: " __VA_ARGS__); \
    breakpoint; os_abort(); \
} while (0)

static inline size_t size_t_from_u64(u64 value) {
    assert(value <= (size_t)-1);
    return (size_t)value;
}

#define arena_array_reserve(arena, data, count, capacity, minimum) arena_array_reserve_((arena), (data), sizeof *(data), (count), (capacity), (minimum))
static void *arena_array_reserve_(Arena *arena, void *data, size_t item_size, u64 count, u64 *capacity, u64 minimum);

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP_LOW MAX
#define CLAMP_HIGH MIN
#define CLAMP(value, low, high) CLAMP_HIGH(CLAMP_LOW((value), (low)), (high))

static inline int memcmp_(const void *a_, const void *b_, u64 byte_count) {
    const u8 *a = a_, *b = b_;
    for (u64 i = 0; i < byte_count; i += 1) {
        if (a[i] != b[i]) return a[i] - b[i];
    }
    return 0;
}

#if !LINK_CRT && BASE_OS != BASE_OS_WASM
    #pragma function(memcmp)
    #pragma function(memcpy)
    #pragma function(memset)

    int memcmp(const void *a, const void *b, u64 byte_count) {
        return memcmp_(a, b, byte_count);
    }

    // NOTE(felix): Optimising compilers turn a copy or fill loop into a call to memcpy or memset, so this loop turns into stack-overflowing recursion.
    //              MSVC does this to memcpy and clang to memset. The volatile pointer stops the loop from being recognised for the non-intrinsic fallback case.
    // TODO(felix): Use metaprogram inline assembly once it exists.
    #if COMPILER_MSVC && BASE_ARCH == BASE_ARCH_X86_64
        void __movsb(unsigned char *destination, const unsigned char *source, size_t byte_count);
        void __stosb(unsigned char *destination, unsigned char byte, size_t byte_count);
        #pragma intrinsic(__movsb, __stosb)
    #endif

    void *memcpy(void *destination, const void *source, u64 byte_count) {
        assert(byte_count == 0 || (destination != 0 && source != 0));

        #if COMPILER_MSVC && BASE_ARCH == BASE_ARCH_X86_64
            __movsb(destination, source, size_t_from_u64(byte_count));
        #elif (COMPILER_CLANG || COMPILER_GCC) && BASE_ARCH == BASE_ARCH_X86_64
            void *destination_cursor = destination;
            __asm__ volatile("rep movsb" : "+D"(destination_cursor), "+S"(source), "+c"(byte_count) : : "memory");
        #else
            volatile u8 *destination_bytes = destination;
            const u8 *source_bytes = source;
            for (u64 i = 0; i < byte_count; i += 1) destination_bytes[i] = source_bytes[i];
        #endif

        return destination;
    }

    void *memset(void *destination, int byte, u64 byte_count) {
        assert(byte_count == 0 || destination != 0);

        #if COMPILER_MSVC && BASE_ARCH == BASE_ARCH_X86_64
            __stosb(destination, (u8)byte, size_t_from_u64(byte_count));
        #elif (COMPILER_CLANG || COMPILER_GCC) && BASE_ARCH == BASE_ARCH_X86_64
            void *destination_cursor = destination;
            __asm__ volatile("rep stosb" : "+D"(destination_cursor), "+c"(byte_count) : "a"(byte) : "memory");
        #else
            volatile u8 *destination_bytes = destination;
            for (u64 i = 0; i < byte_count; i += 1) destination_bytes[i] = (u8)byte;
        #endif

        return destination;
    }
#endif

#if BASE_OS == BASE_OS_WASM
    // NOTE(felix): We build without compiler-rt, which defines the 128-bit multiply that ubsan needs for wasm32 signed 64-bit multiply overflow checks.
    __extension__ typedef __int128 i128;
    __extension__ typedef unsigned __int128 u128;

    i128 __multi3(i128 a, i128 b) {
        u64 a_low = (u64)a;
        u64 a_high = (u64)((u128)a >> 64);
        u64 b_low = (u64)b;
        u64 b_high = (u64)((u128)b >> 64);

        u64 half_mask = 0xffffffff;
        u64 low_times_low = (a_low & half_mask) * (b_low & half_mask);
        u64 high_times_low = (a_low >> 32) * (b_low & half_mask);
        u64 low_times_high = (a_low & half_mask) * (b_low >> 32);
        u64 high_times_high = (a_low >> 32) * (b_low >> 32);

        u64 middle = (low_times_low >> 32) + (high_times_low & half_mask) + (low_times_high & half_mask);
        u64 result_low = (low_times_low & half_mask) | (middle << 32);
        u64 result_high = high_times_high + (high_times_low >> 32) + (low_times_high >> 32) + (middle >> 32);
        result_high += a_high * b_low + a_low * b_high;

        u128 result = ((u128)result_high << 64) | result_low;
        return (i128)result;
    }
#endif

static inline f32 ceilf_(f32 x) {
    assert((f32)INT64_MIN <= x && x <= (f32)INT64_MAX);
    i64 as_i64 = (i64)x;
    f32 result = (f32)((f32)as_i64 + (f32)(x > (f32)as_i64));
    return result;
}

static inline f32 floorf_(f32 x) {
    assert((f32)INT64_MIN <= x && x <= (f32)INT64_MAX);
    i64 as_i64 = (i64)x;
    f32 result = (f32)((f32)as_i64 - (f32)(x < (f32)as_i64));
    return result;
}

// Newton-Raphson
static inline f64 sqrt_(f64 x) {
    // false comparison catches nan
    if (!(x > 0)) return 0;

    union { f64 floating; u64 bits; } value = { .floating = x };
    value.bits = 0x5fe6eb50c7b537a9 - (value.bits >> 1); // rsqrt initial guess

    f64 half = 0.5 * x;
    u64 iterations = 2;
    for (u64 i = 0; i < iterations; i += 1) {
        value.floating *= 1.5 - half * value.floating * value.floating;
    }

    f64 result = x * value.floating; // sqrt from rsqrt
    return result;
}

// Newton-Raphson
static inline f32 sqrtf_(f32 x) {
    // false comparison catches nan
    if (!(x > 0)) return 0;

    union { f32 floating; u32 bits; } value = { .floating = x };
    value.bits = 0x5f375a86 - (value.bits >> 1); // rsqrt initial guess

    f32 half = 0.5f * x;
    u64 iterations = 2;
    for (u64 i = 0; i < iterations; i += 1) {
        value.floating *= 1.5f - half * value.floating * value.floating;
    }

    f32 result = x * value.floating; // sqrt from rsqrt
    return result;
}

static f64 acos_(f64 x) {
    // Caller must clamp.
    assert(-1.0 <= x);
    assert(x <= 1.0);

    f64 negate = x < 0 ? 1.0 : 0;
    x = x < 0 ? -x : x;
    f64 r = -0.0187293;
    r = r * x + 0.0742610;
    r = r * x - 0.2121144;
    r = r * x + 1.5707288;
    r *= sqrt_(1.0 - x);

    return negate * 3.14159265358979 + (1.0 - 2.0 * negate) * r;
}

#if !LINK_CRT
    #if COMPILER_CLANG
        #if BASE_OS == BASE_OS_WASM // TODO(felix): is this the right solution?
            // #define memcmp(a, b, c) __builtin_memcmp((a), (b), (size_t)(c))
            #define memcmp(a, b, c) memcmp_wasm((a), (b), (size_t)(c))
            static int memcmp_wasm(const void *a_, const void *b_, size_t c) {
                const unsigned char *a = a_, *b = b_;
                for (size_t i = 0; i < c; i += 1) {
                    if (a[i] != b[i]) return a[i] - b[i];
                }
                return 0;
            }
            #define memcpy(d, s, c) __builtin_memcpy((d), (s), (size_t)(c))
            #define memset(m, b, c) __builtin_memset((m), (b), (size_t)(c))
        #else
            #define memcmp __builtin_memcmp
            #define memcpy __builtin_memcpy
        #endif
    #endif
#endif

static const char **os_get_arguments(Arena *, u64 *out_count);

#define v(vector) (&(vector).x)
typedef struct { f32 x, y; } V2;
typedef struct { f32 x, y, z; } V3;
typedef struct { f32 x, y, z, w; } V4;
typedef V4 Quat;

#define v4_left(v) ((v).x)
#define v4_top(v) ((v).y)
#define v4_right(v) ((v).z)
#define v4_bottom(v) ((v).w)

typedef union {
    f32 c[3][3];
    V3 columns[3];
} M3;

typedef union {
    f32 c[4][4];
    V4 columns[4];
} M4;

#define PI_F32  3.14159265358979f
#define M_PI    3.14159265358979323846
#define M_PI_2 (M_PI * 0.5)

#define FLT_MIN     1.17549435e-38f
#define FLT_MAX     3.40282347e+38f
#define FLT_EPSILON 1.19209290e-07f

#define DBL_MIN     2.2250738585072014e-308
#define DBL_MAX     1.7976931348623157e+308
#define DBL_EPSILON 2.2204460492503131e-16

#define LDBL_MAX_EXP 16384

static f32 sinf_(f32 x) {
    f32 t = x * (1.f / 6.28318530f);
    t -= floorf_(t); // [0, 1) representing [0, 2pi)
    f32 sign = 1.f;

    if (t > 0.5f) {
        t -= 0.5f;
        sign = -1.f;
    }

    if (t > 0.25f) {
        // fold to [0, 0.25] = [0, pi/2]
        t = 0.5f - t;
    }

    x = t * 6.28318530f;
    f32 x2 = x * x;
    f32 result_unsigned = x * (1.f + x2 * (-0.16666667f + x2 * (0.00833333f + x2 * (-0.00019841f + x2 * 2.7526e-6f))));

    return sign * result_unsigned;
}

static inline f64 ceil_(f64 x) {
    assert((f64)INT64_MIN <= x && x <= (f64)INT64_MAX);
    i64 as_i64 = (i64)x;
    f64 result = (f64)((f64)as_i64 + (f64)(x > (f64)as_i64));
    return result;
}

static inline f64 floor_(f64 x) {
    assert((f64)INT64_MIN <= x && x <= (f64)INT64_MAX);
    i64 as_i64 = (i64)x;
    f64 result = (f64)((f64)as_i64 - (f64)(x < (f64)as_i64));
    return result;
}

static inline f64 round(f64 x) {
    if (x >= 0) return floor_(x + 0.5);
    else return ceil_(x - 0.5);
}

static f64 cos_(f64 x) {
    static const f64 TAU = 6.2831853071795862;
    x -= TAU * round(x * (1.0 / TAU));

    if (x < 0) x = -x;

    f64 sign = 1.0;
    if (x > M_PI_2) { x = M_PI - x; sign = -1.0; }

    f64 x2 = x * x;
    f64 r = -1.0 / 3628800.0;
    r = r * x2 + 1.0 / 40320.0;
    r = r * x2 - 1.0 / 720.0;
    r = r * x2 + 1.0 / 24.0;
    r = r * x2 - 0.5;
    r = r * x2 + 1.0;
    return sign * r;
}

static f32 cosf_(f32 x) {
    return sinf_(x + 1.57079632f);
}

static f32 expf_(f32 x) {
    if (x >  88.f) return 3.40282347e+38f;
    if (x < -88.f) return 0;

    // e^x = 2^n * 2^f, where t = x*log2(e)
    f32 t = x * 1.44269504f;
    i32 n = (i32)t;
    f32 f = t - (f32)n;

    // 2^f on [0,1] via Taylor of e^(f*ln2)
    f32 p = 1.f + f * (0.69314718f + f * (0.24022651f + f * (0.05550411f + f * (0.00961812f + f * 0.00133336f))));

    // multiply by 2^n by stuffing n into the f32 exponent
    i32 bits = (n + 127) << 23;
    f32 scale;
    memcpy(&scale, &bits, 4);

    return p * scale;
}

static f64 fmod_(f64 x, f64 y) {
    f64 dividend = x / y;
    assert((f64)INT64_MIN <= dividend && dividend <= (f64)INT64_MAX);
    return x - (f64)(i64)dividend * y;
}

static f32 fmodf_(f32 x, f32 y) {
    f32 dividend = x / y;
    assert((f32)INT64_MIN <= dividend && dividend <= (f32)INT64_MAX);
    return x - (f32)(i64)dividend * y;
}

static f32 frexpf_(f32 x, i32 *exponent) {
    u32 bits;
    memcpy(&bits, &x, 4);

    i32 biased_exponent = (bits >> 23) & 0xff;

    *exponent = biased_exponent - 126;
    bits = (bits & 0x807fffff) | (126 << 23);

    memcpy(&x, &bits, 4);
    return x;
}

static f32 logf_(f32 x) {
    i32 exponent = 0;
    f32 mantissa = frexpf_(x, &exponent);
    if (mantissa < 0.5f) {
        mantissa *= 2.f;
        exponent -= 1;
    }

    f32 ratio = (mantissa - 1.f) / (mantissa + 1.f);
    f32 ratio2 = ratio * ratio;
    f32 log_mantissa = ratio * (2.f + ratio2 * (0.6667f + ratio2 * 0.4f));

    f32 ln2 = 0.6931472f;
    return log_mantissa + (f32)exponent * ln2;
}

static f32 tanf_(f32 x) {
    f32 intermediate = x * (2.f / (f32)M_PI) + (x >= 0 ? 0.5f : -0.5f);
    assert((f32)INT64_MIN <= intermediate && intermediate <= (f32)INT64_MAX);
    i64 n = (i64)intermediate;
    f32 reduced = x - (f32)n * 1.5707964f;

    f32 r2 = reduced * reduced;
    f32 tan_reduced = reduced * (1.f + r2 * (0.33333f + r2 * (0.13333f + r2 * 0.05397f)));

    if (n & 1) return -1.f / tan_reduced;
    return tan_reduced;
}

static f32 tanhf_(f32 x) {
    if (x >  5.f) return  1.f;
    if (x < -5.f) return -1.f;
    f32 x2 = x * x;
    f32 a = x  * (135135.f + x2 * (17325.f + x2 * (378.f + x2)));
    f32 b = 135135.f + x2 * (62370.f + x2 * (3150.f + x2 * 28.f));
    return a / b;
}

#if COMPILER_CLANG
    #define frexpf __builtin_frexpf
    #define sinf   __builtin_sinf
    #define tanf   __builtin_tanf
    #if LINK_CRT
        #define acos  __builtin_acos
        #define cos   __builtin_cos
        #define cosf  __builtin_cosf
        #define expf  __builtin_expf
        #define fmod  __builtin_fmod
        #define fmodf __builtin_fmodf
        #define logf  __builtin_logf
        #define tanhf __builtin_tanhf
    #endif
#endif

#if !LINK_CRT
    #if !COMPILER_CLANG
        #define floor  floor_
        #define frexpf frexpf_
        #define sinf   sinf_
        #define tanf   tanf_
    #endif
    #define acos   acos_
    #define ceil   ceil_
    #define ceilf  ceilf_
    #define expf   expf_
    #define floorf floorf_
    #define fmod   fmod_
    #define fmodf  fmodf_
    #define cos    cos_
    #define cosf   cosf_
    #define logf   logf_
    #define sqrt   sqrt_
    #define sqrtf  sqrtf_
    #define tanhf  tanhf_
#endif

// TODO(felix): clean these up

#if COMPILER_MSVC
    #pragma intrinsic(abs, _abs64, fabs)
    #define llabs _abs64
    // NOTE(felix): because I got `warning C4163: 'fabsf': not available as an intrinsic function`
    // NOTE(felix): corecrt_math.h defines fabsf as an inline function casting fabs to f32. Might be good to benchmark that approach versus what I'm doing here
    static inline f32 fabsf(f32 x) { return x < 0 ? -x : x; }
#elif COMPILER_CLANG || COMPILER_GCC
    #define INFINITY __builtin_inff()
    #if LINK_CRT
        #define ceil   __builtin_ceil
        #define ceilf  __builtin_ceilf
        #define floorf __builtin_floorf
        #define sqrt   __builtin_sqrt
        #define sqrtf  __builtin_sqrtf
    #else
        #define ceil   ceil_
        #define ceilf  ceilf_
        #define floor  floor_
        #define floorf floorf_
        #define sqrt   sqrt_
        #define sqrtf  sqrtf_
    #endif
    #define abs(x)   __builtin_abs(x)
    #define fabs(x)  __builtin_fabs(x)
    #define fabsf(x) __builtin_fabsf(x)
    #define isnan(x) __builtin_isnan(x)
    #define llabs(x) __builtin_llabs(x)
    #if BASE_OS == BASE_OS_WASM
        static f32 atanf(f32 x) {
            bool negative = x < 0;
            if (negative) x = -x;

            bool inverse = x > 1.f;
            if (inverse) x = 1.f / x;

            f32 x2 = x*x;
            f32 r = x * (0.9998660f + x2 * (-0.3302995f + x2 * (0.1801410f + x2 * (-0.0851330f + x2 * 0.0208351f))));
            if (inverse) r = 1.57079632f - r;
            if (negative) r = -r;
            return r;
        }

        static f32 atan2f(f32 y, f32 x) {
            if (x == 0.f) return y >= 0 ? 1.57079632f : -1.57079632f;
            f32 r = atanf(y/x);
            if (x < 0) r += y >= 0 ? 3.14159265f : -3.14159265f;
            return r;
        }

        static f32 powf(f32 x, f32 e) {
            f32 result = expf(e * logf(x));
            return result;
        }
    #else
        #define atanf(x)     __builtin_atanf(x)
        #define atan2f(y, x) __builtin_atan2f((y), (x))
        #define pow(x, e)    __builtin_pow((x), (e))
        #define powf(x, e)   __builtin_powf((x), (e))
        #define roundf(x)    __builtin_roundf(x)
    #endif
#endif

static inline f32 roundf(f32 x) {
    if (x >= 0) return floorf(x + 0.5f);
    else return ceilf(x - 0.5f);
}

#if COMPILER_MSVC
    #define count_trailing_zeroes(x) (63 - __lzcnt64((i64)(x)))
#elif COMPILER_CLANG || COMPILER_GCC
    #define count_trailing_zeroes(x) (u64)(__builtin_ctzll(x))
#endif

static bool intersect_point_in_rectangle(V2 point, V4 rectangle);
static bool is_power_of_2(u64 x);

static inline f32 radians_from_degrees(f32 degrees);

static void rgba_double_from_hex(u32 hex, double rgba[4]);
static V4   rgba_float_from_hex(u32 hex);
static u32  rgba_hex_from_float(V4 rgba);

static inline bool f32_difference_within_margin(f32 a, f32 b, f32 margin);
static inline f32  f32_stable_lerp(f32 a, f32 b, f32 k, f32 delta_time_seconds);
static inline f32  f32_lerp(f32 a, f32 b, f32 amount);

static inline V2   v2(f32 value);
static inline V2   v2_add(V2 a, V2 b);
static inline V2   v2_div(V2 a, V2 b);
static inline f32  v2_dot(V2 a, V2 b);
static inline bool v2_difference_within_margin(V2 a, V2 b, f32 margin);
static inline f32  v2_len(V2 v);
static inline f32  v2_len_squared(V2 v);
static inline V2   v2_lerp(V2 a, V2 b, f32 amount);
static inline V2   v2_max(V2 a, V2 b);
static inline V2   v2_min(V2 a, V2 b);
static inline V2   v2_mul(V2 a, V2 b);
static inline V2   v2_norm(V2 v);
static inline V2   v2_reciprocal(V2 v);
static inline V2   v2_rotate(V2 v, f32 angle_radians);
static inline V2   v2_round(V2 v);
static inline V2   v2_round_down(V2 v);
static inline V2   v2_scale(V2 v, f32 s);
static inline V2   v2_stable_lerp(V2 a, V2 b, f32 k, f32 delta_time_seconds);
static inline V2   v2_sub(V2 a, V2 b);

static inline V3   v3_add(V3 a, V3 b);
static inline V3   v3_cross(V3 a, V3 b);
static inline f32  v3_dot(V3 a, V3 b);
static inline bool v3_difference_within_margin(V3 a, V3 b, f32 margin);
static inline V3   v3_forward_from_view(M4 view);
static inline f32  v3_len(V3 v);
static inline f32  v3_len_squared(V3 v);
static inline V3   v3_lerp(V3 a, V3 b, f32 amount);
static inline V3   v3_neg(V3 v);
static inline V3   v3_norm(V3 v);
static inline V3   v3_right_from_view(M4 view);
static inline V3   v3_scale(V3 v, f32 s);
static inline V3   v3_sub(V3 a, V3 b);
static inline V3   v3_unproject(V3 pos, M4 view_projection);
static inline V3   v3_up_from_view(M4 view);

static inline V4   v4_add(V4 a, V4 b);
static inline f32  v4_dot(V4 a, V4 b);
static inline bool v4_difference_within_margin(V4 a, V4 b, f32 margin);
static inline V4   v4_lerp(V4 a, V4 b, f32 amount);
static inline V4   v4_round(V4 v);
static inline V4   v4_scale(V4 v, f32 s);
static inline V4   v4_stable_lerp(V4 a, V4 b, f32 k, f32 delta_time_seconds);
static inline V4   v4_sub(V4 a, V4 b);
static inline V4   v4v(V3 xyz, f32 w);
static inline V3   v4_xyz(V4 v);

static inline Quat quat_from_rotation(V3 axis, f32 angle);
static inline Quat quat_mul_quat(Quat a, Quat b);
static inline V3   quat_rotate_v3(Quat q, V3 v);

static inline M3 m3_fill_diagonal(f32 value);
static inline M3 m3_from_rotation(f32 radians, V2 pivot);
static inline M3 m3_inverse(M3 m);
static inline M3 m3_model(V2 scale, f32 radians, V2 pivot, V2 post_translation);
static inline V3 m3_mul_v3(M3 m, V3 v);
static inline M3 m3_transpose(M3 m);

static inline M4 m4_fill_diagonal(f32 value);
static inline M4 m4_from_rotation(V3 axis, f32 angle);
static inline M4 m4_from_top_left_m3(M3 m);
static inline M4 m4_from_translation(V3 translation);
static inline M4 m4_inverse(M4 m);
static inline M4 m4_look_at(V3 eye, V3 centre, V3 up_direction);
static inline M4 m4_mul_m4(M4 a, M4 b);
static inline V4 m4_mul_v4(M4 m, V4 v);
static inline M4 m4_perspective_projection(f32 fov_vertical_radians, f32 width_over_height, f32 range_near, f32 range_far);
static inline M4 m4_transpose(M4 m);

#define CPU_DRAW_FUNCTION static
#include "base/cpu_draw.h"
static cpu_draw_Font cpu_draw_font_from_bdf(const void *bdf, u64 bdf_size);
static V2 cpu_draw_platform_measure_text(const cpu_draw_Font *font, String text, f32 font_size);

// TODO(felix): Move to cpu-rendered platform (or maybe it should always be manually included, and removed from base).
#define BDF_FUNCTION static
#define BDF_GLYPH_MAX_HEIGHT CPU_DRAW_GLYPH_MAX_HEIGHT
#define BDF_GLYPH_MAX_WIDTH_OVER_8 CPU_DRAW_GLYPH_MAX_WIDTH_OVER_8
#include "base/bdf.h"

static f64 f64_from_string(String s);
static i64 int_from_string_base(String s, u64 base);

static u64         cstring_copy(char *buffer, const char *cstring);
static const char *cstring_from_string(Arena *arena, String string);
static u64         cstring_length(const char *cstring);
static const char *cstring_print(Arena *arena, const char *format, ...);

static String string_from_int_base(Arena *arena, i64 _num, u8 base);
static String string_print_(Arena *arena, const char *fmt, va_list arguments);
static String string_print(Arena *arena, const char *fmt, ...);

typedef struct {
    char *data;
    u64 count, capacity;
} String_Builder;
static   void string_builder_push(String_Builder *builder, String string);
static   void string_builder_null_terminate(Arena *arena, String_Builder *builder);
static   void string_builder_print(Arena *arena, String_Builder *builder, const char* format, ...);
static   void string_builder_print_(Arena *arena, String_Builder *builder, const char *fmt, va_list arguments);
static   void string_builder_reserve_unused(Arena *arena, String_Builder *builder, u64 unused_count);
static String string_builder_string(String_Builder builder);

#define FS_FUNCTION static
#define FS_NO_SYSTEM_INCLUDE
#include "base/filesystem.h"
static String os_read_entire_file(Arena *arena, const char *path, bool *ok);
static   bool os_write_entire_file(const char *path, const void *data, u64 count);

typedef struct {
    u64 name_offset;
    u64 data_offset;
    u64 data_size;
} Embedded_File_Entry;
static String base_embedded_files_get(String name);

typedef enum {
    Os_Process_Flag_PRINT_COMMAND_BEFORE_RUNNING = 1 << 0,
    Os_Process_Flag_PRINT_EXECUTION_TIME = 1 << 1,
    Os_Process_Flag_PRINT_EXIT_CODE = 1 << 2,
    Os_Process_Flag_CAPTURE_OUTPUT = 1 << 3,
} Os_Process_Flags;

typedef struct {
    u32 exit_code;
    String output;
} Os_Process_Result;

static Os_Process_Result os_process_run(Arena *arena, const char **arguments, const char *directory, Os_Process_Flags flags);
static void  os_write_console(const char *bytes, u64 count);

static void print(const char *format, ...);
static void print_(const char *format, va_list arguments);

#define CLEX_FUNCTION static
#include "base/clex.h"
// TODO(felix): Make this growable.
#define BUILD_MACROS_EXPONENT 12
#define BUILD_MACROS_CAPACITY (1 << BUILD_MACROS_EXPONENT)

typedef u8 Build_Macro_State;
enum {
    Build_Macro_State_UNDEFINED = 0,
    Build_Macro_State_DEFINED,
    Build_Macro_State_DEFINED_VALUE_UNKNOWN,
    Build_Macro_State_UNKNOWN,
};

typedef u32 Build_Unknown_Reason_Id;

typedef struct {
    const char *description;
    Build_Unknown_Reason_Id cause;
} Build_Unknown_Reason;

typedef struct {
    Build_Unknown_Reason *items;
    u64 count;
    u64 capacity;
} Build_Unknown_Reasons;

typedef struct {
    Build_Macro_State state;
    String definition;
    Build_Unknown_Reason_Id unknown_reason;
} Build_Macro;

typedef struct {
    String name;
    Build_Macro macro;
    bool is_defined_by_source;
} Build_Macro_Slot;

typedef struct {
    Build_Macro_Slot slots[BUILD_MACROS_CAPACITY];
    u32 used;
    bool collect_unknown_reasons;
    Build_Unknown_Reasons reasons;
} Build_Macros;

static String base_string_from_clex_data(clex_Data data);

#define OBJECT_FUNCTION static
#include "base/object_files.h"

#define SERIAL_FUNCTION static
#include "base/serial.h"

typedef enum {
    Build_Compiler_MSVC,
    Build_Compiler_CLANG,
    Build_Compiler_COUNT,
} Build_Compiler;

typedef enum {
    Build_Linker_MSVC,
    Build_Linker_CLANG,
    Build_Linker_COUNT,
} Build_Linker;

typedef enum {
    Build_Mode_DEBUG,
    Build_Mode_RELEASE,
    Build_Mode_COUNT,
} Build_Mode;

typedef struct {
    u8 target_os;
    const char *apple_provision_path;
} Build_Options;

static u32 build_default_everything(Arena *scratch_arena, const char *program_name, const Build_Options *build_options);

// NOTE(felix): synced with ui_Key
typedef enum {
    App_Key_NIL = 0,

    // 32 -> 96 use ASCII values
    App_Key__ASCII_DELIMITER = 128,

    App_Key_SHIFT,
    App_Key_CONTROL,
    App_Key_LEFT,
    App_Key_DOWN,
    App_Key_UP,
    App_Key_RIGHT,

    App_Key_F1, App_Key_F2, App_Key_F3, App_Key_F4, App_Key_F5,
    App_Key_F6, App_Key_F7, App_Key_F8, App_Key_F9, App_Key_F10,
    App_Key_F11, App_Key_F12, App_Key_F13, App_Key_F14, App_Key_F15,
    App_Key_F16, App_Key_F17, App_Key_F18, App_Key_F19, App_Key_F20,
    App_Key_F21, App_Key_F22, App_Key_F23, App_Key_F24,

    App_Key_MAX_VALUE,
} App_Key;

// NOTE(felix): synced with ui_Mouse_Button
typedef enum {
    App_Mouse_Button_LEFT,
    App_Mouse_Button_MIDDLE,
    App_Mouse_Button_RIGHT,
    App_Mouse_Button_MAX_VALUE,
} App_Mouse_Button;

typedef struct {
    V2 window_size;
    f32 dpi_scale;
    f32 seconds_since_last_frame;
    V2 mouse_position;
    f32 scroll;
    bool mouse_clicked[App_Mouse_Button_MAX_VALUE];
    bool mouse_down[App_Mouse_Button_MAX_VALUE];
    bool key_down[App_Key_MAX_VALUE];
    bool key_pressed[App_Key_MAX_VALUE];
    f32 safe_area_inset_left;
    f32 safe_area_inset_top;
    f32 safe_area_inset_right;
    f32 safe_area_inset_bottom;
} App_Frame_Info;

static void app_frame_info_reset_end_of_frame(App_Frame_Info *);

#define BYTE_FROM_BITS(_0, _1, _2, _3, _4, _5, _6, _7) \
    ( ((_0) << 7) | ((_1) << 6) | ((_2) << 5) | ((_3) << 4) | \
      ((_4) << 3) | ((_5) << 2) | ((_6) << 1) | ((_7) << 0) )

typedef u32 GPU_Mesh_Id;
typedef u32 GPU_Shader_Id;
typedef u32 GPU_Texture_Id;

typedef struct {
    const void *data;
    u64 size;
} GPU_Bytes;

// TODO(felix): Check the full std140 layout once the metaprogram can use the parser to do that.
#define gpu_arguments(value) ((GPU_Bytes){ \
    .data = &(value), \
    .size = sizeof(value) + 0 * sizeof(char[sizeof(value) % 16 == 0 ? 1 : -1]), /*NOTE(felix): This acts as a static assert.*/\
})

typedef u8 GPU_Blend_Mode;
enum {
    GPU_Blend_Mode_ALPHA = 0,
    GPU_Blend_Mode_PREMULTIPLIED_ALPHA,
    GPU_Blend_Mode_ADDITIVE,
    GPU_Blend_Mode_NONE,

    GPU_Blend_Mode_COUNT,
};

typedef u8 GPU_Cull_Mode;
enum {
    GPU_Cull_Mode_NONE = 0,
    GPU_Cull_Mode_BACK,
    GPU_Cull_Mode_FRONT,

    GPU_Cull_Mode_COUNT,
};

typedef u8 GPU_Depth_Test;
enum {
    GPU_Depth_Test_NONE = 0,
    GPU_Depth_Test_LESS,
    GPU_Depth_Test_LESS_OR_EQUAL,

    GPU_Depth_Test_COUNT,
};

typedef u8 GPU_Index_Format;
enum {
    GPU_Index_Format_NONE = 0,
    GPU_Index_Format_U16,
    GPU_Index_Format_U32,

    GPU_Index_Format_COUNT,
};

typedef u8 GPU_Texture_Filter;
enum {
    GPU_Texture_Filter_LINEAR = 0,
    GPU_Texture_Filter_NEAREST,

    GPU_Texture_Filter_COUNT,
};

typedef u8 GPU_Texture_Flags;
enum {
    GPU_Texture_Flag_RENDER_TARGET = 1 << 0,
    GPU_Texture_Flag_DEPTH_BUFFER  = 1 << 1,
    GPU_Texture_Flag_MIPMAPS       = 1 << 2,
};

typedef u8 GPU_Texture_Format;
enum {
    GPU_Texture_Format_RGBA8 = 0,
    GPU_Texture_Format_R8,
    GPU_Texture_Format_RGBA16F,

    GPU_Texture_Format_COUNT,
};

typedef u8 GPU_Texture_Wrap;
enum {
    GPU_Texture_Wrap_CLAMP_TO_EDGE = 0,
    GPU_Texture_Wrap_REPEAT,
    GPU_Texture_Wrap_MIRRORED_REPEAT,

    GPU_Texture_Wrap_COUNT,
};

typedef u8 GPU_Topology;
enum {
    GPU_Topology_TRIANGLES = 0,
    GPU_Topology_TRIANGLE_STRIP,
    GPU_Topology_LINES,
    GPU_Topology_POINTS,

    GPU_Topology_COUNT,
};

// TODO(felix): Vertex pulling (texelFetch from an RGBA32F/RGBA32UI data texture with gl_VertexID) would remove GPU_Vertex_Format and the per-mesh vertex array state.
typedef u8 GPU_Vertex_Format;
enum {
    GPU_Vertex_Format_NONE = 0,
    GPU_Vertex_Format_F32X1,
    GPU_Vertex_Format_F32X2,
    GPU_Vertex_Format_F32X3,
    GPU_Vertex_Format_F32X4,
    GPU_Vertex_Format_U8X4_NORMALISED,
    GPU_Vertex_Format_U32X1,

    GPU_Vertex_Format_COUNT,
};

typedef struct {
    String vertex_source;
    String fragment_source;
} GPU_Shader;

#define GPU_MAX_VERTEX_ATTRIBUTES 8
typedef struct {
    GPU_Bytes vertices;
    GPU_Vertex_Format vertex_attributes[GPU_MAX_VERTEX_ATTRIBUTES];
    GPU_Bytes indices;
    GPU_Index_Format index_format;
} GPU_Mesh;

// NOTE(felix): Key == 0 identifies the texture by a hash of its content; key != 0 with pixels uploads them under the key; key != 0 without pixels uses what the key already has, or creates an empty texture if given a size.
typedef struct {
    u64 key;
    u32 width;
    u32 height;
    u32 pixels_per_row;
    GPU_Texture_Format format;
    GPU_Texture_Flags flags;
    GPU_Texture_Filter filter;
    GPU_Texture_Wrap wrap;
    GPU_Bytes pixels;
} GPU_Texture;

#define GPU_MAX_TEXTURES_PER_DRAW 4
#define GPU_MAX_INSTANCE_ATTRIBUTES 8
_Static_assert(GPU_MAX_VERTEX_ATTRIBUTES + GPU_MAX_INSTANCE_ATTRIBUTES <= 16, "GLES 3.0 and WebGL2 guarantee 16 vertex attributes");
typedef struct {
    GPU_Texture_Id target;
    V4 clip;
    M4 transform;

    GPU_Shader_Id shader;
    GPU_Mesh_Id mesh;
    V4 rectangle;
    GPU_Texture_Id textures[GPU_MAX_TEXTURES_PER_DRAW];
    GPU_Bytes arguments;

    GPU_Bytes instances;
    GPU_Vertex_Format instance_attributes[GPU_MAX_INSTANCE_ATTRIBUTES];
    u32 instance_count;

    GPU_Topology topology;
    GPU_Blend_Mode blend_mode;
    GPU_Depth_Test depth_test;
    bool depth_write;
    GPU_Cull_Mode cull_mode;
} GPU_Draw;

typedef struct {
    u32 commands;
    u32 draw_calls;
    u32 program_changes;
    u32 shader_compiles;
    u32 cache_hits;
    u32 cache_misses;
    u32 promotions;
    u32 evictions;
    u32 failed_draws;
    u64 bytes_uploaded;
    u64 cached_bytes;
} GPU_Frame_Stats;

static const u32 gpu_vertex_format_size[GPU_Vertex_Format_COUNT] = {
    [GPU_Vertex_Format_F32X1] = 4,
    [GPU_Vertex_Format_F32X2] = 8,
    [GPU_Vertex_Format_F32X3] = 12,
    [GPU_Vertex_Format_F32X4] = 16,
    [GPU_Vertex_Format_U8X4_NORMALISED] = 4,
    [GPU_Vertex_Format_U32X1] = 4,
};

static const u32 gpu_index_format_size[GPU_Index_Format_COUNT] = {
    [GPU_Index_Format_U16] = 2,
    [GPU_Index_Format_U32] = 4,
};

static const u32 gpu_texture_bytes_per_pixel[GPU_Texture_Format_COUNT] = {
    [GPU_Texture_Format_RGBA8]   = 4,
    [GPU_Texture_Format_R8]      = 1,
    [GPU_Texture_Format_RGBA16F] = 8,
};

typedef struct {
    App_Frame_Info frame;
    Arena *persistent_arena;
    Arena *frame_arena;
    u64 frame_arena_startup_reservation_in_bytes;
    GPU_Frame_Stats gpu_stats_of_last_frame;

    bool hide_cursor;
    bool should_quit;
    const char *window_name;
    u32 clear_color;
} Platform;

#if PLATFORM_NONE
    typedef Platform Platform_Implementation;
    static V2 platform_measure_text(Platform *platform, String text, f32 font_size) {
        (void)platform;
        (void)text;
        (void)font_size;
        panic("PLATFORM=NONE");
        return (V2){0};
    }
#else
    struct Platform_Implementation;
    typedef struct Platform_Implementation Platform_Implementation;

    static V2 platform_measure_text(Platform *platform, String text, f32 font_size);

    static void app_start(Platform *);
    static void app_update_and_render(Platform *);
#endif

static void base_platform_reserve_frame_arena(Platform *platform);


// TODO(felix): Reorder neighbouring draws by state when their blend mode does not depend on order.
static void           gpu_clear(Platform *platform, GPU_Texture_Id target, u32 color);
static void           gpu_draw(Platform *platform, const GPU_Draw *draw);
static GPU_Mesh_Id    gpu_mesh_id(Platform *platform, const GPU_Mesh *mesh);
static GPU_Shader_Id  gpu_shader_id(Platform *platform, const GPU_Shader *shader, bool *ok);
static GPU_Texture_Id gpu_texture_id(Platform *platform, const GPU_Texture *texture);

#define UI_FUNCTION static
#define ui_va_list va_list
#define ui_va_start va_start
#define ui_va_end va_end
#include "base/ui.h"

static ui_Box *base_ui_begin(ui_State *ui, App_Frame_Info *frame);
static void    base_ui_create(ui_State *ui, Platform *platform);
static void    base_ui_measure_text(void *user_data, const char *string, ui_Style style, f32 size[2]);
static void    base_ui_render(ui_State *ui, ui_Box *box);


#endif // !defined(BASE_H)


#if defined(BASE_IMPLEMENTATION)


static inline bool assert_expression(bool value) {
    assert(value);
    return true;
}

#if BUILD_DEBUG && COMPILER_MSVC
    #pragma section(".raddbg", read, write)
    #define raddbg_exe_data __declspec(allocate(".raddbg"))
    raddbg_exe_data unsigned char raddbg_is_attached_byte_marker[1];
    #define raddbg_glue_(a, b) a##b
    #define raddbg_glue(a, b) raddbg_glue_(a, b)
    #define raddbg_gen_data_id() raddbg_glue(raddbg_data__, __COUNTER__)

    #define raddbg_type_view(type, ...) raddbg_exe_data char raddbg_gen_data_id()[] = ("type_view: {type: ```" #type "```, expr: ```" #__VA_ARGS__ "```}");
    #define raddbg_entry_point(...) raddbg_exe_data char raddbg_gen_data_id()[] = ("entry_point: \"" #__VA_ARGS__ "\"");
#else
    #define raddbg_type_view(...)
    #define raddbg_entry_point(...)
#endif

raddbg_entry_point(program)
raddbg_type_view(String, view:text($.data, size=count))

static void program(void);

#if LINK_CRT || (BASE_OS & BASE_OS_ANY_POSIX)
    static int main_argc__;
    static char **main_argv__;

    static int main_return_value__;

    int main(int argc, char **argv) {
        main_argc__ = argc;
        main_argv__ = argv;
        program();
        os_exit(main_return_value__);
    }
#elif BASE_OS == BASE_OS_WINDOWS || BASE_OS == BASE_OS_WASM
    void entrypoint(void) {
        program();
        os_exit(0);
    }
#endif

#if BASE_OS == BASE_OS_WINDOWS
    static bool os_windows_is_argument_separator(char c) {
        return c == ' ' || c == '\t';
    }
#endif

static const char **os_get_arguments(Arena *arena, u64 *out_count) {
    const char **arguments = 0;

    #if BASE_OS == BASE_OS_WINDOWS
    {
        UNICODE_STRING command_line_utf16 = RtlGetCurrentPeb()->ProcessParameters->CommandLine;
        u64 unit_count = command_line_utf16.Length / sizeof(u16);
        u64 count = utf8_from_utf16(0, 0, command_line_utf16.Buffer, unit_count).destination_units_written;

        char *command_line = arena_make(arena, count + 1, char);
        utf8_from_utf16(command_line, count, command_line_utf16.Buffer, unit_count);

        // NOTE(felix): Follows the splitting rules of the Microsoft C runtime since 2008.
        u64 read = 0;
        u64 written = 0;
        u64 argument_count = 0;
        bool program_name_is_quoted = count > 0 && command_line[0] == '"';
        read += program_name_is_quoted;
        while (read < count && (program_name_is_quoted ? command_line[read] != '"' : !os_windows_is_argument_separator(command_line[read]))) {
            command_line[written++] = command_line[read++];
        }

        read += program_name_is_quoted && read < count;
        while (read < count && os_windows_is_argument_separator(command_line[read])) read += 1;
        command_line[written++] = 0;
        argument_count += 1;

        while (read < count) {
            bool in_quotes = false;
            while (read < count && (in_quotes || !os_windows_is_argument_separator(command_line[read]))) {
                u64 backslash_count = 0;
                while (read < count && command_line[read] == '\\') {
                    backslash_count += 1;
                    read += 1;
                }

                bool quote_follows = read < count && command_line[read] == '"';
                u64 literal_backslash_count = quote_follows ? backslash_count / 2 : backslash_count;
                for (u64 i = 0; i < literal_backslash_count; i += 1) command_line[written++] = '\\';

                if (quote_follows) {
                    bool is_escaped_quote = backslash_count % 2 == 1;
                    bool is_doubled_quote = !is_escaped_quote && in_quotes && read + 1 < count && command_line[read + 1] == '"';
                    if (is_escaped_quote || is_doubled_quote) command_line[written++] = '"';
                    else in_quotes = !in_quotes;
                    read += 1 + is_doubled_quote;
                } else if (backslash_count == 0) command_line[written++] = command_line[read++];
            }

            while (read < count && os_windows_is_argument_separator(command_line[read])) read += 1;
            command_line[written++] = 0;
            argument_count += 1;
        }

        arguments = arena_make(arena, argument_count + 1, const char *);
        const char *argument = command_line;
        for (u64 i = 0; i < argument_count; i += 1) {
            arguments[i] = argument;
            argument += cstring_length(argument) + 1;
        }
        *out_count = argument_count;
    }
    #elif BASE_OS & BASE_OS_ANY_POSIX
    {
        (void)arena;
        *out_count = (u64)main_argc__;
        arguments = (const char **)main_argv__;
    }
    #elif BASE_OS == BASE_OS_WASM
    {
        (void)arena;
        *out_count = 0;
    }
    #endif

    return arguments;
}

#define STRINGIFY(...) #__VA_ARGS__

static inline bool ascii_is_alpha(char c) { return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z'); }

static inline bool ascii_is_decimal(char c) { return '0' <= c && c <= '9'; }

static inline bool ascii_is_hexadecimal(char c) { return ('0' <= c && c <= '9') || ('A' <= c && c <= 'F') || ('a' <= c && c <= 'f'); }

static inline bool ascii_is_whitespace(char c) { return c == ' ' || c == '\n' || c == '\r' || c == '\t'; }

#if BUILD_ASAN
    void __asan_poison_memory_region(void const volatile *address, size_t byte_count);
    void __asan_unpoison_memory_region(void const volatile *address, size_t byte_count);
    #define asan_poison_memory_region(address, byte_count)   __asan_poison_memory_region((address), (byte_count))
    #define asan_unpoison_memory_region(address, byte_count) __asan_unpoison_memory_region((address), (byte_count))
#else
    #define asan_poison_memory_region(address, byte_count)   ((void)(address), (void)(byte_count))
    #define asan_unpoison_memory_region(address, byte_count) ((void)(address), (void)(byte_count))
#endif

#define POISON_BYTE 0xcd

#define ARENA_IMPLEMENTATION
#define ARENA_ASSERT assert
#define ARENA_PANIC(message) panic(message)
#define ARENA_MEMCPY memcpy
#define ARENA_MEMSET memset
#define ARENA_ASAN_POISON(address, byte_count)   asan_poison_memory_region((address), (byte_count))
#define ARENA_ASAN_UNPOISON(address, byte_count) asan_unpoison_memory_region((address), (byte_count))
#define ARENA_ASAN BUILD_ASAN
#if BUILD_DEBUG
    #define ARENA_POISON_BYTE POISON_BYTE
#endif
#include "base/arena.h"

static void *base_allocate_from_arena_wrapper(void *context, void *pointer, size_t old_size, size_t new_size) {
    Arena *arena = context;
    void *result = 0;
    if (new_size != 0) {
        result = arena_make_(arena, new_size, 1, true);
        if (pointer != 0) memcpy(result, pointer, MIN(old_size, new_size));
    }
    return result;
}

#define DS_IMPLEMENTATION
#define DS_ALLOCATE(context, pointer, old_size, new_size) base_allocate_from_arena_wrapper((context), (pointer), (old_size), (new_size))
#include "base/data_structures.h"

static void *arena_array_reserve_(Arena *arena, void *data, size_t item_size, u64 count, u64 *capacity, u64 minimum) {
    void *result = data;
    if (*capacity < minimum) {
        assert(item_size > 0);
        assert(count <= *capacity);

        if (data != 0) {
            u64 arena_end = (u64)arena->memory + arena->offset;
            u64 data_end = (u64)data + *capacity * item_size;
            bool is_array_on_this_arena = (u64)arena->memory <= (u64)data && data_end <= arena_end;
            assert(is_array_on_this_arena);
        }

        u64 new_capacity = ds_array_grow_capacity(*capacity, minimum);
        result = ds_array_resize(arena, 0, item_size, 0, new_capacity);
        if (result == 0) panic("unable to reserve %llu items of %llu bytes", new_capacity, (u64)item_size);

        size_t used_size = size_t_from_u64(count) * item_size;
        if (data != 0) memcpy(result, data, used_size);
        *capacity = new_capacity;
    }
    return result;
}


static bool intersect_point_in_rectangle(V2 point, V4 rectangle) {
    bool result = true;
    result = result && v4_left(rectangle) < point.x && point.x < v4_right(rectangle);
    result = result && v4_top(rectangle) < point.y && point.y < v4_bottom(rectangle);
    return result;
}

static bool is_power_of_2(u64 x) {
    return (x & (x - 1)) == 0;
}

static inline f32 radians_from_degrees(f32 degrees) { return degrees * PI_F32 / 180.f; }

static void rgba_double_from_hex(u32 hex, double rgba[4]) {
    double pack = 1.0 / 255.0;
    rgba[0] = pack * (hex >> 24);
    rgba[1] = pack * ((hex >> 16) & 0xff);
    rgba[2] = pack * ((hex >> 8) & 0xff);
    rgba[3] = pack * (hex & 0xff);
}

static V4 rgba_float_from_hex(u32 hex) {
    f32 pack = 1.f / 255.f;
    V4 result = {
        .x = pack * (hex >> 24),
        .y = pack * ((hex >> 16) & 0xff),
        .z = pack * ((hex >> 8) & 0xff),
        .w = pack * (hex & 0xff),
    };
    return result;
}

static u32 rgba_hex_from_float(V4 rgba) {
    u32 c[4] = {0};
    for (u64 i = 0; i < 4; i += 1) c[i] = (u32)(v(rgba)[i] * 255.f + 0.5f);
    u32 result = (c[0] << 24) | (c[1] << 16) | (c[2] << 8) | c[3];
    return result;
}

static inline bool f32_difference_within_margin(f32 a, f32 b, f32 margin) {
    bool result = fabsf(a - b) <= margin;
    return result;
}

static inline f32 f32_stable_lerp(f32 a, f32 b, f32 k, f32 delta_time_seconds) {
	// Courtesy of GingerBill's BSC talk, this is framerate-independent lerping:
	// a += (b - a) * (1.0 - exp(-k * dt))
    f32 lerp_amount = 1.f - expf(-k * delta_time_seconds);
    f32 result = a + (b - a) * lerp_amount;
    return result;
}

static inline f32 f32_lerp(f32 a, f32 b, f32 amount) { return a + amount * (b - a); }

static inline V2 v2(f32 value) { return (V2){ .x = value, .y = value }; }

static inline V2 v2_add(V2 a, V2 b) { return (V2){ .x = a.x + b.x, .y = a.y + b.y }; }

static inline V2 v2_div(V2 a, V2 b) { return (V2){ .x = a.x / b.x, .y = a.y / b.y }; }

static inline f32 v2_dot(V2 a, V2 b) { return a.x * b.x + a.y * b.y; }

static inline bool v2_difference_within_margin(V2 a, V2 b, f32 margin) {
    bool result = fabsf(a.x - b.x) <= margin;
    result = result && fabsf(a.y - b.y) <= margin;
    return result;
}

static inline f32 v2_len(V2 v) { return sqrtf(v2_len_squared(v)); }

static inline f32 v2_len_squared(V2 v) { return v2_dot(v, v); }

static inline V2 v2_lerp(V2 a, V2 b, f32 amount) {
    V2 add = v2_scale(v2_sub(b, a), amount);
    return v2_add(a, add);
}

static inline V2 v2_max(V2 a, V2 b) { return (V2){ .x = MAX(a.x, b.x), .y = MAX(a.y, b.y) }; }

static inline V2 v2_min(V2 a, V2 b) { return (V2){ .x = MIN(a.x, b.x), .y = MIN(a.y, b.y) }; }

static inline V2 v2_mul(V2 a, V2 b) { return (V2){ .x = a.x * b.x, .y = a.y * b.y }; }

static inline V2 v2_norm(V2 v) {
    f32 length = v2_len(v);
    if (length == 0.f) return (V2){0};
    return (V2){ .x = v.x / length, .y = v.y / length };
}

static inline V2 v2_reciprocal(V2 v) { return (V2){ .x = 1.f / v.x, .y = 1.f / v.y }; }

static inline V2 v2_rotate(V2 v, f32 angle_radians) {
    f32 sin_angle = sinf(angle_radians);
    f32 cos_angle = cosf(angle_radians);
    V2 result = { .x = v.x * cos_angle - v.y * sin_angle, .y = v.x * sin_angle + v.y * cos_angle };
    return result;
}

static inline V2 v2_round(V2 v) { return (V2){ .x = roundf(v.x), .y = roundf(v.y) }; }

static inline V2 v2_round_down(V2 v) { return v2_round(v2_sub(v, (V2){ .x = 0.5f, .y = 0.5f })); }

static inline V2 v2_scale(V2 v, f32 s) { return (V2){ .x = v.x * s, .y = v.y * s }; }

static inline V2 v2_stable_lerp(V2 a, V2 b, f32 k, f32 delta_time_seconds) {
    for (u64 i = 0; i < 2; i += 1) {
        v(a)[i] = f32_stable_lerp(v(a)[i], v(b)[i], k, delta_time_seconds);
    }
    return a;
}

static inline V2 v2_sub(V2 a, V2 b) { return (V2){ .x = a.x - b.x, .y = a.y - b.y }; }

static inline V3 v3_add(V3 a, V3 b) { return (V3){ .x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z }; }

static inline V3 v3_cross(V3 a, V3 b) {
    return (V3) {
        .x = a.y * b.z - a.z * b.y,
        .y = a.z * b.x - a.x * b.z,
        .z = a.x * b.y - a.y * b.x,
    };
}

static inline f32 v3_dot(V3 a, V3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

static inline bool v3_difference_within_margin(V3 a, V3 b, f32 margin) {
    bool result = fabsf(a.x - b.x) <= margin;
    result = result && fabsf(a.y - b.y) <= margin;
    result = result && fabsf(a.z - b.z) <= margin;
    return result;
}

static inline V3 v3_forward_from_view(M4 view) {
    return (V3){ .x = -view.c[0][2], .y = -view.c[1][2], .z = -view.c[2][2] };
}

static inline f32 v3_len(V3 v) { return sqrtf(v3_len_squared(v)); }

static inline f32 v3_len_squared(V3 v) { return v3_dot(v, v); }

static inline V3 v3_lerp(V3 a, V3 b, f32 amount) {
    V3 add = v3_scale(v3_sub(b, a), amount);
    return v3_add(a, add);
}

static inline V3 v3_neg(V3 v) { return (V3){ .x = -v.x, .y = -v.y, .z = -v.z }; }

static inline V3 v3_norm(V3 v) {
    f32 length = v3_len(v);
    if (length == 0.f) return (V3){0};
    return (V3){
        .x = v.x / length,
        .y = v.y / length,
        .z = v.z / length,
    };
}

static inline V3 v3_right_from_view(M4 view) {
    return (V3){ .x = view.c[0][0], .y = view.c[1][0], .z = view.c[2][0] };
}

static inline V3 v3_scale(V3 v, f32 s) { return (V3){ .x = v.x * s, .y = v.y * s, .z = v.z * s }; }

static inline V3 v3_sub(V3 a, V3 b) { return (V3){ .x = a.x - b.x, .y = a.y - b.y, .z = a.z - b.z }; }

static inline V3 v3_unproject(V3 pos, M4 view_projection) {
    M4 view_projection_inv = m4_inverse(view_projection);
    Quat q = v4v(pos, 1.f);
    Quat q_trans = m4_mul_v4(view_projection_inv, q);

    V3 q_trans_xyz = {0};
    memcpy(&q_trans_xyz, &q_trans, sizeof q_trans_xyz);

    return v3_scale(q_trans_xyz, 1.f / q_trans.w);
}

static inline V3 v3_up_from_view(M4 view) {
    return (V3){ .x = view.c[0][1], .y = view.c[1][1], .z = view.c[2][1] };
}

static inline V4 v4_add(V4 a, V4 b) { return (V4){ .x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z, .w = a.w + b.w }; }

static inline f32 v4_dot(V4 a, V4 b) {
    f32 dot = 0;
    for (u64 i = 0; i < 4; i += 1) dot += v(a)[i] * v(b)[i];
    return dot;
}

static inline bool v4_difference_within_margin(V4 a, V4 b, f32 margin) {
    bool result = fabsf(a.x - b.x) <= margin;
    result = result && fabsf(a.y - b.y) <= margin;
    result = result && fabsf(a.z - b.z) <= margin;
    result = result && fabsf(a.w - b.w) <= margin;
    return result;
}

static inline V4 v4_lerp(V4 a, V4 b, f32 amount) {
    V4 add = v4_scale(v4_sub(b, a), amount);
    return v4_add(a, add);
}

static inline V4 v4_round(V4 v) {
    v.x = roundf(v.x);
    v.y = roundf(v.y);
    v.z = roundf(v.z);
    v.w = roundf(v.w);
    return v;
}

static inline V4 v4_scale(V4 v, f32 s) { return (V4){ .x = v.x * s, .y = v.y * s, .z = v.z * s, .w = v.w * s }; }

static inline V4 v4_stable_lerp(V4 a, V4 b, f32 k, f32 delta_time_seconds) {
    for (u64 i = 0; i < 4; i += 1) {
        v(a)[i] = f32_stable_lerp(v(a)[i], v(b)[i], k, delta_time_seconds);
    }
    return a;
}

static inline V4 v4_sub(V4 a, V4 b) { return (V4){ .x = a.x - b.x, .y = a.y - b.y, .z = a.z - b.z, .w = a.w - b.w }; }

static V4 v4v(V3 xyz, f32 w) { return (V4){ .x = xyz.x, .y = xyz.y, .z = xyz.z, .w = w }; }

static inline V3 v4_xyz(V4 v) {
    V3 result = { .x = v.x, .y = v.y, .z = v.z };
    return result;
}

static inline Quat quat_from_rotation(V3 axis, f32 angle) {
    f32 half_angle = angle / 2.f;
    f32 sin_half_angle = sinf(half_angle);
    return (Quat){
        .x = axis.x * sin_half_angle,
        .y = axis.y * sin_half_angle,
        .z = axis.z * sin_half_angle,
        .w = cosf(half_angle),
    };
}

static inline Quat quat_mul_quat(Quat a, Quat b) {
    return (Quat){
        .x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
        .y = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
        .z = a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
        .w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z,
    };
}

static inline V3 quat_rotate_v3(Quat q, V3 v) {
    Quat qv = { .x = v.x, .y = v.y, .z = v.z };
    Quat q_conjugate = (Quat){ .x = -q.x, .y = -q.y, .z = -q.z, .w = q.w };
    Quat result_as_quat = quat_mul_quat(quat_mul_quat(q, qv), q_conjugate);

    V3 result = {0};
    memcpy(&result, &result_as_quat, sizeof result);
    return result;
}

static inline M3 m3_fill_diagonal(f32 value) {
    M3 result = { .c = { [0][0] = value, [1][1] = value, [2][2] = value } };
    return result;
}

static inline M3 m3_from_rotation(f32 radians, V2 pivot) {
    return m3_model((V2){ .x = 1.f, .y = 1.f }, radians, pivot, (V2){0});
}

static inline M3 m3_inverse(M3 m) {
    M3 cross = { .columns = {
        [0] = v3_cross(m.columns[1], m.columns[2]),
        [1] = v3_cross(m.columns[2], m.columns[0]),
        [2] = v3_cross(m.columns[0], m.columns[1]),
    } };

    float inverse_determinant = 1.f / v3_dot(cross.columns[2], m.columns[2]);

    M3 result = { .columns = {
        [0] = v3_scale(cross.columns[0], inverse_determinant),
        [1] = v3_scale(cross.columns[1], inverse_determinant),
        [2] = v3_scale(cross.columns[2], inverse_determinant),
    } };

    result = m3_transpose(result);
    return result;
}

static inline M3 m3_model(V2 scale, f32 radians, V2 pivot, V2 post_translation) {
    f32 cos_value = cosf(radians);
    f32 sin_value = sinf(radians);

    f32 a = cos_value * scale.x;
    f32 b = sin_value * scale.y;
    f32 c = -sin_value * scale.x;
    f32 d = cos_value * scale.y;

    V2 a_pivot = { .x = a * pivot.x + c * pivot.y, .y = b * pivot.x + d * pivot.y };
    V2 translation = v2_sub(pivot, a_pivot);
    translation = v2_add(translation, post_translation);

    M3 result = { .c = {
        [0][0] = a,
        [0][1] = b,

        [1][0] = c,
        [1][1] = d,

        [2][0] = translation.x,
        [2][1] = translation.y,
        [2][2] = 1.f,
    } };

    return result;
}

static inline V3 m3_mul_v3(M3 m, V3 v) {
    return (V3){
        .x = v3_dot(v, (V3){ .x = m.c[0][0], .y = m.c[0][1], .z = m.c[0][2] }),
        .y = v3_dot(v, (V3){ .x = m.c[1][0], .y = m.c[1][1], .z = m.c[1][2] }),
        .z = v3_dot(v, (V3){ .x = m.c[2][0], .y = m.c[2][1], .z = m.c[2][2] }),
    };
}

static inline M3 m3_transpose(M3 m) {
    M3 result = m;
    result.c[0][1] = m.c[1][0];
    result.c[0][2] = m.c[2][0];
    result.c[1][0] = m.c[0][1];
    result.c[1][2] = m.c[2][1];
    result.c[2][1] = m.c[1][2];
    result.c[2][0] = m.c[0][2];
    return result;
}

static inline M4 m4_fill_diagonal(f32 value) {
    return (M4){ .c = { [0][0] = value, [1][1] = value, [2][2] = value, [3][3] = value } };
}

static inline M4 m4_from_rotation(V3 axis, f32 angle) {
    axis = v3_norm(axis);

    f32 sin_theta = sinf(angle);
    f32 cos_theta = cosf(angle);
    f32 cos_value = 1.f - cos_theta;

    return (M4){ .c = {
        [0][0] = (axis.x * axis.x * cos_value) + cos_theta,
        [0][1] = (axis.x * axis.y * cos_value) + (axis.z * sin_theta),
        [0][2] = (axis.x * axis.z * cos_value) - (axis.y * sin_theta),

        [1][0] = (axis.y * axis.x * cos_value) - (axis.z * sin_theta),
        [1][1] = (axis.y * axis.y * cos_value) + cos_theta,
        [1][2] = (axis.y * axis.z * cos_value) + (axis.x * sin_theta),

        [2][0] = (axis.z * axis.x * cos_value) + (axis.y * sin_theta),
        [2][1] = (axis.z * axis.y * cos_value) - (axis.x * sin_theta),
        [2][2] = (axis.z * axis.z * cos_value) + cos_theta,

        [3][3] = 1.f,
    } };
}

static inline M4 m4_from_top_left_m3(M3 m) {
    M4 result = {0};
    for (u64 i = 0; i < 3; i += 1) memcpy(&result.columns[i], &m.columns[i], sizeof *m.columns);
    return result;
}

static inline M4 m4_from_translation(V3 translation) {
    M4 result = m4_fill_diagonal(1.f);
    result.c[3][0] = translation.x;
    result.c[3][1] = translation.y;
    result.c[3][2] = translation.z;
    return result;
}

static inline M4 m4_inverse(M4 m) {
    V4 *cols = m.columns;

    V3 cross_0_1 = v3_cross(v4_xyz(cols[0]), v4_xyz(cols[1]));
    V3 cross_2_3 = v3_cross(v4_xyz(cols[2]), v4_xyz(cols[3]));
    V3 sub_1_0 = v3_sub(v3_scale(v4_xyz(cols[0]), cols[1].w), v3_scale(v4_xyz(cols[1]), cols[0].w));
    V3 sub_3_2 = v3_sub(v3_scale(v4_xyz(cols[2]), cols[3].w), v3_scale(v4_xyz(cols[3]), cols[2].w));

    float inv_det = 1.0f / (v3_dot(cross_0_1, sub_3_2) + v3_dot(cross_2_3, sub_1_0));
    cross_0_1 = v3_scale(cross_0_1, inv_det);
    cross_2_3 = v3_scale(cross_2_3, inv_det);
    sub_1_0 = v3_scale(sub_1_0, inv_det);
    sub_3_2 = v3_scale(sub_3_2, inv_det);

    return m4_transpose((M4){ .columns = {
        [0] = v4v(v3_add(v3_cross(v4_xyz(cols[1]), sub_3_2), v3_scale(cross_2_3, cols[1].w)), -v3_dot(v4_xyz(cols[1]), cross_2_3)),
        [1] = v4v(v3_sub(v3_cross(sub_3_2, v4_xyz(cols[0])), v3_scale(cross_2_3, cols[0].w)),  v3_dot(v4_xyz(cols[0]), cross_2_3)),
        [2] = v4v(v3_add(v3_cross(v4_xyz(cols[3]), sub_1_0), v3_scale(cross_0_1, cols[3].w)), -v3_dot(v4_xyz(cols[3]), cross_0_1)),
        [3] = v4v(v3_sub(v3_cross(sub_1_0, v4_xyz(cols[2])), v3_scale(cross_0_1, cols[2].w)),  v3_dot(v4_xyz(cols[2]), cross_0_1)),
    } });
}

static inline M4 m4_look_at(V3 eye, V3 centre, V3 up_direction) {
    V3 forward = v3_norm(v3_sub(centre, eye));
    V3 right   = v3_norm(v3_cross(forward, up_direction));
    V3 up      = v3_cross(right, forward);

    return (M4){ .c = {
        [0][0] =    right.x,
        [0][1] =       up.x,
        [0][2] = -forward.x,

        [1][0] =    right.y,
        [1][1] =       up.y,
        [1][2] = -forward.y,

        [2][0] =    right.z,
        [2][1] =       up.z,
        [2][2] = -forward.z,

        [3][0] = -v3_dot(right,   eye),
        [3][1] = -v3_dot(up,      eye),
        [3][2] =  v3_dot(forward, eye),
        [3][3] =  1.f,
    } };
}

static inline M4 m4_mul_m4(M4 a, M4 b) {
    M4 result = {0};
    for (int col = 0; col < 4; col += 1) for (int row = 0; row < 4; row += 1) {
        f32 sum = 0;
        for (int pos = 0; pos < 4; pos += 1) sum += a.c[pos][row] * b.c[col][pos];
        result.c[col][row] = sum;
    }
    return result;
}

static inline V4 m4_mul_v4(M4 m, V4 v) {
    return (V4){
        .x = v4_dot(v, (V4){ .x = m.c[0][0], .y = m.c[0][1], .z = m.c[0][2], .w = m.c[0][3] }),
        .y = v4_dot(v, (V4){ .x = m.c[1][0], .y = m.c[1][1], .z = m.c[1][2], .w = m.c[1][3] }),
        .z = v4_dot(v, (V4){ .x = m.c[2][0], .y = m.c[2][1], .z = m.c[2][2], .w = m.c[2][3] }),
        .w = v4_dot(v, (V4){ .x = m.c[3][0], .y = m.c[3][1], .z = m.c[3][2], .w = m.c[3][3] }),
    };
}

static inline M4 m4_perspective_projection(f32 fov_vertical_radians, f32 width_over_height, f32 range_near, f32 range_far) {
    f32 cot = 1.f / tanf(fov_vertical_radians / 2.f);
    return (M4){ .c = {
        [0][0] = cot / width_over_height,
        [1][1] = cot,
        [2][2] = (range_near + range_far) / (range_near - range_far),
        [2][3] = -1.f,
        [3][2] = (2.f * range_near * range_far) / (range_near - range_far),
    } };
}

static inline M4 m4_transpose(M4 m) {
    M4 result = {0};
    for (int i = 0; i < 4; i += 1) for (int j = 0; j < 4; j += 1) {
        result.c[i][j] = m.c[j][i];
    }
    return result;
}

#define CPU_DRAW_IMPLEMENTATION
#define CPU_DRAW_ASSERT assert
#define CPU_DRAW_SINF sinf
#define CPU_DRAW_COSF cosf
#define CPU_DRAW_ROUNDF roundf
#define CPU_DRAW_SQRTF sqrtf
#include "base/cpu_draw.h"

static cpu_draw_Font cpu_draw_font_from_bdf(const void *bdf, u64 bdf_size) {
    cpu_draw_Font font = {0};

    bdf_Font bdf_font = {0};
    ensure(bdf_parse(&bdf_font, bdf, bdf_size));
    font.height = bdf_font.size;
    font.ascent = bdf_font.ascent;
    font.descent = bdf_font.descent;
    for (u8 c = ' '; c < 128; c += 1) {
        cpu_draw_Glyph *platform_glyph = &font.glyphs[c - 32];
        bdf_Glyph *bdf_glyph = &bdf_font.glyphs[c - 32];

        platform_glyph->width = bdf_glyph->width;
        platform_glyph->bound_width = bdf_glyph->bound.width;
        platform_glyph->bound_height = bdf_glyph->bound.height;
        platform_glyph->bound_left = bdf_glyph->bound.left;
        platform_glyph->bound_bottom = bdf_glyph->bound.bottom;

        _Static_assert((sizeof platform_glyph->bytes == sizeof bdf_glyph->bytes), "");
        memcpy(platform_glyph->bytes, bdf_glyph->bytes, sizeof bdf_glyph->bytes);
    }

    // boxed question mark mapped to codepoint 127, for unknown codepoints
    cpu_draw_Glyph *g = &font.glyphs[127 - 32];
    *g = font.glyphs['?' - 32];
    for (u64 byte = 0; byte < CPU_DRAW_GLYPH_MAX_WIDTH_OVER_8; byte += 1) {
        g->bytes[byte] = 0xff;
    }
    for (u64 byte = 0; byte < CPU_DRAW_GLYPH_MAX_WIDTH_OVER_8; byte += 1) {
        g->bytes[(u64)(g->bound_height - 1) * CPU_DRAW_GLYPH_MAX_WIDTH_OVER_8 + byte] = 0xff;
    }
    for (u64 row = 0; row < (u64)g->bound_height; row += 1) {
        g->bytes[row * CPU_DRAW_GLYPH_MAX_WIDTH_OVER_8] |= 0x80;
    }
    for (u64 row = 0; row < (u64)g->bound_height; row += 1) {
        u64 last_pixel = (u64)g->bound_width - 1;
        u8 *bits = &g->bytes[row * CPU_DRAW_GLYPH_MAX_WIDTH_OVER_8 + last_pixel / 8];
        *bits |= 1 << (7 - (last_pixel & 7));
    }

    return font;
}

static V2 cpu_draw_platform_measure_text(const cpu_draw_Font *font, String text, f32 font_size) {
    assert(font->height > 0);
    // TODO(felix)
    assert(font->height == (int)roundf(font_size));

    int width = 0;
    cpu_draw_text((cpu_draw_Surface){0}, font, 0, 0, 0, text.data, (int)text.count, &width);
    V2 result = {
        .x = (f32)width,
        .y = (f32)font->ascent,
    };
    return result;
}

static u32 argb_from_rgba(u32 rgba) {
    u32 argb = (rgba >> 8) | (rgba << 24);
    return argb;
}

static u32 abgr_from_rgba(u32 rgba) {
    u32 r = rgba >> 24;
    u32 g = (rgba >> 16) & 0xff;
    u32 b = (rgba >> 8) & 0xff;
    u32 a = rgba & 0xff;
    return (a << 24) | (b << 16) | (g << 8) | r;
}

static u32 pixel_from_rgba(u32 rgba) {
    u32 pixel = 0;
    switch (BASE_OS) {
        case BASE_OS_WINDOWS: pixel = argb_from_rgba(rgba); break;
        case BASE_OS_WASM: pixel = abgr_from_rgba(rgba); break;
        default: unreachable;
    }
    return pixel;
}

#define BDF_ASSERT assert
#define BDF_IMPLEMENTATION
#include "base/bdf.h"

static f64 f64_from_string(String s) {
    f64 result = 0;
    if (s.count == 0) return result;

    bool is_negative = s.data[0] == '-';
    s.data += is_negative;
    s.count -= is_negative;

    u64 decimal_index = 0;
    while (decimal_index < s.count && s.data[decimal_index] != '.') decimal_index += 1;
    String int_string = string_range(s, 0, decimal_index);
    f64 int_part = (f64)int_from_string_base(int_string, 10);

    f64 decimal_part = 0;
    if (decimal_index != s.count) {
        String decimal_string = string_range(s, decimal_index + 1, s.count);
        u64 digit_count = decimal_string.count;
        for (i64 i = (i64)decimal_string.count; i >= 0; i -= 1) {
            bool trailing_zero = decimal_string.data[i] == 0;
            digit_count -= trailing_zero;
            if (!trailing_zero) break;
        }
        decimal_part = (f64)int_from_string_base(decimal_string, 10);
        for (u64 i = 0; i < digit_count; i += 1) decimal_part *= 0.1;
    }

    result = int_part + decimal_part;
    result *= (f64)is_negative * -2.0 + 1.0;
    return result;
}

static i64 int_from_string_base(String s, u64 base) {
    static const u8 decimal_from_hex_digit_table[128] = {
        ['0'] = 0x0, ['1'] = 0x1, ['2'] = 0x2, ['3'] = 0x3,
        ['4'] = 0x4, ['5'] = 0x5, ['6'] = 0x6, ['7'] = 0x7,
        ['8'] = 0x8, ['9'] = 0x9, ['A'] = 0xa, ['B'] = 0xb,
        ['C'] = 0xc, ['D'] = 0xd, ['E'] = 0xe, ['F'] = 0xf,
        ['a'] = 0xa, ['b'] = 0xb, ['c'] = 0xc, ['d'] = 0xd,
        ['e'] = 0xe, ['f'] = 0xf,
    };

    i64 sign = 1;
    if (s.data[0] == '-') {
        sign = -1;
        s.data += 1;
        s.count -= 1;
    }

    i64 result = 0, magnitude = (i64)s.count;
    for (u64 i = 0; i < s.count; i += 1, magnitude -= 1) {
        result *= (i64)base;
        u64 digit = decimal_from_hex_digit_table[(u8)s.data[i]];
        if (digit >= base) {
            log_error("digit '%c' is invalid in base %llu", s.data[i], base);
            return 0;
        }
        result += digit;
    }

    result *= sign;
    return result;
}

#define STR_IMPLEMENTATION
#define STR_ASSERT assert
#include "base/str.h"

#define UTF_IMPLEMENTATION
#include "base/utf.h"

#define SMALL_SNPRINTF_IMPLEMENTATION
#define SMALL_SNPRINTF_ASSERT assert
#define SMALL_SNPRINTF_UNKNOWN_FORMAT_HANDLER small_snprintf_unknown_format_handler
static int small_snprintf_unknown_format_handler(char *buffer, size_t buffer_size, const char *format, size_t *format_index, va_list *arguments) {
    int written = 0;

    char type = format[*format_index];
    switch (type) {
        case 'S': {
            String s = va_arg(*arguments, String);
            for (size_t i = 0; (size_t)written + i + 1 < buffer_size && i < s.count; i += 1) {
                buffer[written + (int)i] = s.data[i];
            }
            written += (int)s.count;
        } break;
        default: panic("invalid format specifier '%c'", type);
    }

    return written;
}
#include "base/small_snprintf.h"

#if BASE_OS == BASE_OS_WASM
    extern double js_performance_now(void);
    #define TIME_JS_PERFORMANCE_NOW js_performance_now
#endif
#define TIME_IMPLEMENTATION
#include "base/time.h"

#define PRNG_IMPLEMENTATION
#define PRNG_ASSERT assert
#include "base/prng.h"

static u64 cstring_copy(char *buffer, const char *cstring) {
    u64 length = cstring_length(cstring);
    memcpy(buffer, cstring, length);
    return length;
}

static const char *cstring_from_string(Arena *arena, String string) {
    char *cstring = arena_make(arena, size_t_from_u64(string.count + 1), char);
    for (u64 i = 0; i < string.count; i += 1) cstring[i] = (char)string.data[i];
    cstring[string.count] = '\0';
    return cstring;
}

static const char *cstring_print(Arena *arena, const char *format, ...) {
    String_Builder builder = {0};
    string_builder_reserve_unused(arena, &builder, cstring_length(format));

    va_list arguments;
    va_start(arguments, format);
    string_builder_print_(arena, &builder, format, arguments);
    va_end(arguments);

    string_builder_null_terminate(arena, &builder);
    return builder.data;
}

// Only bases <= 10
static String string_from_int_base(Arena *arena, i64 _num, u8 base) {
    i64 num = _num;

    u64 count = num < 0 ? 1 : 0;
    do {
        num /= base;
        count += 1;
    } while (num > 0);

    char *string = arena_make(arena, size_t_from_u64(count), char);

    i64 first_digit_index = 0;
    if (num < 0) {
        string[0] = '-';
        first_digit_index = 1;
    }

    num = _num;
    for (i64 i = (i64)count - 1; i >= first_digit_index; i -= 1) {
        string[i] = (char)((num % base) + '0');
        num /= base;
    }

    String result = { .data = string, .count = count };
    return result;
}

static String string_print(Arena *arena, const char *fmt, ...) {
    va_list arguments;
    va_start(arguments, fmt);
    String result = string_print_(arena, fmt, arguments);
    va_end(arguments);
    return result;
}

static String string_print_(Arena *arena, const char *fmt, va_list arguments) {
    String_Builder builder = {0};
    string_builder_print_(arena, &builder, fmt, arguments);
    return string_builder_string(builder);
}

static void string_builder_print(Arena *arena, String_Builder *builder, const char *fmt_c, ...) {
    va_list arguments;
    va_start(arguments, fmt_c);
    string_builder_print_(arena, builder, fmt_c, arguments);
    va_end(arguments);
}

static void string_builder_reserve_unused(Arena *arena, String_Builder *builder, u64 unused_count) {
    builder->data = arena_array_reserve(arena, builder->data, builder->count, &builder->capacity, builder->count + unused_count);
}

static void string_builder_push(String_Builder *builder, String string) {
    assert(builder->count + string.count <= builder->capacity);
    memcpy(&builder->data[builder->count], string.data, string.count);
    builder->count += string.count;
}

static void string_builder_print_(Arena *arena, String_Builder *builder, const char *fmt_c, va_list arguments) {
    u64 size_guess = cstring_length(fmt_c) * 8;
    string_builder_reserve_unused(arena, builder, size_guess);

    char *buffer = &builder->data[builder->count];
    u64 buffer_size = builder->capacity - builder->count;
    if (BUILD_DEBUG) memset(buffer, POISON_BYTE, buffer_size);

    va_list arguments_1;
    va_copy(arguments_1, arguments);
    int bytes_printed = small_vsnprintf(buffer, size_t_from_u64(buffer_size), fmt_c, arguments_1);
    va_end(arguments_1);

    if ((u64)bytes_printed >= buffer_size) {
        u64 needed_unused = (u64)bytes_printed + 1; // for null terminator
        string_builder_reserve_unused(arena, builder, needed_unused);

        buffer = &builder->data[builder->count];
        buffer_size = builder->capacity - builder->count;

        assert(buffer_size >= needed_unused);
        if (BUILD_DEBUG) memset(buffer, POISON_BYTE, buffer_size);

        va_list arguments_2;
        va_copy(arguments_2, arguments);
        bytes_printed = small_vsnprintf(buffer, size_t_from_u64(buffer_size), fmt_c, arguments_2);
        va_end(arguments_2);

        assert((u64)bytes_printed < buffer_size);
    }

    builder->count += (u64)bytes_printed;
}

static String string_builder_string(String_Builder builder) {
    String result = { .data = builder.data, .count = builder.count };
    return result;
}

static inline void string_builder_null_terminate(Arena *arena, String_Builder *builder) {
    // Avoid push(0) because we don't want to increase the string length
    string_builder_reserve_unused(arena, builder, 1);
    builder->data[builder->count] = 0;
}

#if BASE_OS == BASE_OS_WASM
    static fs_File fs_file_open_and_get_size(const char *path, fs_File_Flags flags, unsigned long long *size) {
        (void)path;
        (void)flags;
        (void)size;
        panic("unimplemented for WASM");
        return (fs_File){0};
    }
    static unsigned long long fs_read_entire_file(fs_File file, char *buffer, unsigned long long size) {
        (void)file;
        (void)buffer;
        (void)size;
        panic("unimplemented for WASM");
        return 0;
    }
    static _Bool fs_close(fs_File file) {
        (void)file;
        panic("unimplemented for WASM");
        return 0;
    }
    static fs_File fs_open(const char *path, fs_File_Flags flags) {
        (void)path;
        (void)flags;
        panic("unimplemented for WASM");
        return (fs_File){0};
    }
    static _Bool fs_file_write(fs_File file, const void *bytes, unsigned long long size) {
        (void)file;
        (void)bytes;
        (void)size;
        panic("unimplemented for WASM");
        return 0;
    }
    static _Bool fs_exists(const char *path) {
        (void)path;
        panic("unimplemented for WASM");
        return 0;
    }
    static _Bool fs_make_folder(const char *relative_path) {
        (void)relative_path;
        panic("unimplemented for WASM");
        return 0;
    }
    static _Bool fs_remove_file(const char *relative_path) {
        (void)relative_path;
        panic("unimplemented for WASM");
        return 0;
    }
    static _Bool fs_absolute_path(const char *path, char *buffer, unsigned long long buffer_size) {
        (void)path;
        (void)buffer;
        (void)buffer_size;
        panic("unimplemented for WASM");
        return 0;
    }
#else
    #define FS_IMPLEMENTATION
    #define FS_ASSERT assert
    #include "base/filesystem.h"
#endif

extern const char embedded_files[];

static String base_embedded_files_get(String name) {
    String result = {0};

    u64 file_count;
    memcpy(&file_count, embedded_files, sizeof file_count);

    for (u64 i = 0; i < file_count && result.data == 0; i += 1) {
        Embedded_File_Entry entry;
        memcpy(&entry, embedded_files + sizeof file_count + i * sizeof entry, sizeof entry);

        String entry_name = string_from_cstring(embedded_files + entry.name_offset);
        if (string_equals(entry_name, name)) result = (String){ .data = embedded_files + entry.data_offset, .count = entry.data_size };
    }

    return result;
}

static String os_read_entire_file(Arena *arena, const char *path, bool *ok) {
    String result = {0};
    u64 size = 0;
    fs_File file = fs_file_open_and_get_size(path, fs_File_Flag_READ, &size);
    bool read_ok = file.handle != 0;

    if (read_ok && size > 0) {
        char *data = arena_make(arena, size_t_from_u64(size + 1), char);
        read_ok = fs_read_entire_file(file, data, size) == size;
        data[size] = 0;
        if (read_ok) result = (String){ .data = data, .count = size };
    }

    fs_close(file);
    *ok = read_ok;
    return result;
}

static bool os_write_entire_file(const char *path, const void *data, u64 count) {
    bool ok = false;

    #if BASE_OS == BASE_OS_WASM
    {
        void js_file_write(const char *name, const void *bytes, u32 count);
        assert(count <= UINT32_MAX);
        js_file_write(path, data, (u32)count);
        ok = true;
    }
    #else
    {
        fs_File file = fs_open(path, fs_File_Flag_WRITE);
        ok = fs_file_write(file, data, count);
        fs_close(file);
    }
    #endif

    assert(ok);
    if (!ok) log_error("unable to write %llu bytes to path '%s'", count, path);
    return ok;
}

static void log_internal_with_location(const char *file, u64 line, const char *func, const char *format, ...) {
    va_list arguments;
    va_start(arguments, format);
    log_internal_with_location_v(file, line, func, format, arguments);
    va_end(arguments);
    print("\n");
}

static void log_internal_with_location_v(const char *file, u64 line, const char *func, const char *format, va_list arguments) {
    print_(format, arguments);
    print("\n");

    #if BUILD_DEBUG
    {
        #if BASE_OS == BASE_OS_WINDOWS
        {
            print("%s(%llu,1): note: in %s()", file, line, func);
        }
        #else
        {
            print("%s:%llu: in %s()", file, line, func);
        }
        #endif
    }
    #else
    {
        (void)(file); (void)(line); (void)(func);
    }
    #endif
}

static u16 *os_utf16_from_utf8(Arena *arena, const char *utf8, u64 count) {
    size_t source_count = size_t_from_u64(count);
    size_t unit_count = utf16_from_utf8(0, 0, utf8, source_count).destination_units_written;
    u16 *utf16 = arena_make(arena, unit_count + 1, u16);
    utf16_from_utf8(utf16, unit_count, utf8, source_count);
    utf16[unit_count] = 0;
    return utf16;
}

static Os_Process_Result os_process_run(Arena *arena, const char **arguments, const char *directory, Os_Process_Flags flags) {
    u32 exit_code = 1;

    bool capture_output = !!(flags & Os_Process_Flag_CAPTURE_OUTPUT);
    u64 output_capacity = 0;
    char *output_data = 0;
    u64 output_count = 0;
    u64 output_read_size = 4096;

    if (flags & Os_Process_Flag_PRINT_COMMAND_BEFORE_RUNNING) {
        if (directory != 0) print("[./%s] ", directory);
        for (const char **a = arguments; *a != 0; a += 1) print("%s ", *a);
        print("\n");
    }

    f64 start_time = 0;
    if (flags & Os_Process_Flag_PRINT_EXECUTION_TIME) {
        start_time = time_relative_timestamp();
    }

    #if BASE_OS == BASE_OS_WINDOWS
    {
        Scratch scratch = scratch_begin(arena);
        const u16 *application_name = 0;

        u64 capacity = 0;
        for (const char **a = arguments; *a != 0; a += 1) {
            u64 escaped_length_at_most = 2 * cstring_length(*a);
            u64 enclosing_quote_count = 2;
            u64 separator_count = 1;
            capacity += escaped_length_at_most + enclosing_quote_count + separator_count;
        }

        char *command_line = arena_make(scratch.arena, capacity, char);
        u64 command_length = 0;

        for (const char **a = arguments; *a != 0; a += 1) {
            const char *argument = *a;
            if (a > arguments) command_line[command_length++] = ' ';

            bool needs_quotes = argument[0] == 0;
            for (const char *c = argument; *c != 0; c += 1) needs_quotes |= *c == ' ' || *c == '\t' || *c == '\n' || *c == '\v' || *c == '"';

            if (!needs_quotes) command_length += cstring_copy(command_line + command_length, argument);
            else {
                command_line[command_length++] = '"';
                const char *c = argument;
                bool at_end = false;
                while (!at_end) {
                    u64 backslash_count = 0;
                    while (*c == '\\') {
                        backslash_count += 1;
                        c += 1;
                    }

                    at_end = *c == 0;
                    bool quote_follows = *c == '"';
                    u64 written_backslashes = (at_end || quote_follows) ? 2 * backslash_count + quote_follows : backslash_count;
                    for (u64 i = 0; i < written_backslashes; i += 1) command_line[command_length++] = '\\';

                    if (!at_end) {
                        command_line[command_length++] = *c;
                        c += 1;
                    }
                }
                command_line[command_length++] = '"';
            }
        }

        u16 *command_line_utf16 = os_utf16_from_utf8(scratch.arena, command_line, command_length);
        u16 *directory_utf16 = directory == 0 ? 0 : os_utf16_from_utf8(scratch.arena, directory, cstring_length(directory));

        void *output_read_pipe = 0;
        void *output_write_pipe = 0;
        bool pipe_ok = true;
        if (capture_output) {
            enum { HANDLE_FLAG_INHERIT = 0x1 };
            struct _SECURITY_ATTRIBUTES pipe_attributes = { .nLength = sizeof pipe_attributes, .bInheritHandle = true };
            pipe_ok = CreatePipe(&output_read_pipe, &output_write_pipe, &pipe_attributes, 0);
            pipe_ok = pipe_ok && SetHandleInformation(output_read_pipe, HANDLE_FLAG_INHERIT, 0);
            if (!pipe_ok) log_error("unable to create an output pipe for '%s' (error %u)", arguments[0], RtlGetLastWin32Error());
        }

        struct _SECURITY_ATTRIBUTES *process_attributes = 0;
        struct _SECURITY_ATTRIBUTES *thread_attributes = 0;
        int inherit_handles = capture_output;
        u32 creation_flags = 0;
        void *environment = 0;
        STARTUPINFOW startup_info = { .cb = sizeof startup_info };
        PROCESS_INFORMATION process_info = {0};

        if (capture_output) {
            enum { STARTF_USESTDHANDLES = 0x100 };
            RTL_USER_PROCESS_PARAMETERS *parameters = RtlGetCurrentPeb()->ProcessParameters;
            startup_info.dwFlags = STARTF_USESTDHANDLES;
            startup_info.hStdInput = parameters->StandardInput;
            startup_info.hStdOutput = output_write_pipe;
            startup_info.hStdError = parameters->StandardError;
        }

        int ok = pipe_ok && CreateProcessW(
            application_name,
            command_line_utf16,
            process_attributes,
            thread_attributes,
            inherit_handles,
            creation_flags,
            environment,
            directory_utf16,
            &startup_info,
            &process_info
        );
        scratch_end(scratch);
        if (output_write_pipe != 0) NtClose(output_write_pipe);

        if (pipe_ok && !ok) {
            log_error("unable to start process '%s'; CreateProcessW() failed with error %u", arguments[0], RtlGetLastWin32Error());
        } else if (ok) {
            for (bool reading = capture_output; reading;) {
                output_data = arena_array_reserve(arena, output_data, output_count, &output_capacity, output_count + output_read_size);
                IO_STATUS_BLOCK status_block = {0};
                unsigned long unused_capacity = (unsigned long)MIN(output_capacity - output_count, UINT32_MAX);
                long read_status = NtReadFile(output_read_pipe, 0, 0, 0, &status_block, output_data + output_count, unused_capacity, 0, 0);
                reading = read_status >= 0 && status_block.Information > 0;
                if (reading) output_count += status_block.Information;
            }

            int wait_status = NtWaitForSingleObject(process_info.hProcess, false, 0);

            PROCESS_BASIC_INFORMATION information = {0};
            enum { ProcessBasicInformation = 0 };
            int query_status = wait_status >= 0 ? NtQueryInformationProcess(process_info.hProcess, ProcessBasicInformation, &information, sizeof information, 0) : wait_status;
            if (query_status >= 0) exit_code = (u32)information.ExitStatus;
            else log_error("unable to get the exit code of '%s' (status 0x%x)", arguments[0], (u32)query_status);

            NtClose(process_info.hProcess);
            NtClose(process_info.hThread);
        }
        if (output_read_pipe != 0) NtClose(output_read_pipe);
    }
    #elif BASE_OS & BASE_OS_ANY_POSIX
    {
        int output_pipe[2] = { -1, -1 };
        enum { PIPE_READ_END = 0, PIPE_WRITE_END = 1, STANDARD_OUTPUT = 1 };
        bool pipe_ok = !capture_output || pipe(output_pipe) == 0;
        if (!pipe_ok) log_error("unable to create an output pipe for '%s' (errno=%d)", arguments[0], errno);

        pid_t pid = pipe_ok ? fork() : -1;
        if (pipe_ok && pid < 0) {
            log_error("unable to fork() to start process '%s' (errno=%d)", arguments[0], errno);
        } else if (pid == 0) { // child
            if (capture_output) {
                dup2(output_pipe[PIPE_WRITE_END], STANDARD_OUTPUT);
                close(output_pipe[PIPE_READ_END]);
                close(output_pipe[PIPE_WRITE_END]);
            }

            bool directory_ok = directory == 0;
            if (directory != 0) {
                int result = chdir(directory);
                directory_ok = result == 0;
                if (result != 0) log_error("unable to chdir('%s') (errno=%d)", directory, errno);
            }

            if (directory_ok) {
                execvp(arguments[0], (char *const *)arguments);
                log_error("unable to execvp '%s' (errno=%d)", arguments[0], errno);
            }

            os_exit(1);
        } else if (pid > 0) { // parent
            if (capture_output) close(output_pipe[PIPE_WRITE_END]);
            for (bool reading = capture_output; reading;) {
                output_data = arena_array_reserve(arena, output_data, output_count, &output_capacity, output_count + output_read_size);
                ssize_t read_count = read(output_pipe[PIPE_READ_END], output_data + output_count, output_capacity - output_count);
                reading = read_count > 0;
                if (reading) output_count += (u64)read_count;
            }
            if (capture_output) close(output_pipe[PIPE_READ_END]);

            int status = 0;
            pid_t result = waitpid(pid, &status, 0);
            bool wait_ok = result != -1;
            if (result == -1) log_error("unable to wait on '%s' (errno=%d)", arguments[0], errno);

            if (wait_ok) {
                if (WIFEXITED(status)) exit_code = (u32)WEXITSTATUS(status);
                else if (WIFSIGNALED(status)) exit_code = 128 + WTERMSIG(status);
            }
        }
    }
    #else
    {
        (void)arena;
        (void)capture_output;
        (void)output_capacity;
        (void)output_read_size;
        panic("unimplemented");
    }
    #endif

    if (flags & (Os_Process_Flag_PRINT_EXECUTION_TIME | Os_Process_Flag_PRINT_EXIT_CODE)) {
        print("'%s' exited", arguments[0]);

        if (flags & Os_Process_Flag_PRINT_EXECUTION_TIME) {
            f64 end_time = time_relative_timestamp();
            f64 seconds = time_seconds_between_relative_timestamps(start_time, end_time);
            print(" after %.2lf seconds", seconds);
        }

        if (flags & Os_Process_Flag_PRINT_EXIT_CODE) print(": %u", exit_code);

        print("\n");
    }

    Os_Process_Result result = { .exit_code = exit_code, .output = { .data = output_data, .count = output_count } };
    return result;
}

static void os_write_console(const char *bytes, u64 count) {
    // TODO(felix): stderr support
    // NOTE(felix): can't use assert in this function because panic() will call os_write_console, so we'll end up with a recursively failing assert and stack overflow. Instead, use `if (!condition) { breakpoint; abort(); }`
    #if BASE_OS == BASE_OS_WINDOWS
    {
        #if WINDOWS_SUBSYSTEM == WINDOWS_SUBSYSTEM_WINDOWS
        {
            // TODO(felix): Make this arena thread-safe once base supports multiple threads.
            static Arena debug_output_arena = {0};
            Scratch scratch = scratch_begin(&debug_output_arena);
            OutputDebugStringW(os_utf16_from_utf8(scratch.arena, bytes, count));
            scratch_end(scratch);
        }
        #else
        {
            void *output_handle = RtlGetCurrentPeb()->ProcessParameters->StandardOutput;
            void *invalid_handle_value = (void *)(long long)-1;
            if (output_handle == 0 || output_handle == invalid_handle_value) { breakpoint; os_abort(); }

            IO_STATUS_BLOCK device_status_block = {0};
            FILE_FS_DEVICE_INFORMATION device = {0};
            enum { FileFsDeviceInformation = 4, FILE_DEVICE_CONSOLE = 0x50 };
            int device_status = NtQueryVolumeInformationFile(output_handle, &device_status_block, &device, sizeof device, FileFsDeviceInformation);
            bool is_console = device_status >= 0 && device.DeviceType == FILE_DEVICE_CONSOLE;

            if (count > UINT32_MAX) { breakpoint; os_abort(); }

            if (is_console) {
                // TODO(felix): Make this arena thread-safe once base supports multiple threads.
                static Arena console_arena = {0};
                Scratch scratch = scratch_begin(&console_arena);
                {
                    u64 unit_count = utf16_from_utf8(0, 0, bytes, count).destination_units_written;
                    u16 *units = arena_make(scratch.arena, size_t_from_u64(unit_count), u16);
                    utf16_from_utf8(units, unit_count, bytes, count);

                    unsigned long console_units_written = 0;
                    int ok = WriteConsoleW(output_handle, units, (unsigned)unit_count, &console_units_written, 0);
                    if (!ok || console_units_written != unit_count) { breakpoint; os_abort(); }
                }
                scratch_end(scratch);
            } else {
                IO_STATUS_BLOCK status_block = {0};
                int status = NtWriteFile(output_handle, 0, 0, 0, &status_block, bytes, (unsigned)count, 0, 0);

                enum { STATUS_PENDING = 0x103 };
                if (status == STATUS_PENDING) {
                    status = NtWaitForSingleObject(output_handle, false, 0);
                    if (status >= 0) status = status_block.Status;
                }
                if (status < 0 || status_block.Information != count) { breakpoint; os_abort(); }
            }
        }
        #endif
    }
    #elif BASE_OS & BASE_OS_ANY_POSIX
    {
        int stdout_handle = 1;
        for (u64 total_written = 0; total_written < count;) {
            i64 bytes_written = write(stdout_handle, bytes + total_written, count - total_written);

            enum { os_write_console__EINTR = 4 };
            bool interrupted = bytes_written < 0 && errno == os_write_console__EINTR;
            if (bytes_written <= 0 && !interrupted) { breakpoint; os_abort(); }
            if (bytes_written > 0) total_written += (u64)bytes_written;
        }
    }
    #elif BASE_OS == BASE_OS_WASM
    {
        if (count > UINT32_MAX) { breakpoint; os_abort(); }
        extern void os_write_console_js(const char *bytes, u32 count);
        os_write_console_js(bytes, (u32)count);
    }
    #else
        #error "unimplemented"
    #endif
}

static void print(const char *format, ...) {
    va_list arguments;
    va_start(arguments, format);
    print_(format, arguments);
    va_end(arguments);
}

static void print_(const char *format, va_list arguments) {
    static Arena print_arena;
    Scratch scratch = scratch_begin(&print_arena);

    String_Builder output = {0};
    string_builder_reserve_unused(scratch.arena, &output, cstring_length(format));
    string_builder_print_(scratch.arena, &output, format, arguments);
    string_builder_null_terminate(scratch.arena, &output);

    #if (BASE_OS == BASE_OS_WINDOWS) && WINDOWS_SUBSYSTEM == WINDOWS_SUBSYSTEM_CONSOLE && BUILD_DEBUG
        OutputDebugStringW(os_utf16_from_utf8(scratch.arena, (const char *)output.data, output.count));
    #endif

    os_write_console(output.data, output.count);
    scratch_end(scratch);
}

static void os_show_fatal_error(const char *file, u64 line, const char *function, const char *format, ...) {
    #if (BASE_OS == BASE_OS_WINDOWS) && WINDOWS_SUBSYSTEM == WINDOWS_SUBSYSTEM_WINDOWS
    {
        static Arena fatal_error_arena = {0};
        Scratch scratch = scratch_begin(&fatal_error_arena);

        String_Builder message = {0};
        va_list arguments;
        va_start(arguments, format);
        string_builder_print_(scratch.arena, &message, format, arguments);
        va_end(arguments);
        string_builder_print(scratch.arena, &message, "\n\n%s(%llu) in %s()", file, line, function);

        const char caption[] = "panic";
        u16 *caption_utf16 = os_utf16_from_utf8(scratch.arena, caption, sizeof caption - 1);
        u16 *message_utf16 = os_utf16_from_utf8(scratch.arena, (const char *)message.data, message.count);

        enum { MB_ICONERROR = 0x10, MB_TASKMODAL = 0x2000, MB_SETFOREGROUND = 0x10000 };
        MessageBoxW(0, message_utf16, caption_utf16, MB_ICONERROR | MB_TASKMODAL | MB_SETFOREGROUND);

        scratch_end(scratch);
    }
    #else
    {
        (void)file;
        (void)line;
        (void)function;
        (void)format;
    }
    #endif
}

#define CLEX_IMPLEMENTATION
#define CLEX_ASSERT assert
#include "base/clex.h"

static u32 build_macros_find(const Build_Macros *macros, String name) {
    u32 hash = ds_hash_fnv1a(name.data, size_t_from_u64(name.count));
    u32 slot = hash;

    for (bool searching = true; searching;) {
        slot = ds_hash_step_index(hash, BUILD_MACROS_EXPONENT, slot);
        String slot_name = macros->slots[slot].name;
        searching = slot_name.data != 0 && !string_equals(slot_name, name);
    }

    return slot;
}

static u32 build_macros_insert(Build_Macros *macros, String name, bool *ok) {
    enum { MAXIMUM_LOAD_PERCENT = 70 };
    u32 slot = build_macros_find(macros, name);
    bool is_new = macros->slots[slot].name.data == 0;
    bool has_space = !is_new || (macros->used + 1) * 100 <= BUILD_MACROS_CAPACITY * MAXIMUM_LOAD_PERCENT;

    if (has_space && is_new) {
        macros->slots[slot].name = name;
        macros->used += 1;
    }
    if (!has_space) log_error("more than %d macros; the metaprogram's table is full", BUILD_MACROS_CAPACITY * MAXIMUM_LOAD_PERCENT / 100);

    *ok = has_space;
    return slot;
}

#define build_unknown_reason(arena, macros, description, cause) ((macros)->collect_unknown_reasons ? build_unknown_reason_((arena), (macros), (description), (cause)) : BUILD_UNKNOWN_REASON_NOT_COLLECTED)
enum { BUILD_UNKNOWN_REASON_NOT_COLLECTED = 1 };
static Build_Unknown_Reason_Id build_unknown_reason_(Arena *arena, Build_Macros *macros, const char *description, Build_Unknown_Reason_Id cause) {
    Build_Unknown_Reasons *reasons = &macros->reasons;
    reasons->items = arena_array_reserve(arena, reasons->items, reasons->count, &reasons->capacity, reasons->count + 2);

    bool id_zero_is_reserved = reasons->count == 0;
    reasons->count += id_zero_is_reserved;

    Build_Unknown_Reason_Id id = (Build_Unknown_Reason_Id)reasons->count;
    reasons->items[id] = (Build_Unknown_Reason){ .description = description, .cause = cause };
    reasons->count += 1;
    return id;
}

static Build_Macro build_macros_get(Arena *arena, Build_Macros *macros, String name) {
    u32 slot = build_macros_find(macros, name);
    bool starts_with_underscore_and_uppercase = name.count >= 2 && name.data[0] == '_' && 'A' <= name.data[1] && name.data[1] <= 'Z';
    bool is_reserved_name = string_starts_with(name, S("__")) || starts_with_underscore_and_uppercase;
    bool is_unlisted_reserved_name = macros->slots[slot].name.data == 0 && is_reserved_name;

    Build_Macro macro = macros->slots[slot].macro;

    if (is_unlisted_reserved_name) {
        macro.state = Build_Macro_State_UNKNOWN;
        macro.unknown_reason = build_unknown_reason(arena, macros, cstring_print(arena, "`%S` is a reserved identifier", name), 0);
    }

    return macro;
}

static String base_string_from_clex_data(clex_Data data) {
    return string_from_pointers(data.start, data.end);
}

#define OBJECT_IMPLEMENTATION
#define OBJECT_ASSERT assert
#include "base/object_files.h"

#define SERIAL_IMPLEMENTATION
#define SERIAL_ASSERT assert
#include "base/serial.h"

#define BUILD_FLAGS_MAX 32

static const char *build_compiler_initial_command[Build_Compiler_COUNT][BUILD_FLAGS_MAX] = {
    [Build_Compiler_MSVC] = {
        "cl",
        "-c",
        "-nologo",
        "-FC",
        "-diagnostics:column",
        "-std:c11",
        "-Oi",
        0,
    },
    [Build_Compiler_CLANG] = {
        "clang",
        "-c",
        "-std=c11",
        "-fdiagnostics-absolute-paths",
        "-ferror-limit=1",
        "-pedantic",
        "-fno-strict-aliasing",
        0,
    },
};

static const char *build_linker_initial_command[Build_Linker_COUNT][BASE_OS_COUNT][BUILD_FLAGS_MAX] = {
    [Build_Linker_MSVC][BASE_OS_WINDOWS] = {
        "link",
        "-nologo",
        "-machine:X64",
        "-incremental:no",
        "-fixed",
        0,
    },
    [Build_Linker_CLANG][BASE_OS_WINDOWS] = {
        "clang",
        "-Xlinker", "-machine:X64",
        "-Xlinker", "-incremental:no", // incremental PDBs are buggy
        0,
    },
    [Build_Linker_CLANG][BASE_OS_MACOS] = {
        "clang",
        // TODO(felix): Move to META(framework "") directive (see what the Odin syntax is).
        // "-framework", "CoreFoundation",
        // "-framework", "Metal",
        // "-framework", "Cocoa",
        // "-framework", "Metalkit",
        // "-framework", "Quartz",
        // "-framework", "AudioToolbox",
        0,
    },
    [Build_Linker_CLANG][BASE_OS_IOS_DEVICE] = {
        "clang",
        0,
    },
    [Build_Linker_CLANG][BASE_OS_IOS_SIMULATOR] = {
        "clang",
        0,
    },
    [Build_Linker_CLANG][BASE_OS_WASM] = {
        "clang",
        "--target=wasm32",
        "-nostdlib",
        "-Wl,--no-entry",
        "-Wl,--allow-undefined", // NOTE(felix): consider --allow-undefined-file for explicit whitelist
        "-Wl,--stack-first",
        "-Wl,-z,stack-size=1048576", // NOTE(felix): 1mb matches windows. I ran into stack overflow on macOS but not on Windows for some reason--on the same project, in the same browser
        0,
    },
};

static const char *build_compiler_mode_flags[Build_Mode_COUNT][Build_Compiler_COUNT][BUILD_FLAGS_MAX] = {
    [Build_Mode_DEBUG] = {
        [Build_Compiler_MSVC] = { "-MTd", "-Z7", 0 },
        // NOTE(felix): With clang for wasm, ubsan traps with no stack trace, which at the stage in WASM platform development I'm at while writing this, gets in the way of the stack traces for my `panic()`s in C code.
        // TODO(felix): Make this configurable - the sanitisers are helpful too.
        [Build_Compiler_CLANG] = {
            "-g3",
            "-fsanitize=undefined",
            "-fsanitize-trap",
            "-O0",
            0,
        },
    },
    [Build_Mode_RELEASE] = {
        [Build_Compiler_MSVC]  = { "-MT", "-O2", 0 },
        [Build_Compiler_CLANG] = { "-O3", 0 },
    },
};

static const char *build_compiler_extra_errors[Build_Compiler_COUNT][BUILD_FLAGS_MAX] = {
    [Build_Compiler_MSVC] = { "-W4", "-WX", 0 },
    [Build_Compiler_CLANG] = {
        // https://clang.llvm.org/docs/DiagnosticsReference.html
        "-Wall",
        "-Werror",
        "-Wextra",
        "-Wshadow",
        "-Wconversion",
        "-Wfloat-equal",
        "-Wswitch-default",
        "-Wundef",
        "-Wshift-overflow",
        0,
    },
};

static const char *build_compiler_disabled_errors[Build_Compiler_COUNT][BUILD_FLAGS_MAX] = {
    [Build_Compiler_MSVC] = {
        "-wd4127", // conditional expression is constant
    },
    [Build_Compiler_CLANG] = {
        "-Wno-dollar-in-identifier-extension", // for raddbg_type_view
        "-Wno-unused-function",
        "-Wno-assume",
        "-Wno-deprecated-declarations",
        "-Wno-overlength-strings",
    },
};

static const char *build_compiler_out[Build_Compiler_COUNT] = {
    [Build_Compiler_MSVC] = "-Fo:",
    [Build_Compiler_CLANG] = "-o",
};

static const char *build_object_extension[BASE_OS_COUNT] = {
    [BASE_OS_WINDOWS]       = "obj",
    [BASE_OS_MACOS]         = "o",
    [BASE_OS_IOS_DEVICE]    = "o",
    [BASE_OS_IOS_SIMULATOR] = "o",
    [BASE_OS_WASM]          = "o",
};

static const char *build_binary_extension[BASE_OS_COUNT] = {
    [BASE_OS_WINDOWS]       = "exe",
    [BASE_OS_MACOS]         = "bin",
    [BASE_OS_IOS_DEVICE]    = "bin",
    [BASE_OS_IOS_SIMULATOR] = "bin",
    [BASE_OS_WASM]          = "wasm",
};

#define BUILD_ARGUMENTS_MAX 256
typedef struct {
    const char *items[BUILD_ARGUMENTS_MAX];
    u32 count;
} Build_Arguments;

static void build_push_argument(Build_Arguments *arguments, const char *argument) {
    assert(arguments->count + 1 < BUILD_ARGUMENTS_MAX);
    arguments->items[arguments->count] = argument;
    arguments->count += 1;
}

static void build_push_arguments(Build_Arguments *arguments, const char **list) {
    if (list == 0) return;
    for (const char **a = list; *a != 0; a += 1) build_push_argument(arguments, *a);
}

typedef struct {
    const char *code_path, *object_path;
    const char **extra_flags;
    bool no_extra_errors;
} Build_Object;

#define BUILD_OBJECTS_MAX 64
typedef struct {
    Build_Object items[BUILD_OBJECTS_MAX];
    u32 count;
} Build_Objects;

static void build_objects_push(Build_Objects *objects, Build_Object object) {
    assert(objects->count < BUILD_OBJECTS_MAX);
    objects->items[objects->count] = object;
    objects->count += 1;
}

enum { BUILD_LOCATION_CAPACITY = 4128 };

static void build_format_location(char *buffer, u64 buffer_size, const clex_Lexer *lexer) {
    int column = 0;
    const char *path = 0;
    const char *path_end = 0;
    int line = clex_get_current_location(lexer, &column, &path, &path_end);

    enum { PATH_CAPACITY = 4096 };
    char path_as_given[PATH_CAPACITY] = {0};
    u64 path_count = MIN((u64)(path_end - path), PATH_CAPACITY - 1);
    if (path_count > 0) memcpy(path_as_given, path, size_t_from_u64(path_count));
    char absolute_path[PATH_CAPACITY] = {0};
    bool has_absolute_path = path_count > 0 && fs_absolute_path(path_as_given, absolute_path, sizeof absolute_path);

    const char *shown_path = path_count > 0 ? path_as_given : "<MACRO>";
    if (has_absolute_path) shown_path = absolute_path;
    small_snprintf(buffer, size_t_from_u64(buffer_size), "%s(%d,%d)", shown_path, line, column);
}

static bool build_eat_token(clex_Lexer *lexer, clex_Token_Kind expected, const char *format, ...) {
    clex_lex(lexer);
    bool ok = lexer->token.kind == expected;

    va_list arguments;
    va_start(arguments, format);
    if (!ok) {
        char location[BUILD_LOCATION_CAPACITY] = {0};
        build_format_location(location, sizeof location, lexer);

        print("%s: error: ", location);
        print_(format, arguments);
        print("; got '%S'\n", base_string_from_clex_data(lexer->token.data));
    }
    va_end(arguments);

    return ok;
}

static const char *base_clex_current_location_get_as_string(Arena *arena, const clex_Lexer *lexer) {
    char location[BUILD_LOCATION_CAPACITY] = {0};
    build_format_location(location, sizeof location, lexer);
    return cstring_print(arena, "%s", location);
}

static void build_log_error(const clex_Lexer *lexer, const char *format, ...) {
    char location[BUILD_LOCATION_CAPACITY] = {0};
    build_format_location(location, sizeof location, lexer);

    print("%s: error: ", location);
    va_list arguments;
    va_start(arguments, format);
    print_(format, arguments);
    va_end(arguments);
    print("\n");
}

typedef struct {
    clex_Condition_Value value;
    Build_Unknown_Reason_Id unknown_reason;
} Build_Condition;

static Build_Condition build_evaluate_macro_is_defined(Arena *arena, Build_Macros *macros, String name, bool expect_defined) {
    Build_Macro macro = build_macros_get(arena, macros, name);

    bool is_defined = macro.state == Build_Macro_State_DEFINED || macro.state == Build_Macro_State_DEFINED_VALUE_UNKNOWN;
    Build_Condition condition = { .value = is_defined == expect_defined ? clex_Condition_Value_TRUE : clex_Condition_Value_FALSE };
    if (macro.state == Build_Macro_State_UNKNOWN) condition = (Build_Condition){ .value = clex_Condition_Value_UNKNOWN, .unknown_reason = macro.unknown_reason };

    return condition;
}

static Build_Condition build_evaluate_preprocessor_condition(Arena *arena, const clex_Lexer *file, Build_Macros *macros, clex_Data condition_text, bool *ok) {
    clex_Condition line = clex_condition_init(condition_text);
    Build_Unknown_Reason_Id first_unknown_reason = 0;

    while (clex_condition_step(&line)) {
        String name = base_string_from_clex_data(line.name);

        switch (line.request) {
            case clex_Condition_Request_TOKEN_MEMORY: {
                line.tokens = arena_array_reserve(arena, line.tokens, line.token_count, &line.token_capacity, (u64)line.token_count + 1);
            } break;
            case clex_Condition_Request_DEFINED: {
                Build_Condition definedness = build_evaluate_macro_is_defined(arena, macros, name, true);
                if (first_unknown_reason == 0) first_unknown_reason = definedness.unknown_reason;
                clex_condition_answer_value(&line, definedness.value);
            } break;
            case clex_Condition_Request_BUILTIN: {
                // TODO(felix): Support compiler builtins like `__has_builtin(...)` and `__has_feature(...)`.
                if (first_unknown_reason == 0) first_unknown_reason = build_unknown_reason(arena, macros, cstring_print(arena, "`%S(...)` is a compiler builtin, which the metaprogram does not evaluate", name), 0);
                clex_condition_answer_value(&line, clex_Condition_Value_UNKNOWN);
            } break;
            case clex_Condition_Request_NAME: {
                Build_Macro macro = build_macros_get(arena, macros, name);
                switch (macro.state) {
                    case Build_Macro_State_UNDEFINED: clex_condition_answer_value(&line, clex_Condition_Value_FALSE); break;
                    case Build_Macro_State_DEFINED: {
                        clex_Data definition = { .start = macro.definition.data, .end = macro.definition.data + macro.definition.count };
                        clex_condition_answer_definition(&line, definition);
                    } break;
                    case Build_Macro_State_DEFINED_VALUE_UNKNOWN: case Build_Macro_State_UNKNOWN: {
                        if (first_unknown_reason == 0) first_unknown_reason = macro.unknown_reason;
                        clex_condition_answer_value(&line, clex_Condition_Value_UNKNOWN);
                    } break;
                    default: unreachable;
                }
            } break;
            default: unreachable;
        }
    }

    clex_Node *nodes = arena_make(arena, size_t_from_u64((u64)line.token_count + 1), clex_Node);
    clex_Condition_Value value = clex_condition_evaluate(&line, nodes);

    String condition_string = base_string_from_clex_data(condition_text);
    String error_text = line.error_text.start != 0 ? base_string_from_clex_data(line.error_text) : S("end of line");

    const char *message = 0;
    switch (line.error) {
        case clex_Condition_Error_NONE: break;
        case clex_Condition_Error_EXPANSION_DEPTH: message = cstring_print(arena, "macro expansion is nested too deeply in condition `%S`", condition_string); break;
        case clex_Condition_Error_DEFINED_OPERAND: message = cstring_print(arena, "expected `defined NAME` or `defined(NAME)` in condition `%S`", condition_string); break;
        case clex_Condition_Error_BUILTIN_OPERAND: message = cstring_print(arena, "expected `(...)` after compiler builtin in condition `%S`", condition_string); break;
        case clex_Condition_Error_FUNCTION_LIKE_MACRO: message = cstring_print(arena, "function-like macro `%S` is not supported in condition `%S`", error_text, condition_string); break;
        case clex_Condition_Error_INVALID_TOKEN: message = cstring_print(arena, "invalid token in condition `%S`", condition_string); break;
        case clex_Condition_Error_INVALID_EXPRESSION: message = cstring_print(arena, "invalid expression at `%S` in condition `%S`", error_text, condition_string); break;
        case clex_Condition_Error_EVALUATION: message = cstring_print(arena, "cannot evaluate `%S` in condition `%S`", error_text, condition_string); break;
        default: unreachable;
    }
    if (message != 0) build_log_error(file, "%s", message);

    Build_Condition condition = { .value = value };
    if (value == clex_Condition_Value_UNKNOWN) {
        assert(first_unknown_reason != 0);
        condition.unknown_reason = build_unknown_reason(arena, macros, cstring_print(arena, "%s: condition `%S` is unknown", base_clex_current_location_get_as_string(arena, file), condition_string), first_unknown_reason);
    }

    *ok = message == 0;
    return condition;
}

// The metaprogram reads either a:
// known group (group_unknown_reason == 0): the compiler builds it, so the metaprogram acts on everything; or
// unknown group (group_unknown_reason != 0): the compiler maybe builds it, so the metaprogram reads it, but `META(...)` is an error, `#define` and `#undef` make their macro unknown, and `#error` has no effect.
// The metaprogram doesn't read a group the compiler skips.
typedef struct {
    clex_Conditional chain;
    Build_Unknown_Reason_Id group_unknown_reason;
} Build_Conditional;

static bool build_enter_conditional_group(Build_Conditional *conditionals, u32 conditional_depth, Build_Condition condition, bool is_else) {
    Build_Conditional *conditional = &conditionals[conditional_depth - 1];
    bool earlier_group_maybe_taken = conditional->chain.state == clex_Conditional_State_MAYBE_TAKEN;
    bool read_group = clex_conditional_enter(&conditional->chain, condition.value, is_else);

    if (read_group && condition.value == clex_Condition_Value_UNKNOWN) conditional->group_unknown_reason = condition.unknown_reason;
    if (read_group && condition.value == clex_Condition_Value_TRUE && !earlier_group_maybe_taken) {
        conditional->group_unknown_reason = conditional_depth >= 2 ? conditionals[conditional_depth - 2].group_unknown_reason : 0;
    }

    return read_group;
}

static void build_macros_predefine(Build_Macros *macros, const char *name, Build_Macro_State state, const char *definition, Build_Unknown_Reason_Id unknown_reason) {
    bool ok = false;
    u32 slot = build_macros_insert(macros, string_from_cstring(name), &ok);
    assert(ok);

    bool is_known = state == Build_Macro_State_DEFINED || state == Build_Macro_State_UNDEFINED;
    assert(is_known == (unknown_reason == 0));
    assert((state == Build_Macro_State_DEFINED) == (definition != 0));

    macros->slots[slot].macro = (Build_Macro){
        .state = state,
        .definition = definition != 0 ? string_from_cstring(definition) : (String){0},
        .unknown_reason = unknown_reason,
    };
}

static Build_Unknown_Reason_Id build_unknown_reason_for_unset_setting(Arena *arena, Build_Macros *macros, const char *setting, const char *directive, const char *meta_location) {
    return build_unknown_reason(arena, macros, meta_location != 0
        ? cstring_print(arena, "%s is set only by `META(%s)` at %s, which is itself in an unknown group", setting, directive, meta_location)
        : cstring_print(arena, "%s is set by `META(%s = ...)`, and the program has none in a known group", setting, directive), 0);
}

static void build_set_macro_from_meta(const clex_Lexer *lexer, Build_Macros *macros, const char *directive, const char *name, bool is_defined, const char *definition, bool *ok) {
    bool insert_ok = false;
    u32 slot = build_macros_insert(macros, string_from_cstring(name), &insert_ok);

    bool is_defined_by_source = insert_ok && macros->slots[slot].is_defined_by_source;
    if (is_defined_by_source) build_log_error(lexer, "`META(%s)` sets `%s`, but the source defines or undefines `%s` before it", directive, name, name);

    bool set_ok = insert_ok && !is_defined_by_source;
    if (set_ok) {
        Build_Macro *macro = &macros->slots[slot].macro;
        *macro = (Build_Macro){0};
        if (is_defined) {
            macro->state = Build_Macro_State_DEFINED;
            macro->definition = string_from_cstring(definition);
        }
    }

    *ok = set_ok;
}

typedef struct {
    u8 os_mask;
    u8 compiler_mask;
    u8 arch_mask;
    Build_Macro_State state;
    const char *name;
    const char *definition;
} Build_Platform_Macro;

enum {
    Build_Platform_ANY_OS = BASE_OS_COUNT - 1,
    Build_Platform_ANY_ARCH = BASE_ARCH_COUNT - 1,
    Build_Platform_MSVC = 1 << Build_Compiler_MSVC,
    Build_Platform_CLANG = 1 << Build_Compiler_CLANG,
    Build_Platform_ANY_COMPILER = Build_Platform_MSVC | Build_Platform_CLANG,
};

static const Build_Platform_Macro build_platform_macros[] = {
    { BASE_OS_WINDOWS, Build_Platform_ANY_COMPILER, Build_Platform_ANY_ARCH, Build_Macro_State_DEFINED, "_WIN32", "1" },
    { BASE_OS_WINDOWS, Build_Platform_ANY_COMPILER, BASE_ARCH_X86_64 | BASE_ARCH_ARM64, Build_Macro_State_DEFINED, "_WIN64", "1" },
    { BASE_OS_WINDOWS, Build_Platform_ANY_COMPILER, Build_Platform_ANY_ARCH, Build_Macro_State_DEFINED_VALUE_UNKNOWN, "_MSC_VER", 0 },
    { BASE_OS_WINDOWS, Build_Platform_ANY_COMPILER, BASE_ARCH_X86_64, Build_Macro_State_DEFINED, "_M_X64", "100" },
    { BASE_OS_WINDOWS, Build_Platform_ANY_COMPILER, BASE_ARCH_X86_64, Build_Macro_State_DEFINED, "_M_AMD64", "100" },
    { BASE_OS_WINDOWS, Build_Platform_ANY_COMPILER, BASE_ARCH_ARM64, Build_Macro_State_DEFINED, "_M_ARM64", "1" },
    { BASE_OS_WINDOWS, Build_Platform_ANY_COMPILER, BASE_ARCH_ARM64, Build_Macro_State_UNKNOWN, "_M_ARM64EC", 0 },
    { BASE_OS_WINDOWS, Build_Platform_ANY_COMPILER, Build_Platform_ANY_ARCH, Build_Macro_State_UNKNOWN, "__STDC__", 0 },
    { Build_Platform_ANY_OS & ~BASE_OS_WINDOWS, Build_Platform_CLANG, Build_Platform_ANY_ARCH, Build_Macro_State_DEFINED, "__STDC__", "1" },
    { Build_Platform_ANY_OS, Build_Platform_ANY_COMPILER, Build_Platform_ANY_ARCH, Build_Macro_State_DEFINED, "__STDC_VERSION__", "201112L" },
    { Build_Platform_ANY_OS, Build_Platform_CLANG, Build_Platform_ANY_ARCH, Build_Macro_State_DEFINED, "__clang__", "1" },
    { Build_Platform_ANY_OS & ~BASE_OS_WINDOWS, Build_Platform_CLANG, Build_Platform_ANY_ARCH, Build_Macro_State_DEFINED, "__GNUC__", "4" },
    { Build_Platform_ANY_OS, Build_Platform_CLANG, BASE_ARCH_X86_64, Build_Macro_State_DEFINED, "__x86_64__", "1" },
    { Build_Platform_ANY_OS, Build_Platform_CLANG, BASE_ARCH_ARM64, Build_Macro_State_DEFINED, "__aarch64__", "1" },
    { BASE_OS_WASM, Build_Platform_CLANG, BASE_ARCH_WASM32, Build_Macro_State_DEFINED, "__wasm__", "1" },
    { BASE_OS_WASM, Build_Platform_CLANG, BASE_ARCH_WASM32, Build_Macro_State_DEFINED, "__wasm32__", "1" },
    { BASE_OS_ANY_APPLE, Build_Platform_CLANG, Build_Platform_ANY_ARCH, Build_Macro_State_DEFINED, "__APPLE__", "1" },
    { BASE_OS_ANY_APPLE, Build_Platform_CLANG, Build_Platform_ANY_ARCH, Build_Macro_State_DEFINED, "__MACH__", "1" },
    { BASE_OS_LINUX, Build_Platform_CLANG, Build_Platform_ANY_ARCH, Build_Macro_State_DEFINED, "__linux__", "1" },
    { BASE_OS_LINUX, Build_Platform_CLANG, Build_Platform_ANY_ARCH, Build_Macro_State_UNKNOWN, "__gnu_linux__", 0 },
};

static const char *const build_os_macro_names[BASE_OS_COUNT] = {
    [BASE_OS_WINDOWS]       = "BASE_OS_WINDOWS",
    [BASE_OS_MACOS]         = "BASE_OS_MACOS",
    [BASE_OS_IOS_DEVICE]    = "BASE_OS_IOS_DEVICE",
    [BASE_OS_IOS_SIMULATOR] = "BASE_OS_IOS_SIMULATOR",
    [BASE_OS_LINUX]         = "BASE_OS_LINUX",
    [BASE_OS_WASM]          = "BASE_OS_WASM",
};

typedef enum {
    Build_Meta_Setting_UNSET = 0,
    Build_Meta_Setting_FALSE,
    Build_Meta_Setting_TRUE,
} Build_Meta_Setting;

typedef struct {
    Build_Meta_Setting link_crt;
    Build_Meta_Setting subsystem_windows;
} Build_Meta_Settings;

typedef struct {
    const char *location;
    String directive;
    Build_Unknown_Reason_Id unknown_reason;
} Build_Meta_In_Unknown_Group;

static bool build_flag_changes_preprocessor(String flag) {
    bool defines = string_starts_with(flag, S("-D")) || string_starts_with(flag, S("/D"));
    defines = defines || string_starts_with(flag, S("-U")) || string_starts_with(flag, S("/U"));
    defines = defines || string_starts_with(flag, S("-std")) || string_starts_with(flag, S("/std"));
    return defines;
}

static void build_rebuild_and_rerun_self_if_out_of_date(Arena *scratch_arena, const char **arguments, u64 argument_count) {
    #if BASE_OS == BASE_OS_WINDOWS
    {
        Scratch scratch = scratch_begin(scratch_arena);

        char executable_path[4096] = {0};
        bool executable_path_ok = fs_executable_path(executable_path, sizeof executable_path);
        String_Cut executable_path_parts = string_cut_last(string_from_cstring(executable_path), '\\');
        String executable_folder = executable_path_parts.head;
        String executable_name = executable_path_parts.tail;

        const char *build_source_path = cstring_print(scratch.arena, "%S\\build.c", executable_folder);
        const char *base_folder_path = cstring_print(scratch.arena, "%S\\src\\base", executable_folder);

        fs_File_Info executable_info = {0};
        bool executable_info_ok = executable_path_ok && fs_file_info(executable_path, &executable_info);

        // TODO(felix): Check the timestamps of only the files that build.c recursively depends on, once the parser supports this.
        fs_File_Info build_source_info = {0};
        bool build_source_ok = executable_info_ok && fs_file_info(build_source_path, &build_source_info);
        u64 newest_source_time = build_source_info.modification_time;
        const char *newest_source_path = build_source_path;

        bool base_folder_ok = false;
        if (build_source_ok) {
            fs_Folder_Iterator it = fs_folder_iterator(base_folder_path, fs_Folder_Iterator_Flag_RECURSIVE);
            while (fs_folder_iterate(&it)) {
                bool is_newer_file = !it.info.is_folder && it.info.modification_time > newest_source_time;
                if (is_newer_file) {
                    newest_source_time = it.info.modification_time;
                    newest_source_path = cstring_print(scratch.arena, "%s", it.info.path);
                }
            }
            base_folder_ok = !it.had_error;
            fs_folder_iterator_end(&it);
        }

        bool sources_ok = build_source_ok && base_folder_ok;
        if (!sources_ok) log_error("can't check if '%s' is out of date against '%s' and '%s'", executable_path, build_source_path, base_folder_path);

        bool out_of_date = sources_ok && newest_source_time > executable_info.modification_time;
        if (!out_of_date){
            print("info: reusing '%s'\n", executable_path);
        } else {
            print("info: rebuilding '%s' because '%s' is newer\n", executable_path, newest_source_path);

            String executable_stem = string_cut_last(executable_name, '.').head;
            const char *executable_folder_cstring = cstring_from_string(scratch.arena, executable_folder);
            const char *old_executable_path = cstring_print(scratch.arena, "%S\\%S.old.exe", executable_folder, executable_stem);
            const char *output_flag = cstring_print(scratch.arena, "-Fe:%S", executable_name);

            bool renamed = fs_rename(executable_path, old_executable_path);
            if (!renamed) log_error("can't rename '%s' to '%s'", executable_path, old_executable_path);

            const char *compile[] = {
                "cl", "-nologo", "-FC", "-diagnostics:column", "-std:c11", "-Oi", "build.c", "-Z7", "-MTd", "-DBUILD_DEBUG", "-DWINDOWS_SUBSYSTEM=WINDOWS_SUBSYSTEM_CONSOLE", "-Isrc",
                "-link", "-subsystem:console", "-incremental:no", "-entry:entrypoint", "Kernel32.lib", "Ntdll.lib", output_flag,
                0,
            };
            Os_Process_Flags compile_flags = Os_Process_Flag_PRINT_COMMAND_BEFORE_RUNNING | Os_Process_Flag_PRINT_EXIT_CODE | Os_Process_Flag_PRINT_EXECUTION_TIME;
            u32 rebuild_exit_code = renamed ? os_process_run(scratch.arena, compile, executable_folder_cstring, compile_flags).exit_code : 1;
            bool compiled = renamed && rebuild_exit_code == 0;

            if (renamed && !compiled) {
                bool restored = fs_rename(old_executable_path, executable_path);
                if (restored) log_error("rebuilding '%s' failed; restored the previous executable", executable_path);
                else log_error("rebuilding '%s' failed, and restoring it from '%s' also failed", executable_path, old_executable_path);
            }

            fs_File_Info rebuilt_info = {0};
            bool rebuilt_info_ok = compiled && fs_file_info(executable_path, &rebuilt_info);
            bool rebuilt_is_current = rebuilt_info_ok && rebuilt_info.modification_time >= newest_source_time;
            if (compiled && !rebuilt_is_current) {
                log_error("rebuilt '%s' is still older than '%s', which may have a timestamp in the future", executable_path, newest_source_path);
                rebuild_exit_code = 1;
            }

            if (rebuilt_is_current) {
                const char **rerun_arguments = arena_make(scratch.arena, argument_count + 1, const char *);
                rerun_arguments[0] = executable_path;
                for (u64 i = 1; i < argument_count; i += 1) rerun_arguments[i] = arguments[i];
                rebuild_exit_code = os_process_run(scratch.arena, rerun_arguments, 0, 0).exit_code;
            }

            os_exit(rebuild_exit_code);
        }

        scratch_end(scratch);
    }
    #else
    {
        (void)scratch_arena;
        (void)arguments;
        (void)argument_count;
    }
    #endif
}

static u32 build_default_everything(Arena *scratch_arena, const char *program_name, const Build_Options *options) {
    Scratch scratch = scratch_begin(scratch_arena);

    u8 target_os = BASE_OS;
    if (options != 0 && options->target_os != 0) target_os = options->target_os;

    u32 exit_code = 1;

    u64 argument_count = 0;
    const char **arguments = os_get_arguments(scratch.arena, &argument_count);
    build_rebuild_and_rerun_self_if_out_of_date(scratch.arena, arguments, argument_count);

    char path_separator = (BASE_OS & BASE_OS_ANY_POSIX) ? '/' : '\\';

    const char *source_folder = cstring_print(scratch.arena, "..%csrc%c", path_separator, path_separator);
    const char *dependency_folder = cstring_print(scratch.arena, "..%cdeps%c", path_separator, path_separator);
    const char *c_file_path = cstring_print(scratch.arena, "%smain.c", source_folder);

    static const Build_Compiler build_default_compiler[BASE_OS_COUNT] = {
        [BASE_OS_WINDOWS]       = Build_Compiler_MSVC,
        [BASE_OS_MACOS]         = Build_Compiler_CLANG,
        [BASE_OS_IOS_DEVICE]    = Build_Compiler_CLANG,
        [BASE_OS_IOS_SIMULATOR] = Build_Compiler_CLANG,
        [BASE_OS_WASM]          = Build_Compiler_CLANG,
    };
    Build_Compiler compiler = build_default_compiler[target_os];

    Build_Mode build_mode = Build_Mode_DEBUG;
    for (u64 i = 0; i < argument_count; i += 1) {
        if (cstring_equals(arguments[i], "clang")) compiler = Build_Compiler_CLANG;
        if (cstring_equals(arguments[i], "release")) build_mode = Build_Mode_RELEASE;
    }

    Build_Objects objects = {0};

    Build_Arguments compiler_extra_flags = {0};
    Build_Arguments linker_extra_flags = {0};
    Build_Arguments link_libraries = {0};

    const char *object_extension = build_object_extension[target_os];

    const char *build_folder = "build";
    bool build_folder_ok = fs_exists(build_folder);
    if (!build_folder_ok) {
        build_folder_ok = fs_make_folder(build_folder);
        if (!build_folder_ok) log_error("failure creating folder '%s'", build_folder);
    }

    bool ok = false;
    String code = os_read_entire_file(scratch.arena, &c_file_path[3], &ok);
    assert(ok);

    String_Builder metaprogram_code = {0};
    String metaprogram_output_file = {0};

    String generate_here_directive = S("generate_here");

    typedef struct {
        String name;
        String contents;
    } Build_Embedded_File;

    Build_Embedded_File *files_to_embed = 0;
    u64 files_to_embed_count = 0;
    u64 files_to_embed_capacity = 0;

    enum { BUILD_CONDITIONAL_DEPTH_MAX = 64, BUILD_PASS_COUNT_MAX = 8 };
    Build_Conditional conditionals[BUILD_CONDITIONAL_DEPTH_MAX] = {0};
    u32 conditional_depth = 0;
    Build_Macros *macros = 0;
    clex_Lexer lexer = {0};

    Build_Meta_Settings settings_from_earlier_passes = {0};
    bool collect_unknown_reasons = false;
    const char *link_libc_meta_location = 0;
    const char *subsystem_meta_location = 0;
    u32 pass_count = 0;
    f64 time_at_parse_start = time_relative_timestamp();
    bool parse_ok = build_folder_ok;

    for (bool parsing = parse_ok; parsing;) {
        pass_count += 1;
        assert(pass_count <= BUILD_PASS_COUNT_MAX);

        compiler_extra_flags = (Build_Arguments){0};
        linker_extra_flags = (Build_Arguments){0};
        link_libraries = (Build_Arguments){0};
        metaprogram_output_file = (String){0};
        files_to_embed_count = 0;
        conditional_depth = 0;

        Build_Meta_Settings settings_this_pass = {0};
        Build_Meta_In_Unknown_Group *metas_in_unknown_groups = 0;
        u64 meta_in_unknown_group_count = 0;
        u64 meta_in_unknown_group_capacity = 0;
        Build_Unknown_Reason_Id link_crt_unset_reason = 0;
        Build_Unknown_Reason_Id address_sanitiser_unset_reason = 0;
        Build_Unknown_Reason_Id subsystem_unset_reason = 0;

        macros = arena_make(scratch.arena, 1, Build_Macros);
        macros->collect_unknown_reasons = collect_unknown_reasons;
        {
            u8 target_arch = BASE_ARCH;
            if (target_os == BASE_OS_WASM) target_arch = BASE_ARCH_WASM32;
            if (target_os == BASE_OS_IOS_DEVICE) target_arch = BASE_ARCH_ARM64;
            u8 compiler_bit = (u8)(1 << compiler);

            u64 platform_macro_count = sizeof build_platform_macros / sizeof build_platform_macros[0];
            for (u64 i = 0; i < platform_macro_count; i += 1) {
                Build_Platform_Macro macro = build_platform_macros[i];
                bool applies = (macro.os_mask & target_os) && (macro.compiler_mask & compiler_bit) && (macro.arch_mask & target_arch);
                if (applies) {
                    Build_Unknown_Reason_Id unknown_reason = 0;
                    if (macro.state == Build_Macro_State_DEFINED_VALUE_UNKNOWN) unknown_reason = build_unknown_reason(scratch.arena, macros, cstring_print(scratch.arena, "`%s` is defined, but the metaprogram does not know its value", macro.name), 0);
                    if (macro.state == Build_Macro_State_UNKNOWN) unknown_reason = build_unknown_reason(scratch.arena, macros, cstring_print(scratch.arena, "the metaprogram does not know if the compiler defines `%s`", macro.name), 0);
                    build_macros_predefine(macros, macro.name, macro.state, macro.definition, unknown_reason);
                }
            }

            for (u64 i = 0; i < platform_macro_count; i += 1) {
                const char *name = build_platform_macros[i].name;
                bool is_listed_for_target = macros->slots[build_macros_find(macros, string_from_cstring(name))].name.data != 0;
                if (!is_listed_for_target) build_macros_predefine(macros, name, Build_Macro_State_UNDEFINED, 0, 0);
            }

            build_macros_predefine(macros, "BASE_OS", Build_Macro_State_DEFINED, build_os_macro_names[target_os], 0);
            build_macros_predefine(macros, "BUILD_DEBUG", Build_Macro_State_DEFINED, build_mode == Build_Mode_DEBUG ? "1" : "0", 0);
            if (settings_from_earlier_passes.link_crt != Build_Meta_Setting_UNSET) {
                build_macros_predefine(macros, "LINK_CRT", Build_Macro_State_DEFINED, settings_from_earlier_passes.link_crt == Build_Meta_Setting_TRUE ? "1" : "0", 0);
            } else {
                link_crt_unset_reason = build_unknown_reason_for_unset_setting(scratch.arena, macros, "`LINK_CRT`", "link_libc", link_libc_meta_location);
                build_macros_predefine(macros, "LINK_CRT", Build_Macro_State_DEFINED_VALUE_UNKNOWN, 0, link_crt_unset_reason);
            }

            if (target_os == BASE_OS_WINDOWS) {
                if (settings_from_earlier_passes.subsystem_windows != Build_Meta_Setting_UNSET) {
                    bool subsystem_is_windows_so_far = settings_from_earlier_passes.subsystem_windows == Build_Meta_Setting_TRUE;
                    build_macros_predefine(macros, "WINDOWS_SUBSYSTEM", Build_Macro_State_DEFINED, subsystem_is_windows_so_far ? "WINDOWS_SUBSYSTEM_WINDOWS" : "WINDOWS_SUBSYSTEM_CONSOLE", 0);
                } else {
                    subsystem_unset_reason = build_unknown_reason_for_unset_setting(scratch.arena, macros, "the Windows subsystem", "subsystem", subsystem_meta_location);
                    build_macros_predefine(macros, "WINDOWS_SUBSYSTEM", Build_Macro_State_DEFINED_VALUE_UNKNOWN, 0, subsystem_unset_reason);
                }
            }

            if (compiler == Build_Compiler_MSVC) {
                bool address_sanitiser_is_known = build_mode == Build_Mode_RELEASE || settings_from_earlier_passes.link_crt != Build_Meta_Setting_UNSET;
                bool uses_address_sanitiser = build_mode == Build_Mode_DEBUG && settings_from_earlier_passes.link_crt == Build_Meta_Setting_TRUE;
                if (address_sanitiser_is_known) {
                    build_macros_predefine(macros, "__SANITIZE_ADDRESS__", uses_address_sanitiser ? Build_Macro_State_DEFINED : Build_Macro_State_UNDEFINED, uses_address_sanitiser ? "1" : 0, 0);
                } else {
                    address_sanitiser_unset_reason = build_unknown_reason_for_unset_setting(scratch.arena, macros, "the address sanitiser", "link_libc", link_libc_meta_location);
                    build_macros_predefine(macros, "__SANITIZE_ADDRESS__", Build_Macro_State_UNKNOWN, 0, address_sanitiser_unset_reason);
                }
            }
        }

        lexer = clex_init(&c_file_path[3], &c_file_path[cstring_length(c_file_path)], code.data, &code.data[code.count], 0);
        while (parse_ok && clex_lex(&lexer)) {
            String preprocess_data2 = base_string_from_clex_data(lexer.token.data2);
            bool skip_group = false;
            Build_Unknown_Reason_Id group_unknown_reason = conditional_depth > 0 ? conditionals[conditional_depth - 1].group_unknown_reason : 0;
            bool group_is_unknown = group_unknown_reason != 0;

            switch (lexer.token.kind) {
                case clex_Token_DEFINE: case clex_Token_UNDEF: {
                    bool is_define = lexer.token.kind == clex_Token_DEFINE;
                    clex_Data name = is_define ? lexer.token.data : clex_directive_name(lexer.token.data2);
                    parse_ok = name.start != name.end;
                    if (!parse_ok) build_log_error(&lexer, "expected a name after `#undef`");

                    String name_string = base_string_from_clex_data(name);
                    u32 slot = 0;
                    if (parse_ok) slot = build_macros_insert(macros, name_string, &parse_ok);
                    if (parse_ok) macros->slots[slot].is_defined_by_source = true;

                    if (parse_ok && group_is_unknown) {
                        macros->slots[slot].macro = (Build_Macro){ .state = Build_Macro_State_UNKNOWN, .unknown_reason = build_unknown_reason(scratch.arena, macros, cstring_print(scratch.arena, "%s: `#%s %S` is in an unknown group", base_clex_current_location_get_as_string(scratch.arena, &lexer), is_define ? "define" : "undef", name_string), group_unknown_reason) };
                    } else if (parse_ok) {
                        macros->slots[slot].macro = is_define ? (Build_Macro){ .state = Build_Macro_State_DEFINED, .definition = preprocess_data2 } : (Build_Macro){ .state = Build_Macro_State_UNDEFINED };
                    }
                } break;
                case clex_Token_INCLUDE: {
                    bool include_ok = false;
                    clex_Include include = clex_include_operand(lexer.token.data2, &include_ok);
                    parse_ok = include_ok;
                    if (!parse_ok) build_log_error(&lexer, "expected `\"path\"` or `<path>` after `#include`; got `%S`", preprocess_data2);

                    if (parse_ok && !include.is_system) {
                        String path = base_string_from_clex_data(include.path);
                        const char *read_path = cstring_print(scratch.arena, "%s%S", &source_folder[3], path);
                        String file = os_read_entire_file(scratch.arena, read_path, &parse_ok);
                        if (!parse_ok) build_log_error(&lexer, "failed reading '%s'", read_path);

                        if (parse_ok) {
                            ok = clex_push_file(&lexer, path.data, &path.data[path.count], file.data, &file.data[file.count]);
                            assert(ok);
                        }
                    }
                } break;
                case clex_Token_IF: case clex_Token_IFDEF: case clex_Token_IFNDEF: {
                    Build_Condition condition = { .value = clex_Condition_Value_FALSE };
                    if (lexer.token.kind == clex_Token_IF) {
                        condition = build_evaluate_preprocessor_condition(scratch.arena, &lexer, macros, lexer.token.data2, &parse_ok);
                    } else {
                        clex_Data name = clex_directive_name(lexer.token.data2);
                        if (name.start == name.end) {
                            build_log_error(&lexer, "expected a name after `#ifdef` or `#ifndef`");
                            parse_ok = false;
                        }
                        String name_string = base_string_from_clex_data(name);
                        if (parse_ok) condition = build_evaluate_macro_is_defined(scratch.arena, macros, name_string, lexer.token.kind == clex_Token_IFDEF);
                        if (condition.value == clex_Condition_Value_UNKNOWN) {
                            const char *directive = lexer.token.kind == clex_Token_IFDEF ? "ifdef" : "ifndef";
                            condition.unknown_reason = build_unknown_reason(scratch.arena, macros, cstring_print(scratch.arena, "%s: `#%s %S` is unknown", base_clex_current_location_get_as_string(scratch.arena, &lexer), directive, name_string), condition.unknown_reason);
                        }
                    }

                    if (conditional_depth == BUILD_CONDITIONAL_DEPTH_MAX) {
                        build_log_error(&lexer, "conditionals are nested too deeply");
                        parse_ok = false;
                    }

                    if (parse_ok) {
                        Build_Conditional *conditional = &conditionals[conditional_depth];
                        conditional_depth += 1;
                        *conditional = (Build_Conditional){0};
                        skip_group = !build_enter_conditional_group(conditionals, conditional_depth, condition, false);
                    }
                } break;
                case clex_Token_ELIF: case clex_Token_ELSE: {
                    bool is_else = lexer.token.kind == clex_Token_ELSE;
                    Build_Conditional *conditional = conditional_depth > 0 ? &conditionals[conditional_depth - 1] : 0;

                    if (conditional == 0) {
                        build_log_error(&lexer, is_else ? "`#else` without `#if`" : "`#elif` without `#if`");
                        parse_ok = false;
                    } else if (conditional->chain.saw_else) {
                        build_log_error(&lexer, is_else ? "`#else` after `#else`" : "`#elif` after `#else`");
                        parse_ok = false;
                    }

                    bool chain_has_taken_group = parse_ok && conditional->chain.state == clex_Conditional_State_TAKEN;
                    Build_Condition condition = { .value = clex_Condition_Value_TRUE };
                    if (parse_ok && !chain_has_taken_group && !is_else) {
                        condition = build_evaluate_preprocessor_condition(scratch.arena, &lexer, macros, lexer.token.data2, &parse_ok);
                    }

                    if (parse_ok) skip_group = !build_enter_conditional_group(conditionals, conditional_depth, condition, is_else);
                } break;
                case clex_Token_ENDIF: {
                    if (conditional_depth == 0) {
                        build_log_error(&lexer, "`#endif` without `#if`");
                        parse_ok = false;
                    } else conditional_depth -= 1;
                } break;
                case clex_Token_ERROR_DIRECTIVE: {
                    if (!group_is_unknown) {
                        build_log_error(&lexer, "#error %S", preprocess_data2);
                        parse_ok = false;
                    }
                } break;
                default: break;
            }

            if (skip_group && !clex_skip_conditional_group(&lexer)) {
                build_log_error(&lexer, "missing `#endif`");
                parse_ok = false;
            }

            bool token_is_meta = parse_ok && lexer.token.kind == clex_Token_IDENTIFIER && string_equals(base_string_from_clex_data(lexer.token.data), S("META"));

            if (token_is_meta && group_is_unknown) {
                clex_Lexer directive_lexer = lexer;
                String directive = {0};
                bool has_directive = clex_lex(&directive_lexer) && directive_lexer.token.kind == '(' && clex_lex(&directive_lexer) && directive_lexer.token.kind == clex_Token_IDENTIFIER;
                if (has_directive) directive = base_string_from_clex_data(directive_lexer.token.data);

                metas_in_unknown_groups = arena_array_reserve(scratch.arena, metas_in_unknown_groups, meta_in_unknown_group_count, &meta_in_unknown_group_capacity, meta_in_unknown_group_count + 1);
                metas_in_unknown_groups[meta_in_unknown_group_count] = (Build_Meta_In_Unknown_Group){ .location = base_clex_current_location_get_as_string(scratch.arena, &lexer), .directive = directive, .unknown_reason = group_unknown_reason };
                meta_in_unknown_group_count += 1;
            }

            bool meta_has_arguments = token_is_meta && !group_is_unknown && clex_lex(&lexer) && lexer.token.kind == '(';
            if (meta_has_arguments) parse_ok = build_eat_token(&lexer, clex_Token_IDENTIFIER, "expected directive at start of `META(...)`");

            if (meta_has_arguments && parse_ok) {
                String token_data = base_string_from_clex_data(lexer.token.data);

                if (string_equals(token_data, generate_here_directive)) {
                    parse_ok = metaprogram_output_file.count == 0;
                    if (!parse_ok) build_log_error(&lexer, "directive `META(%S)` can only be used once", generate_here_directive);
                    parse_ok = parse_ok && build_eat_token(&lexer, ')', "expected ')' closing META(...) block");

                    if (parse_ok) {
                        clex_lex(&lexer);
                        parse_ok = lexer.token.kind == clex_Token_INCLUDE && string_equals(base_string_from_clex_data(lexer.token.data), S("include"));
                        if (!parse_ok) build_log_error(&lexer, "expected `#include`");
                    }

                    if (parse_ok) {
                        bool include_ok = false;
                        clex_Include include = clex_include_operand(lexer.token.data2, &include_ok);
                        parse_ok = include_ok && !include.is_system;
                        if (parse_ok) metaprogram_output_file = base_string_from_clex_data(include.path);
                        else build_log_error(&lexer, "expected string after `#include`");
                    }
                } else if (string_equals(token_data, S("embed"))) {
                    parse_ok = build_eat_token(&lexer, clex_Token_STRING, "expected path, like this: `META(embed \"path/to/file\")`");
                    String input_file = base_string_from_clex_data(lexer.token.data);
                    parse_ok = parse_ok && build_eat_token(&lexer, ')', "expected ')' closing META(...) block");

                    if (parse_ok) {
                        const char *input_path = cstring_from_string(scratch.arena, input_file);
                        String contents = os_read_entire_file(scratch.arena, input_path, &parse_ok);
                        if (!parse_ok) build_log_error(&lexer, "failed reading `META(embed)` input '%s'", input_path);

                        if (parse_ok) {
                            files_to_embed = arena_array_reserve(scratch.arena, files_to_embed, files_to_embed_count, &files_to_embed_capacity, files_to_embed_count + 1);
                            files_to_embed[files_to_embed_count] = (Build_Embedded_File){ .name = input_file, .contents = contents };
                            files_to_embed_count += 1;
                        }
                    }
                } else if (string_equals(token_data, S("subsystem"))) {
                    parse_ok = settings_this_pass.subsystem_windows == Build_Meta_Setting_UNSET;
                    if (!parse_ok) build_log_error(&lexer, "you can't specify `subsystem` multiple times");

                    const char *error = "expected `subsystem = console/windows`";
                    parse_ok = parse_ok && build_eat_token(&lexer, '=', error);
                    parse_ok = parse_ok && build_eat_token(&lexer, clex_Token_IDENTIFIER, error);

                    if (parse_ok) {
                        token_data = base_string_from_clex_data(lexer.token.data);
                        if (string_equals(token_data, S("console"))) {
                            settings_this_pass.subsystem_windows = Build_Meta_Setting_FALSE;
                        } else if (string_equals(token_data, S("windows"))) {
                            settings_this_pass.subsystem_windows = Build_Meta_Setting_TRUE;
                        } else {
                            build_log_error(&lexer, "expected `console` or `windows` in `META(subsystem = console/windows)`, got `%S`", token_data);
                            parse_ok = false;
                        }
                    }

                    parse_ok = parse_ok && build_eat_token(&lexer, ')', "expected `)` to close `META(...)` directive");

                    if (parse_ok && target_os == BASE_OS_WINDOWS) {
                        bool is_windows = settings_this_pass.subsystem_windows == Build_Meta_Setting_TRUE;
                        build_set_macro_from_meta(&lexer, macros, "subsystem", "WINDOWS_SUBSYSTEM", true, is_windows ? "WINDOWS_SUBSYSTEM_WINDOWS" : "WINDOWS_SUBSYSTEM_CONSOLE", &parse_ok);
                    }
                } else if (string_equals(token_data, S("link_libc"))) {
                    parse_ok = settings_this_pass.link_crt == Build_Meta_Setting_UNSET;
                    if (!parse_ok) build_log_error(&lexer, "you can't specify `link_libc` multiple times");

                    const char *error = "expected `link_libc = true/false`";
                    parse_ok = parse_ok && build_eat_token(&lexer, '=', error);
                    parse_ok = parse_ok && build_eat_token(&lexer, clex_Token_IDENTIFIER, error);

                    if (parse_ok) {
                        token_data = base_string_from_clex_data(lexer.token.data);
                        if (string_equals(token_data, S("true"))) {
                            settings_this_pass.link_crt = Build_Meta_Setting_TRUE;
                        } else if (string_equals(token_data, S("false"))) {
                            settings_this_pass.link_crt = Build_Meta_Setting_FALSE;
                        } else {
                            build_log_error(&lexer, "expected `true` or `false` in `META(link_libc = true/false)`, got `%S`", token_data);
                            parse_ok = false;
                        }
                    }

                    parse_ok = parse_ok && build_eat_token(&lexer, ')', "expected `)` to close `META(...)` directive");

                    bool links_crt = settings_this_pass.link_crt == Build_Meta_Setting_TRUE;
                    if (parse_ok && links_crt && target_os == BASE_OS_WASM) {
                        build_log_error(&lexer, "`META(link_libc = true)` is not possible on wasm, which has no libc");
                        parse_ok = false;
                    }

                    if (parse_ok) build_set_macro_from_meta(&lexer, macros, "link_libc", "LINK_CRT", true, links_crt ? "1" : "0", &parse_ok);
                    if (parse_ok && compiler == Build_Compiler_MSVC) {
                        bool uses_address_sanitiser = links_crt && build_mode == Build_Mode_DEBUG;
                        build_set_macro_from_meta(&lexer, macros, "link_libc", "__SANITIZE_ADDRESS__", uses_address_sanitiser, "1", &parse_ok);
                    }
                } else if (string_equals(token_data, S("link"))) {
                    parse_ok = build_eat_token(&lexer, clex_Token_STRING, "expected `META(link \"Library\")`");
                    String library = base_string_from_clex_data(lexer.token.data);

                    if (parse_ok && library.count == 0) {
                        build_log_error(&lexer, "link library name is empty");
                        parse_ok = false;
                    }

                    if (parse_ok) {
                        const char *library_cstring = cstring_from_string(scratch.arena, library);
                        bool is_already_linked = false;
                        for (u64 i = 0; i < link_libraries.count && !is_already_linked; i += 1) {
                            is_already_linked = cstring_equals_case_insensitive(link_libraries.items[i], library_cstring);
                        }
                        if (!is_already_linked) build_push_argument(&link_libraries, library_cstring);
                    }
                } else if (string_equals(token_data, S("compile_flag"))) {
                    parse_ok = build_eat_token(&lexer, clex_Token_STRING, "expected `META(compile_flag \"...\")`");
                    String flag = base_string_from_clex_data(lexer.token.data);

                    if (parse_ok && flag.count == 0) {
                        build_log_error(&lexer, "extra compile flag is empty string");
                        parse_ok = false;
                    }

                    if (parse_ok && build_flag_changes_preprocessor(flag)) {
                        // TODO(felix): Let compile flags define macros.
                        build_log_error(&lexer, "compile flag `%S` defines macros or sets the C standard, which `META(...)` does not support yet", flag);
                        parse_ok = false;
                    }

                    if (parse_ok) build_push_argument(&compiler_extra_flags, cstring_from_string(scratch.arena, flag));
                } else if (string_equals(token_data, S("link_flag"))) {
                    parse_ok = build_eat_token(&lexer, clex_Token_STRING, "expected `META(link_flag \"...\")`");
                    String flag = base_string_from_clex_data(lexer.token.data);

                    if (parse_ok && flag.count == 0) {
                        build_log_error(&lexer, "extra linker flag is empty string");
                        parse_ok = false;
                    }

                    if (parse_ok) build_push_argument(&linker_extra_flags, cstring_from_string(scratch.arena, flag));
                } else if (string_equals(token_data, S("compile_and_link_flag"))) {
                    parse_ok = build_eat_token(&lexer, clex_Token_STRING, "expected `META(compile_and_link_flag \"...\")`");
                    String flag = base_string_from_clex_data(lexer.token.data);

                    if (parse_ok && flag.count == 0) {
                        build_log_error(&lexer, "extra compile and link flag is empty string");
                        parse_ok = false;
                    }

                    if (parse_ok && build_flag_changes_preprocessor(flag)) {
                        // TODO(felix): Let compile flags define macros.
                        build_log_error(&lexer, "compile flag `%S` defines macros or sets the C standard, which `META(...)` does not support yet", flag);
                        parse_ok = false;
                    }

                    if (parse_ok) {
                        const char *as_cstring = cstring_from_string(scratch.arena, flag);
                        build_push_argument(&linker_extra_flags, as_cstring);
                        build_push_argument(&compiler_extra_flags, as_cstring);
                    }
                } else {
                    build_log_error(&lexer, "unsupported metaprogram directive '%S'", token_data);
                    parse_ok = false;
                }
            }
        }
        bool lexing_failed = parse_ok && lexer.token.kind != clex_Token_END_OK;
        if (lexing_failed) {
            const clex_Cursor *cursor = &lexer.stack[lexer.depth];
            const char *text_end = cursor->c;
            while (text_end < cursor->end && text_end - cursor->c < 40 && *text_end != '\n' && *text_end != '\r') text_end += 1;
            String text = string_from_pointers(cursor->c, text_end);
            build_log_error(&lexer, "%s: `%S`", lexer.error_message, text);
            parse_ok = false;
        }

        if (parse_ok && conditional_depth != 0) {
            build_log_error(&lexer, "%u `#if`s are missing their `#endif`", conditional_depth);
            parse_ok = false;
        }

        if (parse_ok) {
            assert(settings_from_earlier_passes.link_crt == Build_Meta_Setting_UNSET || settings_this_pass.link_crt == settings_from_earlier_passes.link_crt);
            assert(settings_from_earlier_passes.subsystem_windows == Build_Meta_Setting_UNSET || settings_this_pass.subsystem_windows == settings_from_earlier_passes.subsystem_windows);
        }

        bool link_crt_was_set_this_pass = settings_this_pass.link_crt != settings_from_earlier_passes.link_crt;
        bool subsystem_was_set_this_pass = settings_this_pass.subsystem_windows != settings_from_earlier_passes.subsystem_windows;
        bool another_pass_can_fix_errors = meta_in_unknown_group_count != 0 && (link_crt_was_set_this_pass || subsystem_was_set_this_pass);
        parsing = parse_ok && another_pass_can_fix_errors;

        bool has_final_errors = parse_ok && !parsing && meta_in_unknown_group_count != 0;
        if (has_final_errors && !collect_unknown_reasons) {
            for (u64 i = 0; i < meta_in_unknown_group_count; i += 1) {
                const Build_Meta_In_Unknown_Group *meta = &metas_in_unknown_groups[i];
                if (link_libc_meta_location == 0 && string_equals(meta->directive, S("link_libc"))) link_libc_meta_location = meta->location;
                if (subsystem_meta_location == 0 && string_equals(meta->directive, S("subsystem"))) subsystem_meta_location = meta->location;
            }
            collect_unknown_reasons = true;
            parsing = true;
        } else if (has_final_errors) {
            for (u64 i = 0; i < meta_in_unknown_group_count; i += 1) {
                const Build_Meta_In_Unknown_Group *meta = &metas_in_unknown_groups[i];
                bool sets_link_crt = string_equals(meta->directive, S("link_libc"));
                bool sets_subsystem = string_equals(meta->directive, S("subsystem"));

                bool sets_own_condition = false;
                for (Build_Unknown_Reason_Id reason = meta->unknown_reason; reason != 0; reason = macros->reasons.items[reason].cause) {
                    bool reason_is_link_crt = reason == link_crt_unset_reason || reason == address_sanitiser_unset_reason;
                    bool reason_is_subsystem = reason == subsystem_unset_reason;
                    sets_own_condition = sets_own_condition || (sets_link_crt && reason_is_link_crt) || (sets_subsystem && reason_is_subsystem);
                }

                const char *message = "`META(...)` is in an unknown group, so the metaprogram doesn't know if the compiler builds it";
                if (sets_own_condition) message = cstring_print(scratch.arena, "`META(%S)` sets the condition of its own group; this is an unrecoverable cycle", meta->directive);
                print("%s: error: %s\n", meta->location, message);
                for (Build_Unknown_Reason_Id reason = meta->unknown_reason; reason != 0; reason = macros->reasons.items[reason].cause) {
                    print("    because %s\n", macros->reasons.items[reason].description);
                }
            }
            parse_ok = false;
        }

        settings_from_earlier_passes = settings_this_pass;
    }

    if (parse_ok) {
        f64 parse_seconds = time_seconds_between_relative_timestamps(time_at_parse_start, time_relative_timestamp());
        print("Parsing took %.2lf seconds in %u pass%s.\n", parse_seconds, pass_count, pass_count > 1 ? "es" : "");
    }

    if (parse_ok && files_to_embed_count > 0) {
        object_Bundle_Flags flags = object_Bundle_READ;
        switch (target_os) {
            case BASE_OS_WINDOWS: flags |= object_Bundle_COFF; break;
            // TODO(felix): Handle iOS separately from iOS-simulator.
            case BASE_OS_IOS_DEVICE: flags |= object_Bundle_MACHO_IOS; break;
            case BASE_OS_IOS_SIMULATOR: flags |= object_Bundle_MACHO_IOS_SIMULATOR; break;
            case BASE_OS_MACOS: flags |= object_Bundle_MACHO_MACOS; break;
            case BASE_OS_WASM: flags |= object_Bundle_WASM; break;
            default: {
                log_error("`META(embed)` is not supported for this target");
                parse_ok = false;
            } break;
        }

        u64 entries_offset = sizeof(u64);
        u64 names_offset = entries_offset + files_to_embed_count * sizeof(Embedded_File_Entry);
        u64 table_size = names_offset;
        u64 terminator_size = 1;
        for (u64 i = 0; i < files_to_embed_count; i += 1) table_size += files_to_embed[i].name.count + terminator_size + files_to_embed[i].contents.count + terminator_size;
        if (table_size > UINT32_MAX) {
            log_error("embedded files total %llu bytes, but object files support at most %u", table_size, UINT32_MAX);
            parse_ok = false;
        }

        if (parse_ok) {
            u8 *table = arena_make(scratch.arena, size_t_from_u64(table_size), u8);
            memcpy(table, &files_to_embed_count, sizeof files_to_embed_count);

            u64 cursor = names_offset;
            for (u64 i = 0; i < files_to_embed_count; i += 1) {
                Build_Embedded_File *file = &files_to_embed[i];
                Embedded_File_Entry entry = { .name_offset = cursor };
                memcpy(table + cursor, file->name.data, file->name.count);
                cursor += file->name.count;
                table[cursor] = 0;
                cursor += terminator_size;

                entry.data_offset = cursor;
                entry.data_size = file->contents.count;
                memcpy(table + cursor, file->contents.data, file->contents.count);
                cursor += file->contents.count;
                table[cursor] = 0;
                cursor += terminator_size;

                memcpy(table + entries_offset + i * sizeof entry, &entry, sizeof entry);
            }
            assert(cursor == table_size);

            const char array_name[] = "embedded_files";
            u64 byte_count = 0;
            u8 *to_write = object_bundle(array_name, sizeof array_name - 1, table, (u32)table_size, 0, &byte_count, flags);
            assert(to_write == 0 && byte_count > 0);

            u8 *bytes = arena_make(scratch.arena, size_t_from_u64(byte_count), u8);
            to_write = object_bundle(array_name, sizeof array_name - 1, table, (u32)table_size, bytes, &byte_count, flags);

            const char *output_name = cstring_print(scratch.arena, "meta.generated.%s", object_extension);
            const char *output_in_build_folder = cstring_print(scratch.arena, "%s/%s", build_folder, output_name);
            parse_ok = to_write != 0 && os_write_entire_file(output_in_build_folder, bytes, byte_count);
            if (!parse_ok) log_error("failure writing the object file for embedded files to '%s'", output_in_build_folder);

            build_objects_push(&objects, (Build_Object){ .object_path = output_name });
        }
    }

    bool metaprogram_output_ok = parse_ok;
    if (metaprogram_output_ok && metaprogram_code.count > 0) {
        if (metaprogram_output_file.count == 0) {
            log_error("missing `META(%S)` followed by #include to designate codegen destination", generate_here_directive);
            metaprogram_output_ok = false;
        } else {
            const char *file = cstring_from_string(scratch.arena, metaprogram_output_file);
            metaprogram_output_ok = os_write_entire_file(file, metaprogram_code.data, metaprogram_code.count);
        }
    }

    build_objects_push(&objects, (Build_Object){ .code_path = c_file_path });

    bool link_crt = settings_from_earlier_passes.link_crt == Build_Meta_Setting_TRUE;
    if (settings_from_earlier_passes.link_crt == Build_Meta_Setting_UNSET) link_crt = !!(target_os & BASE_OS_ANY_APPLE);
    bool subsystem_is_windows = settings_from_earlier_passes.subsystem_windows != Build_Meta_Setting_FALSE;

    // TODO(felix): should be handled by metaprogram
    const char *include_paths[] = {
        cstring_print(scratch.arena, "-I%s", dependency_folder),
        cstring_print(scratch.arena, "-I%s", source_folder),
        cstring_print(scratch.arena, "-I..%c", path_separator),
        0,
    };

    Build_Linker linker = compiler == Build_Compiler_CLANG ? Build_Linker_CLANG : Build_Linker_MSVC;
    bool use_asan = link_crt && build_mode == Build_Mode_DEBUG;

    // NOTE(felix): MSVC and LLVM ship incompatible clang_rt.asan_dynamic-x86_64.dll, so the executable must find the one from its own compiler before any on PATH.
    if (use_asan && target_os == BASE_OS_WINDOWS) {
        String runtime_folder = {0};
        u32 query_exit_code = 1;
        switch (compiler) {
            case Build_Compiler_CLANG: {
                const char *query[] = { "clang", "-print-resource-dir", 0 };
                Os_Process_Result result = os_process_run(scratch.arena, query, 0, Os_Process_Flag_CAPTURE_OUTPUT);
                query_exit_code = result.exit_code;
                String resource_folder = result.output;
                while (resource_folder.count > 0 && ascii_is_whitespace(resource_folder.data[resource_folder.count - 1])) resource_folder.count -= 1;
                runtime_folder = string_print(scratch.arena, "%S\\lib\\windows", resource_folder);
            } break;
            case Build_Compiler_MSVC: {
                const char *query[] = { "where", "cl", 0 };
                Os_Process_Result result = os_process_run(scratch.arena, query, 0, Os_Process_Flag_CAPTURE_OUTPUT);
                query_exit_code = result.exit_code;
                String compiler_paths = result.output;
                u64 first_path_length = 0;
                while (first_path_length < compiler_paths.count && compiler_paths.data[first_path_length] != '\r' && compiler_paths.data[first_path_length] != '\n') first_path_length += 1;
                String first_compiler_path = string_range(compiler_paths, 0, first_path_length);
                runtime_folder = string_cut_last(first_compiler_path, '\\').head;
            } break;
            default: unreachable;
        }

        const char *runtime_name = "clang_rt.asan_dynamic-x86_64.dll";
        const char *runtime_source = cstring_print(scratch.arena, "%S\\%s", runtime_folder, runtime_name);
        const char *runtime_destination = cstring_print(scratch.arena, "%s\\%s", build_folder, runtime_name);

        bool read_ok = false;
        String runtime = query_exit_code == 0 ? os_read_entire_file(scratch.arena, runtime_source, &read_ok) : (String){0};
        bool copy_ok = read_ok && os_write_entire_file(runtime_destination, runtime.data, runtime.count);
        if (!copy_ok) log_error("unable to copy the address sanitiser runtime from '%s' to '%s'", runtime_source, runtime_destination);
    }

    Os_Process_Flags process_flags =
        Os_Process_Flag_PRINT_COMMAND_BEFORE_RUNNING |
        Os_Process_Flag_PRINT_EXIT_CODE |
        Os_Process_Flag_PRINT_EXECUTION_TIME;

    bool compile_ok = metaprogram_output_ok;
    for (u64 i = 0; compile_ok && i < objects.count; i += 1) {
        Build_Object *object = &objects.items[i];

        bool should_compile = object->code_path != 0;

        if (should_compile) {
            bool no_custom_object_path = object->object_path == 0;
            if (no_custom_object_path) {
                String code_path = string_from_cstring(object->code_path);
                String basename_with_path = string_cut_last(code_path, '.').head;
                String basename_no_path = string_cut_last(basename_with_path, path_separator).tail;

                object->object_path = cstring_print(scratch.arena, "%S.%s", basename_no_path, object_extension);
            }

            Build_Arguments compile = {0};

            build_push_arguments(&compile, build_compiler_initial_command[compiler]);

            build_push_arguments(&compile, compiler_extra_flags.items);

            // TODO(felix): should be handled by reading META() annotations
            if (target_os == BASE_OS_WASM) {
                assert(compiler == Build_Compiler_CLANG);

                const char *clang_wasm_flags[BUILD_FLAGS_MAX] = {
                    "--target=wasm32",
                    0,
                };

                build_push_arguments(&compile, clang_wasm_flags);
            }

            build_push_arguments(&compile, include_paths);
            build_push_argument(&compile, link_crt ? "-DLINK_CRT=1" : "-DLINK_CRT=0");
            if (target_os == BASE_OS_WINDOWS) {
                build_push_argument(&compile, subsystem_is_windows ? "-DWINDOWS_SUBSYSTEM=WINDOWS_SUBSYSTEM_WINDOWS" : "-DWINDOWS_SUBSYSTEM=WINDOWS_SUBSYSTEM_CONSOLE");
            }

            if (!object->no_extra_errors) build_push_arguments(&compile, build_compiler_extra_errors[compiler]);
            build_push_arguments(&compile, build_compiler_disabled_errors[compiler]);

            build_push_arguments(&compile, build_compiler_mode_flags[build_mode][compiler]);
            build_push_argument(&compile, build_mode == Build_Mode_DEBUG ? "-DBUILD_DEBUG=1" : "-DBUILD_DEBUG=0");
            if (use_asan) build_push_argument(&compile, "-fsanitize=address");

            build_push_arguments(&compile, object->extra_flags);

            bool delete_pdb_first_because_msvc_incremental_is_broken = (target_os == BASE_OS_WINDOWS && compiler == Build_Compiler_MSVC);
            if (delete_pdb_first_because_msvc_incremental_is_broken) {
                String object_path = string_from_cstring(object->object_path);
                String object_basename = string_cut_last(object_path, '.').head;
                object_basename = string_cut_last(object_basename, path_separator).tail;

                const char *pdb_file = cstring_print(scratch.arena, "%S.pdb", object_basename);
                if (fs_exists(pdb_file)) {
                    ok = fs_remove_file(pdb_file);
                    if (!ok) log_internal("warning: failure removing old PDB '%s'", pdb_file);
                }
            }

            build_push_argument(&compile, object->code_path);

            const char *output = cstring_print(scratch.arena, "%s%s", build_compiler_out[compiler], object->object_path);
            build_push_argument(&compile, output);

            exit_code = os_process_run(scratch.arena, compile.items, build_folder, process_flags).exit_code;
            compile_ok = exit_code == 0;
        }
    }

    bool link_attempted = false;
    if (compile_ok) {
        Build_Arguments link = {0};

        build_push_arguments(&link, build_linker_initial_command[linker][target_os]);

        if (target_os == BASE_OS_WINDOWS) {
            const char *subsystem = subsystem_is_windows ? "windows" : "console";
            const char *entry = link_crt ? "mainCRTStartup" : "entrypoint";

            if (linker == Build_Linker_MSVC) {
                build_push_argument(&link, cstring_print(scratch.arena, "-subsystem:%s", subsystem));
                build_push_argument(&link, cstring_print(scratch.arena, "-entry:%s", entry));
                if (build_mode == Build_Mode_DEBUG) {
                    build_push_argument(&link, "-debug");
                    if (link_crt) build_push_argument(&link, "libucrtd.lib");
                } else {
                    build_push_argument(&link, "-opt:ref");
                    build_push_argument(&link, "-opt:icf");
                    if (link_crt) build_push_argument(&link, "libucrt.lib");
                }
            } else {
                build_push_argument(&link, cstring_print(scratch.arena, "-Wl,-subsystem:%s", subsystem));
                build_push_argument(&link, cstring_print(scratch.arena, "-Wl,-entry:%s", entry));
                if (build_mode == Build_Mode_DEBUG) build_push_argument(&link, "-g");
            }
        }

        if (linker == Build_Linker_CLANG && use_asan) build_push_argument(&link, "-fsanitize=address");
        if (target_os == BASE_OS_WASM && build_mode == Build_Mode_RELEASE) build_push_argument(&link, "-Wl,--strip-all");

        for (u64 i = 0; i < objects.count; i += 1) build_push_argument(&link, objects.items[i].object_path);

        const char *link_prefix = "";
        const char *link_suffix = "";
        switch (linker) {
            case Build_Linker_MSVC: link_suffix = ".lib"; break;
            case Build_Linker_CLANG: link_prefix = "-l"; break;
            default: unreachable;
        }
        for (u64 i = 0; i < link_libraries.count; i += 1) {
            const char *library = link_libraries.items[i];
            const char *to_push = cstring_print(scratch.arena, "%s%s%s", link_prefix, library, link_suffix);
            build_push_argument(&link, to_push);
        }

        build_push_arguments(&link, linker_extra_flags.items);

        const char *syntax = target_os == BASE_OS_WINDOWS && (linker == Build_Linker_MSVC) ? "-out:" : "-o";
        const char *extension = build_binary_extension[target_os];

        const char *output_name = target_os == BASE_OS_WASM ? "module" : program_name;
        const char *output = 0;
        bool app_folder_ok = true;
        if (target_os & BASE_OS_ANY_APPLE) {
            const char *app_folder = cstring_print(scratch.arena, "%s%c%s.app", build_folder, path_separator, output_name);
            if (!fs_exists(app_folder)) {
                app_folder_ok = fs_make_folder(app_folder);
                if (!app_folder_ok) log_error("failure making folder '%s'", app_folder);
            }
            output = cstring_print(scratch.arena, "%s%s.app%c%s", syntax, output_name, path_separator, output_name);
        }
        else output = cstring_print(scratch.arena, "%s%s.%s", syntax, output_name, extension);
        build_push_argument(&link, output);

        if (app_folder_ok) {
            exit_code = os_process_run(scratch.arena, link.items, build_folder, process_flags).exit_code;
            link_attempted = true;
        } else exit_code = 1;

        if (target_os == BASE_OS_WASM && exit_code == 0) {
            const char *glue_path = cstring_print(scratch.arena, "%sbase%cbase_platform_wasm.js", &source_folder[3], path_separator);
            bool glue_ok = false;
            String glue = os_read_entire_file(scratch.arena, glue_path, &glue_ok);
            if (!glue_ok) log_error("failed reading '%s'", glue_path);

            bool glue_written = false;
            const char *glue_output_path = cstring_print(scratch.arena, "%s%cmodule.js", build_folder, path_separator);
            if (glue_ok) {
                fs_remove_file(glue_output_path);
                glue_written = os_write_entire_file(glue_output_path, glue.data, glue.count);

                if (glue_written) print("wrote '%s'\n", glue_output_path);
                else print("unable to write '%s'\n", glue_output_path);
            }

            const char *page_path = cstring_print(scratch.arena, "%sbase%cbase_platform_wasm.html", &source_folder[3], path_separator);
            bool page_ok = false;
            String page = {0};
            if (glue_written) {
                page = os_read_entire_file(scratch.arena, page_path, &page_ok);
                if (!page_ok) log_error("failed reading '%s'", page_path);
            }

            bool html_ok = false;
            const char *html_output_path = cstring_print(scratch.arena, "%s%c%s.html", build_folder, path_separator, program_name);
            if (page_ok) {
                fs_remove_file(html_output_path);
                html_ok = os_write_entire_file(html_output_path, page.data, page.count);

                if (html_ok) print("wrote '%s'\n", html_output_path);
                else print("unable to write '%s'\n", html_output_path);
            }
            exit_code = !html_ok;
        }
    }

    if (link_attempted && (target_os & BASE_OS_ANY_IOS)) {
        ok = false;
        const char *bundle_identifier = 0;
        if (exit_code == 0) {
            const char *plist_format = STRINGIFY(
                <?xml version="1.0" encoding="UTF-8"?>
                <!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN"
                  "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
                <plist version="1.0">
                <dict>
                    <key>CFBundleName</key>               <string>%s</string>
                    <key>CFBundleDisplayName</key>        <string>%s</string>
                    <key>CFBundleExecutable</key>         <string>%s</string>
                    <key>CFBundleIdentifier</key>         <string>%s</string>
                    <key>CFBundleVersion</key>            <string>1</string>
                    <key>CFBundleShortVersionString</key> <string>1.0</string>
                    <key>CFBundlePackageType</key>        <string>APPL</string>
                    <key>MinimumOSVersion</key>           <string>16.0</string>
                    <key>UIDeviceFamily</key>             <array><integer>1</integer></array>
                    <key>UILaunchScreen</key>             <dict/>
                    <key>UISupportedInterfaceOrientations</key>
                    <array><string>UIInterfaceOrientationPortrait</string></array>
                    <key>CFBundleSupportedPlatforms</key>
                    <array><string>iPhoneSimulator</string></array>
                </dict>
                </plist>
            );

            // TODO(felix): Programmatically get user instead of hardcoding to "felix".
            bundle_identifier = cstring_print(scratch.arena, "com.felix.%c%s", program_name[0] | 0x20, &program_name[1]);
            String plist = string_print(scratch.arena, plist_format, program_name, program_name, program_name, bundle_identifier);

            const char *path = cstring_print(scratch.arena, "%s%c%s.app%cInfo.plist", build_folder, path_separator, program_name, path_separator);

            ok = os_write_entire_file(path, plist.data, plist.count);
            if (!ok) {
                log_error("failure writing '%s' (%llu bytes)", path, plist.count);
            } else print("Wrote %llu bytes to '%s'.\n", plist.count, path);
        }

        if (ok) {

        }

        if (ok) {
            if (target_os == BASE_OS_IOS_SIMULATOR) {
                print("You can run: xcrun simctl install booted %s%c%s.app && "
                    "xcrun simctl launch --terminate-running-process --console booted %s\n",
                    build_folder, path_separator, program_name, bundle_identifier
                );
            } else {
                assert(target_os == BASE_OS_IOS_DEVICE);
                print("You can run: UDID=<UDID>; xcrun devicectl device install app -d $UDID %s%c%s.app && "
                    "xcrun devicectl device process launch --terminate-existing --console -d $UDID %s",
                    build_folder, path_separator, program_name, bundle_identifier
                );
            }
        }

        exit_code = ok ? 0 : 1;
    }

    scratch_end(scratch);
    return exit_code;
}

static void app_frame_info_reset_end_of_frame(App_Frame_Info *frame) {
    frame->scroll = 0;
    memset(frame->mouse_clicked, 0, sizeof frame->mouse_clicked);
    memset(frame->key_pressed, 0, sizeof frame->key_pressed);
}

static void base_platform_reserve_frame_arena(Platform *platform) {
    bool app_start_didnt_allocate_from_platform_arenas = platform->persistent_arena->memory == 0 && platform->frame_arena->memory == 0;
    assert(app_start_didnt_allocate_from_platform_arenas);

    u64 frame_arena_reservation = platform->frame_arena_startup_reservation_in_bytes;
    if (frame_arena_reservation == 0) frame_arena_reservation = ARENA_DEFAULT_RESERVE_SIZE;
    *platform->frame_arena = arena_reserve(size_t_from_u64(frame_arena_reservation));
}

typedef struct {
    u64 key;
    u64 last_used_frame;
    u32 generation;
    u32 next_in_bucket;
    u32 older_slot;
    u32 newer_slot;
    bool is_used;
    bool is_keyed_by_content;
    bool must_not_be_evicted;
} GPU_Cache_Entry;

typedef struct {
    GPU_Cache_Entry *entries;
    u32 *first_slot_from_bucket;
    u32 exponent;
    u32 oldest_slot;
    u32 newest_slot;
} GPU_Cache;

static u32 gpu__cache_bucket(const GPU_Cache *cache, u64 key) {
    return (u32)(key ^ (key >> 32)) & ((1u << cache->exponent) - 1);
}

static u32 gpu__cache_find(const GPU_Cache *cache, u64 key, bool is_keyed_by_content) {
    u32 found_slot = 0;
    u32 slot = cache->first_slot_from_bucket[gpu__cache_bucket(cache, key)];
    while (slot != 0 && found_slot == 0) {
        const GPU_Cache_Entry *entry = &cache->entries[slot];
        if (entry->key == key && entry->is_keyed_by_content == is_keyed_by_content) found_slot = slot;
        slot = entry->next_in_bucket;
    }
    return found_slot;
}

static void gpu__cache_link_as_newest(GPU_Cache *cache, u32 slot) {
    GPU_Cache_Entry *entry = &cache->entries[slot];
    entry->older_slot = cache->newest_slot;
    entry->newer_slot = 0;
    if (cache->newest_slot != 0) cache->entries[cache->newest_slot].newer_slot = slot;
    else cache->oldest_slot = slot;
    cache->newest_slot = slot;
}

static void gpu__cache_unlink(GPU_Cache *cache, u32 slot) {
    GPU_Cache_Entry *entry = &cache->entries[slot];
    if (entry->older_slot != 0) cache->entries[entry->older_slot].newer_slot = entry->newer_slot;
    else cache->oldest_slot = entry->newer_slot;
    if (entry->newer_slot != 0) cache->entries[entry->newer_slot].older_slot = entry->older_slot;
    else cache->newest_slot = entry->older_slot;
}

static u32 gpu__cache_insert(GPU_Cache *cache, u64 key, bool is_keyed_by_content, u64 frame_index) {
    u32 slot = cache->oldest_slot;
    GPU_Cache_Entry *entry = &cache->entries[slot];
    assert(!entry->is_used);

    u32 bucket = gpu__cache_bucket(cache, key);
    entry->key = key;
    entry->last_used_frame = frame_index;
    entry->is_used = true;
    entry->is_keyed_by_content = is_keyed_by_content;
    entry->next_in_bucket = cache->first_slot_from_bucket[bucket];
    cache->first_slot_from_bucket[bucket] = slot;
    gpu__cache_unlink(cache, slot);
    gpu__cache_link_as_newest(cache, slot);
    return slot;
}

static GPU_Cache gpu__cache_make(Arena *persistent_arena, u32 exponent) {
    u32 capacity = 1u << exponent;
    GPU_Cache cache = {
        .entries = arena_make(persistent_arena, capacity + 1, GPU_Cache_Entry),
        .first_slot_from_bucket = arena_make(persistent_arena, capacity, u32),
        .exponent = exponent,
    };
    for (u32 slot = 1; slot <= capacity; slot += 1) gpu__cache_link_as_newest(&cache, slot);
    return cache;
}

static void gpu__cache_mark_used(GPU_Cache *cache, u32 slot, u64 frame_index) {
    GPU_Cache_Entry *entry = &cache->entries[slot];
    if (entry->last_used_frame != frame_index) {
        gpu__cache_unlink(cache, slot);
        gpu__cache_link_as_newest(cache, slot);
        entry->last_used_frame = frame_index;
    }
}

static u32 gpu__cache_oldest_evictable_slot(const GPU_Cache *cache) {
    u32 slot = cache->oldest_slot;
    while (slot != 0 && (!cache->entries[slot].is_used || cache->entries[slot].must_not_be_evicted)) slot = cache->entries[slot].newer_slot;
    return slot;
}

#define GPU_MAX_MESHES_EXPONENT 12
#define GPU_MAX_SHADERS_EXPONENT 8
#define GPU_MAX_TEXTURES_EXPONENT 10
#define GPU_ID_SLOT_BITS 20
#define GPU_ID_GENERATION_MASK ((1u << (32 - GPU_ID_SLOT_BITS)) - 1)
_Static_assert(GPU_MAX_MESHES_EXPONENT < GPU_ID_SLOT_BITS && GPU_MAX_SHADERS_EXPONENT < GPU_ID_SLOT_BITS && GPU_MAX_TEXTURES_EXPONENT < GPU_ID_SLOT_BITS, "every cache slot fits in an Id");
static void gpu__cache_remove(GPU_Cache *cache, u32 slot) {
    GPU_Cache_Entry *entry = &cache->entries[slot];

    u32 *link = &cache->first_slot_from_bucket[gpu__cache_bucket(cache, entry->key)];
    while (*link != slot) link = &cache->entries[*link].next_in_bucket;
    *link = entry->next_in_bucket;

    gpu__cache_unlink(cache, slot);
    entry->older_slot = 0;
    entry->newer_slot = cache->oldest_slot;
    if (cache->oldest_slot != 0) cache->entries[cache->oldest_slot].older_slot = slot;
    else cache->newest_slot = slot;
    cache->oldest_slot = slot;
    entry->is_used = false;
    entry->generation = (entry->generation + 1) & GPU_ID_GENERATION_MASK;
}

static u32 gpu__id_from_slot(const GPU_Cache *cache, u32 slot) {
    return (cache->entries[slot].generation << GPU_ID_SLOT_BITS) | slot;
}

static u32 gpu__slot_from_id(const GPU_Cache *cache, u32 id, bool *ok) {
    u32 slot = id & ((1u << GPU_ID_SLOT_BITS) - 1);
    u32 generation = id >> GPU_ID_SLOT_BITS;
    u32 capacity = 1u << cache->exponent;

    bool slot_is_in_range = slot != 0 && slot <= capacity;
    u32 checked_slot = slot_is_in_range ? slot : 0;
    const GPU_Cache_Entry *entry = &cache->entries[checked_slot];
    bool is_valid = slot_is_in_range && entry->is_used && entry->generation == generation;

    *ok = is_valid;
    return is_valid ? slot : 0;
}

#define UI_ASSERT assert
#define UI_VSNPRINTF small_vsnprintf
#define ui_size_t size_t
#define UI_IMPLEMENTATION
#include "base/ui.h"

static ui_Box *base_ui_begin(ui_State *ui, App_Frame_Info *frame) {
    ui->window_size[ui_X] = frame->window_size.x - frame->safe_area_inset_left - frame->safe_area_inset_right;
    ui->window_size[ui_Y] = frame->window_size.y - frame->safe_area_inset_top - frame->safe_area_inset_bottom;
    assert(ui->window_size[ui_X] != 0.f);
    assert(ui->window_size[ui_Y] != 0.f);

    ui->mouse_position[ui_X] = frame->mouse_position.x;
    ui->mouse_position[ui_Y] = frame->mouse_position.y;

    _Static_assert(sizeof ui->mouse_clicked == sizeof frame->mouse_clicked, "Base and UI types must be identical.");
    memcpy(ui->mouse_clicked, frame->mouse_clicked, sizeof ui->mouse_clicked);

    _Static_assert(sizeof ui->mouse_down == sizeof frame->mouse_down, "Base and UI types must be identical.");
    memcpy(ui->mouse_down, frame->mouse_down, sizeof ui->mouse_down);

    _Static_assert(sizeof ui->key_pressed == sizeof frame->key_pressed, "Base and UI types must be identical.");
    memcpy(ui->key_pressed, frame->key_pressed, sizeof ui->key_pressed);

    ui_begin(ui);
    ui_Box *root = ui_push(ui, 0, ui_Y, 0);
    root->rectangle.position[ui_X] = frame->safe_area_inset_left;
    root->rectangle.position[ui_Y] = frame->safe_area_inset_top;
    return root;
}

static void base_ui_create(ui_State *ui, Platform *platform) {
    ui_create(ui, base_ui_measure_text);
    ui->user_data = platform;
}

static void base_ui_render(ui_State *ui, ui_Box *box) {
    panic("TODO(felix): Support the new GPU API: draw the shadow, background, border and text of each visible box, clipped to the inside of its parent's border if ui_Flag_CLIP_TO_PARENT is set.");

    for (ui_Box *child = box->first_child; child != 0; child = child->next_sibling) {
        base_ui_render(ui, child);
    }
}

static void base_ui_measure_text(void *user_data, const char *string, ui_Style style, f32 size[2]) {
    String s = { .data = string, .count = cstring_length(string) };
    V2 measure = platform_measure_text(user_data, s, style.font_size);
    size[0] = measure.x;
    size[1] = measure.y;
}


#endif // defined(BASE_IMPLEMENTATION)
