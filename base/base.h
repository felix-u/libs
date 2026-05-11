#if !defined(BASE_H)
#define BASE_H


#include "base/base_context.h"

#define meta(...)

#include <stdarg.h> // NOTE(felix): doesn't seem like anything can be done about this one

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

#if BASE_C_VERSION < 2011
    // TODO(felix): is there a typedef hack we can do here?
    #define _Static_assert(expression, message)
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
    void exit(int); // TODO(felix): can remove?
    void abort(void); // TODO(felix): own implementation to not depend on libc
    void *calloc(size_t item_count, size_t item_size); // TODO(felix): remove once using virtual alloc arena
    void free(void *pointer); // TODO(felix): remove once using virtual alloc arena
    // TODO(felix): which of these can be removed in favour of doing direct syscalls?
        #include <fcntl.h>
        #include <unistd.h>
    #include <stdio.h> // TODO(felix): only needed for FILE. remove!
#elif BASE_OS == BASE_OS_MACOS
    // TODO(felix): same notes as above

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
    int     access(const char *path, int mode);
    void   *calloc(size_t item_count, size_t item_size);
    int     chdir(const char *path);
    int     clock_gettime(clockid_t, struct timespec *);
    int     execvp(const char *file, char *const argv[]);
    void    exit(int);
    int     fclose(FILE *file);
    FILE   *fopen(const char *path, const char *mode);
    pid_t   fork(void);
    size_t  fread(void *pointer, size_t size, size_t number_of_items, FILE *file);
    void    free(void *pointer);
    size_t  fwrite(const void *pointer, size_t size, size_t number_of_items, FILE *file);
    int     lstat(const char *path, struct stat *buffer);
    void   *malloc(size_t bytes);
    int     mkdir(const char *path, mode_t mode);
    int     pthread_create(pthread_t *, const pthread_attr_t *, void *(*start_routine)(void *), void *argument);
    char   *realpath(const char *file_name, char *resolved_name); //  __DARWIN_EXTSN(realpath); // NOTE(felix): `__DARWIN_EXTSN` needed to silence "error: cannot apply asm label to function after its first use"
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
    #if COMPILER_CLANG || COMPILER_MSVC
        #define WIN32_CALL(T) __declspec(dllimport) T __stdcall
    #endif

    #define os_abort() ExitProcess(1)
    #define os_exit(code) ExitProcess(code)

    // NOTE(felix): meta(link) does nothing yet, but by using it already I won't have to rewrite comments when it's implemented
    meta(link "Kernel32.lib")
    WIN32_CALL(int)           CloseHandle(void *);
    struct _SECURITY_ATTRIBUTES;
    WIN32_CALL(int)           CreateDirectoryA(const char *path, struct _SECURITY_ATTRIBUTES *);
    WIN32_CALL(void *)        CreateFileA(const char *file_name, unsigned long desired_access, unsigned long share_mode, struct _SECURITY_ATTRIBUTES *, unsigned long creation_disposition, unsigned long flags_and_attributes, void *template_file);
    #if !defined(_PROCESSTHREADSAPI_H_)
        typedef struct { // https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/ns-processthreadsapi-startupinfoa
            unsigned long cb;
            char *lpReserved;
            char *lpDesktop;
            char *lpTitle;
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
        } STARTUPINFOA;
        typedef struct { // https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/ns-processthreadsapi-process_information
            void *hProcess;
            void *hThread;
            unsigned long dwProcessId;
            unsigned long dwThreadId;
        } PROCESS_INFORMATION;
    #endif
    WIN32_CALL(int)           CreateProcessA(const char *application_name, char *command_line, struct _SECURITY_ATTRIBUTES *process_attributes, struct _SECURITY_ATTRIBUTES *thread_attributes, int should_inherit_handles, unsigned long creation_flags, void *environment, const char *current_directory, STARTUPINFOA *, PROCESS_INFORMATION *);
    WIN32_CALL(int)           DeleteFileA(const char *file_name);
    WIN32_CALL(void)          ExitProcess(unsigned exit_code);
    WIN32_CALL(int) FindClose(void *find_file_handle);
    typedef struct {
        unsigned long dwLowDateTime;
        unsigned long dwHighDateTime;
    } FILETIME;
    #define MAX_PATH 260
    typedef struct {
        unsigned long dwFileAttributes;
        FILETIME ftCreationTime;
        FILETIME ftLastAccessTime;
        FILETIME ftLastWriteTime;
        unsigned long nFileSizeHigh;
        unsigned long nFileSizeLow;
        unsigned long dwReserved0;
        unsigned long dwReserved1;
        char cFileName[MAX_PATH];
        char cAlternateFileName[14];
        unsigned long dwFileType; // Obsolete. Do not use.
        unsigned long dwCreatorType; // Obsolete. Do not use
        unsigned short wFinderFlags; // Obsolete. Do not use
    } WIN32_FIND_DATAA;
    WIN32_CALL(void *) FindFirstFileA(const char *file_name, WIN32_FIND_DATAA *);
    WIN32_CALL(int) FindNextFileA(void *find_file_handle, WIN32_FIND_DATAA *);
    WIN32_CALL(unsigned short *) GetCommandLineW(void);
    WIN32_CALL(int)           GetExitCodeProcess(void *process, unsigned long *exit_code);
    WIN32_CALL(unsigned long) GetFileAttributesA(const char *file_name);
    #if !defined(_WINNT_)
        typedef long long LARGE_INTEGER;
    #endif
    WIN32_CALL(int)           GetFileSizeEx(void *file, LARGE_INTEGER *file_size);
    WIN32_CALL(unsigned long) GetFinalPathNameByHandleA(void *handle, char *path, unsigned long path_size, unsigned long flags);
    WIN32_CALL(unsigned long) GetLastError(void);
    WIN32_CALL(void *)        GetProcessHeap(void);
    WIN32_CALL(void *)        GetStdHandle(unsigned long standard_handle);
    WIN32_CALL(void *)        HeapAlloc(void *heap, unsigned long flags, u64 bytes);
    WIN32_CALL(int)           HeapFree(void *heap, unsigned long flags, void *memory);
    WIN32_CALL(void)          OutputDebugStringA(const char *output_string);
    WIN32_CALL(int)           QueryPerformanceCounter(LARGE_INTEGER *performance_count);
    WIN32_CALL(int)           QueryPerformanceFrequency(LARGE_INTEGER *frequency);
    struct _OVERLAPPED;
    WIN32_CALL(int)           ReadFile(void *file, void *buffer, unsigned long number_of_bytes_to_read, unsigned long *number_of_bytes_read, struct _OVERLAPPED *);
    WIN32_CALL(unsigned long) WaitForSingleObject(void *handle, unsigned long milliseconds);
    WIN32_CALL(int)           WriteFile(void *file, const void *buffer, unsigned long number_of_bytes_to_write, unsigned long *number_of_bytes_written, struct _OVERLAPPED *);

    meta(link "Shell32.lib")
    WIN32_CALL(unsigned short **) CommandLineToArgvW(const unsigned short *command_line, int *number_of_arguments);
    WIN32_CALL(void *) ShellExecuteA(void *handle, const char *operation, const char *file, const char *parameters, const char *directory, int show_command_count);
#elif BASE_OS == BASE_OS_WASM
    typedef u64 size_t;
    #define os_abort() // TODO(felix)
    #define os_exit(code) // TODO(felix)
#endif // BASE_OS

#if COMPILER_CLANG || COMPILER_GCC // they share many builtins
    #define builtin_unreachable __builtin_unreachable()
    #define force_inline inline __attribute__((always_inline))
    #if BASE_OS == BASE_OS_WINDOWS
        // TODO(felix): this is only for memcmp, etc. fix!
        #include <string.h>
    #endif
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
    #define force_inline inline __forceinline

#elif COMPILER_STANDARD
    // NOTE(felix): I don't even know why I have this. Getting my base layer working with a non-major compiler is not a priority
    #include <assert.h>
    #define builtin_assume(expr) assert(expr)
    #define breakpoint builtin_assume(false)
    #define builtin_unreachable assert(false)
    #define force_inline inline

#endif // COMPILER

#if defined(unreachable)
    #undef unreachable
#endif

#if BUILD_DEBUG
    #define assert(expr) do { if(!(expr)) panic("failed assertion `"#expr"`"); } while (0)
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

#define TIME_FUNCTION static
#define TIME_NO_SYSTEM_INCLUDE
#include "base/time.h"

#define PRNG_FUNCTION static
#include "base/prng.h"

typedef struct Arena Arena;
#define Array(T) struct { T *data; u64 count, capacity; }
typedef Array(void) Array_void;

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

#define panic(...) panic_(__FILE__, __LINE__, __func__, __VA_ARGS__)
#define panic_(file, line, function, ...) do { \
    log_internal_with_location((file), (line), (function), "panic: " __VA_ARGS__); \
    breakpoint; os_abort(); \
} while (0)

static void reserve_exactly_(Arena *arena ,Array_void *a, u64 item_size, u64 new_capacity, bool zero);
static u64 grow_capacity(u64 current, u64 minimum);
#define reserve(arena, a, c) reserve_exactly((arena), (a), grow_capacity((a)->capacity, (c)))
#define reserve_exactly(arena, a, c) do { \
    _Static_assert(sizeof *(a) == sizeof(Array_void), "parameter must point to an Array(T)"); \
    reserve_exactly_((arena), (Array_void *)(a), sizeof *((a)->data), (c), true); \
} while (0)
#define reserve_unused(arena, a, c) reserve((arena), (a), (a)->count + (c))
#define reserve_unused_exactly(arena, a, c) reserve_exactly((arena), (a), (a)->count + (c))
#define push(arena, a, o) do { \
    reserve_unused((arena), (a), 1); \
    (a)->data[(a)->count++] = (o); \
} while (0)
#define push_assume_capacity(a, o) do { \
    _Static_assert(sizeof *(a) == sizeof(Array_void), "parameter must point to an Array(T)"); \
    assert((a)->count + 1 <= (a)->capacity); \
    (a)->data[(a)->count++] = (o); \
} while (0)
#define push_many(arena, a, items, num) do { \
    reserve_unused((arena), (a), (num)); \
    for (u64 i__pm__ = 0; i__pm__ < (num); i__pm__ += 1) { \
        (a)->data[(a)->count++] = (items)[i__pm__]; \
    } \
} while (0)
#define push_many_assume_capacity(a, items, num) do { \
    _Static_assert(sizeof *(a) == sizeof(Array_void), "parameter must point to an Array(T)"); \
    assert((a)->count + (num) <= (a)->capacity); \
    for (u64 i__pmac__ = 0; i__pmac__ < (num); i__pmac__ += 1) { \
        (a)->data[(a)->count++] = (items)[i__pmac__]; \
    } \
} while (0)
#define push_slice(a, b) push_many((a), (b).data, (b).count)
#define push_slice_assume_capacity(a, b) push_many_assume_capacity((a), (b).data, (b).count)
static void swap_remove_(Array_void *array, u64 index, u64 item_size);
#define swap_remove(a, i) do { \
    _Static_assert(sizeof *(a) == sizeof(Array_void), "parameter must point to an Array(T)"); \
    swap_remove_((Array_void *)(a), (i), sizeof *(a)->data); \
} while (0)

// NOTE(felix): these are function-like macros and so I would've liked them to be lowercase, but windows.h defines lowercase min,max and it's too much of a hassle to work out, especially with more than one translation unit. This is the simpler solution by far.
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP_LOW MAX
#define CLAMP_HIGH MIN
#define CLAMP(value, low, high) CLAMP_HIGH(CLAMP_LOW((value), (low)), (high))

static force_inline int memcmp_(const void *a_, const void *b_, u64 byte_count) {
    const u8 *a = a_, *b = b_;
    for (u64 i = 0; i < byte_count; i += 1) {
        if (a[i] != b[i]) return a[i] - b[i];
    }
    return 0;
}

static force_inline void *memcpy_(void *destination_, const void *source_, u64 byte_count) {
    u8 *destination = destination_;
    const u8 *source = source_;
    if (byte_count != 0) {
        assert(destination != 0);
        assert(source != 0);
    }
    for (u64 i = 0; i < byte_count; i += 1) destination[i] = source[i];
    return destination;
}

static force_inline void *memset_(void *destination_, int byte_, u64 byte_count) {
    assert(byte_ < 256);
    u8 byte = (u8)byte_;
    u8 *destination = destination_;
    assert(destination != 0);
    for (u64 i = 0; i < byte_count; i += 1) destination[i] = byte;
    return destination;
}

#if !LINK_CRT && BASE_OS != BASE_OS_WASM
    #pragma function(memcmp)
    #pragma function(memcpy)
    #pragma function(memset)

    int memcmp(const void *a, const void *b, u64 byte_count) {
        return memcmp_(a, b, byte_count);
    }

    void *memcpy(void *destination, const void *source, u64 byte_count) {
        return memcpy_(destination, source, byte_count);
    }

    void *memset(void *destination, int byte, u64 byte_count) {
        return memset_(destination, byte, byte_count);
    }
#endif

static force_inline f32 ceilf_(f32 x) {
    assert((f32)INT64_MIN <= x && x <= (f32)INT64_MAX);
    i64 as_i64 = (i64)x;
    f32 result = (f32)((f32)as_i64 + (f32)(x > (f32)as_i64));
    return result;
}

static force_inline f32 floorf_(f32 x) {
    assert((f32)INT64_MIN <= x && x <= (f32)INT64_MAX);
    i64 as_i64 = (i64)x;
    f32 result = (f32)((f32)as_i64 - (f32)(x < (f32)as_i64));
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

#if !LINK_CRT
    #if COMPILER_CLANG
        #if BASE_OS == BASE_OS_WASM // is this the right solution?
            // #define memcmp(a, b, c) __builtin_memcmp((a), (b), (unsigned long)(c))
            #define memcmp(a, b, c) memcmp_wasm((a), (b), (unsigned long)(c))
            static int memcmp_wasm(const void *a_, const void *b_, size_t c) {
                const unsigned char *a = a_, *b = b_;
                for (size_t i = 0; i < c; i += 1) {
                    if (a[i] != b[i]) return a[i] - b[i];
                }
                return 0;
            }
            #define memcpy(d, s, c) __builtin_memcpy((d), (s), (unsigned long)(c))
            #define memset(m, b, c) __builtin_memset((m), (b), (unsigned long)(c))
        #else
            #define memcmp __builtin_memcmp
            #define memcpy __builtin_memcpy
        #endif
    #endif
#endif

struct Arena {
    void *mem;
    u64 offset;
    u64 capacity;
    u64 last_offset;
};

static void  arena_deinit(Arena *arena);
static Arena arena_from_memory(u8 *bytes, u64 count);
static Arena arena_init(u64 initial_size_bytes); // TODO(felix): remove this function. A zeroed reserve+commit arena will be valid and will grow on demand
#define      arena_make(a, i, T) (T *)arena_make_((a), (i), sizeof(T), true)
static void *arena_make_(Arena *arena, u64 item_count, u64 item_size, bool zero);
static void *arena_push(Arena *arena, const void *data, u64 count);

#if BUILD_ASAN
    #define asan_poison_memory_region(address, byte_count)   __asan_poison_memory_region(address, byte_count)
    #define asan_unpoison_memory_region(address, byte_count) __asan_unpoison_memory_region(address, byte_count)
    void __asan_poison_memory_region(const void *address, u64 size);
    void __asan_unpoison_memory_region(const void *address, u64 size);
#else
    #define asan_poison_memory_region(address, byte_count)   do { (void)(address); (void)(byte_count); } while (0)
    #define asan_unpoison_memory_region(address, byte_count) do { (void)(address); (void)(byte_count); } while (0)
#endif // BUILD_ASAN

static const char **os_get_arguments(Arena *, u64 *out_count);

static u64 hash_djb2(const void *bytes, u64 byte_count);
static u64 hash_lookup_msi(u64 hash, u64 exponent, u64 index);

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
    i32 exponent;
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
        #define cosf  __builtin_cosf
        #define expf  __builtin_expf
        #define fmodf __builtin_fmodf
        #define logf  __builtin_logf
        #define tanhf __builtin_tanhf
    #endif
#endif

#if !LINK_CRT
    #if !COMPILER_CLANG
        #define frexpf frexpf_
        #define sinf   sinf_
        #define tanf   tanf_
    #endif
    #define ceilf  ceilf_
    #define expf   expf_
    #define floorf floorf_
    #define fmodf  fmodf_
    #define cosf   cosf_
    #define logf   logf_
    #define sqrtf  sqrtf_
    #define tanhf  tanhf_
#endif

// TODO(felix): clean these up

#if COMPILER_MSVC
    #pragma intrinsic(abs, _abs64, fabs)
    #define abs_i32(x) abs(x)
    #define abs_i64(x) _abs64(x)
    #define abs_f64(x) fabs(x)
    // NOTE(felix): because I got `warning C4163: 'fabsf': not available as an intrinsic function`
    // NOTE(felix): corecrt_math.h defines fabsf as an inline function casting fabs to f32. Might be good to benchmark that approach versus what I'm doing here
    static force_inline f32 fabsf(f32 x) { return x < 0 ? -x : x; }
#elif COMPILER_CLANG || COMPILER_GCC
    #define INFINITY __builtin_inff()

    #define abs_i32(x) __builtin_abs(x)
    #define abs_i64(x) __builtin_llabs(x)
    #define abs_f64(x) __builtin_fabs(x)
    #if LINK_CRT
        #define ceilf  __builtin_ceilf
        #define floorf __builtin_floorf
        #define sqrtf  __builtin_sqrtf
    #else
        #define ceilf  ceilf_
        #define floorf floorf_
        #define sqrtf  sqrtf_
    #endif
    #define fabsf(x)   __builtin_fabsf(x)
    #define floor(x)   __builtin_floor(x)
    #define isnan(x)   __builtin_isnan(x)
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
        #define powf(x, e)   __builtin_powf((x), (e))
        #define roundf(x)    __builtin_roundf(x)
    #endif
#endif

static force_inline f32 roundf(f32 x) {
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

static force_inline f32 radians_from_degrees(f32 degrees);

static  V4 rgba_float_from_hex(u32 hex);
static u32 rgba_hex_from_float(V4 rgba);

static force_inline bool f32_difference_within_margin(f32 a, f32 b, f32 margin);
static       inline f32  f32_stable_lerp(f32 a, f32 b, f32 k, f32 delta_time_seconds);
static force_inline f32  f32_lerp(f32 a, f32 b, f32 amount);

static force_inline V2   v2(f32 value);
static force_inline V2   v2_add(V2 a, V2 b);
static force_inline V2   v2_div(V2 a, V2 b);
static       inline f32  v2_dot(V2 a, V2 b);
static force_inline bool v2_difference_within_margin(V2 a, V2 b, f32 margin);
static       inline f32  v2_len(V2 v);
static       inline f32  v2_len_squared(V2 v);
static       inline V2   v2_lerp(V2 a, V2 b, f32 amount);
static force_inline V2   v2_max(V2 a, V2 b);
static force_inline V2   v2_min(V2 a, V2 b);
static force_inline V2   v2_mul(V2 a, V2 b);
static       inline V2   v2_norm(V2 v);
static force_inline V2   v2_reciprocal(V2 v);
static       inline V2   v2_rotate(V2 v, f32 angle_radians);
static       inline V2   v2_round(V2 v);
static       inline V2   v2_round_down(V2 v);
static force_inline V2   v2_scale(V2 v, f32 s);
static       inline V2   v2_stable_lerp(V2 a, V2 b, f32 k, f32 delta_time_seconds);
static force_inline V2   v2_sub(V2 a, V2 b);

static force_inline V3   v3_add(V3 a, V3 b);
static       inline V3   v3_cross(V3 a, V3 b);
static       inline f32  v3_dot(V3 a, V3 b);
static force_inline bool v3_difference_within_margin(V3 a, V3 b, f32 margin);
static force_inline V3   v3_forward_from_view(M4 view);
static       inline f32  v3_len(V3 v);
static       inline f32  v3_len_squared(V3 v);
static       inline V3   v3_lerp(V3 a, V3 b, f32 amount);
static force_inline V3   v3_neg(V3 v);
static       inline V3   v3_norm(V3 v);
static force_inline V3   v3_right_from_view(M4 view);
static force_inline V3   v3_scale(V3 v, f32 s);
static force_inline V3   v3_sub(V3 a, V3 b);
static       inline V3   v3_unproject(V3 pos, M4 view_projection);
static force_inline V3   v3_up_from_view(M4 view);

static force_inline V4   v4_add(V4 a, V4 b);
static       inline f32  v4_dot(V4 a, V4 b);
static force_inline bool v4_difference_within_margin(V4 a, V4 b, f32 margin);
static       inline V4   v4_lerp(V4 a, V4 b, f32 amount);
static       inline V4   v4_round(V4 v);
static force_inline V4   v4_scale(V4 v, f32 s);
static       inline V4   v4_stable_lerp(V4 a, V4 b, f32 k, f32 delta_time_seconds);
static force_inline V4   v4_sub(V4 a, V4 b);
static force_inline V4   v4v(V3 xyz, f32 w);
static force_inline V3   v4_xyz(V4 v);

static inline Quat quat_from_rotation(V3 axis, f32 angle);
static inline Quat quat_mul_quat(Quat a, Quat b);
static inline V3   quat_rotate_v3(Quat q, V3 v);

static force_inline M3 m3_fill_diagonal(f32 value);
static       inline M3 m3_from_rotation(f32 radians, V2 pivot);
static       inline M3 m3_inverse(M3 m);
static       inline M3 m3_model(V2 scale, f32 radians, V2 pivot, V2 post_translation);
static       inline V3 m3_mul_v3(M3 m, V3 v);
static       inline M3 m3_transpose(M3 m);

static force_inline M4 m4_fill_diagonal(f32 value);
static       inline M4 m4_from_rotation(V3 axis, f32 angle);
static       inline M4 m4_from_top_left_m3(M3 m);
static       inline M4 m4_from_translation(V3 translation);
static       inline M4 m4_inverse(M4 m);
static       inline M4 m4_look_at(V3 eye, V3 centre, V3 up_direction);
static       inline M4 m4_mul_m4(M4 a, M4 b);
static       inline V4 m4_mul_v4(M4 m, V4 v);
static       inline M4 m4_perspective_projection(f32 fov_vertical_radians, f32 width_over_height, f32 range_near, f32 range_far);
static force_inline M4 m4_transpose(M4 m);

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

typedef Array(char) String_Builder;
static   void string_builder_null_terminate(Arena *arena, String_Builder *builder);
static   void string_builder_print(Arena *arena, String_Builder *builder, const char* format, ...);
static   void string_builder_print_(Arena *arena, String_Builder *builder, const char *fmt, va_list arguments);
static String string_builder_string(String_Builder builder);

#define FS_FUNCTION static
#define FS_NO_SYSTEM_INCLUDE
#include "base/filesystem.h"
static String os_read_entire_file(Arena *arena, const char *path);
static   bool os_write_entire_file(const char *path, const void *data, u64 count);

typedef enum {
    Os_Process_Flag_PRINT_COMMAND_BEFORE_RUNNING = 1 << 0,
    Os_Process_Flag_PRINT_EXECUTION_TIME = 1 << 1,
    Os_Process_Flag_PRINT_EXIT_CODE = 1 << 2,
} Os_Process_Flags;

static void *os_heap_allocate(u64 byte_count);
static void  os_heap_free(void *pointer);
static  u32  os_process_run(Arena scratch, const char **arguments, const char *directory, Os_Process_Flags);
static void  os_write_console(const char *bytes, u64 count);

static void print(const char *format, ...);
static void print_(const char *format, va_list arguments);

#define CLEX_FUNCTION static
#include "base/clex.h"
#define BASE_CLEX_DEFINE_EXPONENT 10
#define BASE_CLEX_DEFINE_CAPACITY (1 << BASE_CLEX_DEFINE_EXPONENT)
typedef struct {
    const char *names[BASE_CLEX_DEFINE_CAPACITY];
    const char *definitions[BASE_CLEX_DEFINE_CAPACITY];
    u32 used;
} Base_Clex_Define;
static const char *base_clex_define(void *user_data, const char *symbol, const char *definition);
static bool base_clex_symbols_match(const char *a, const char *b);
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

static u32 build_default_everything(Arena scratch, const char *program_name, u8 target_os);

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
    App_Mouse_LEFT,
    App_Mouse_MIDDLE,
    App_Mouse_RIGHT,
    App_Mouse_MAX_VALUE,
} App_Mouse_Button;

typedef struct {
    V2 window_size;
    f32 dpi_scale;
    f32 seconds_since_last_frame;
    V2 mouse_position;
    f32 scroll;
    bool mouse_clicked[App_Mouse_MAX_VALUE];
    bool mouse_down[App_Mouse_MAX_VALUE];
    bool key_down[App_Key_MAX_VALUE];
    bool key_pressed[App_Key_MAX_VALUE];
} App_Frame_Info;

static void app_frame_info_reset_end_of_frame(App_Frame_Info *);

typedef enum {
    Draw_Kind_NIL = 0,
    Draw_Kind_TEXT,
    Draw_Kind_RECTANGLE,
    Draw_Kind_QUADRILATERAL,
    Draw_Kind_LINE,
    Draw_Kind_SPRITE_1BIT,

    Draw_Kind_COUNT,
} Draw_Kind;

typedef enum {
    Draw_Color_SOLID = 0,

    Draw_Color_TOP_LEFT = Draw_Color_SOLID,
    Draw_Color_BOTTOM_LEFT,
    Draw_Color_BOTTOM_RIGHT,
    Draw_Color_TOP_RIGHT,

    Draw_Color_MAX_VALUE,
} Draw_Color;

typedef enum {
    Draw_Corner_TOP_LEFT,
    Draw_Corner_BOTTOM_LEFT,
    Draw_Corner_BOTTOM_RIGHT,
    Draw_Corner_TOP_RIGHT,

    Draw_Corner_COUNT,
} Draw_Corner;

#define BYTE_FROM_BITS(_0, _1, _2, _3, _4, _5, _6, _7) \
    ( ((_0) << 7) | ((_1) << 6) | ((_2) << 5) | ((_3) << 4) | \
      ((_4) << 3) | ((_5) << 2) | ((_6) << 1) | ((_7) << 0) )

typedef struct {
    Draw_Kind kind;
    V2 position;
    V4 clip;
    u32 color[Draw_Color_MAX_VALUE];
    bool gradient;
    union {
        struct {
            String string;
            f32 font_size;
        } text;
        struct {
            u32 border_color;
            f32 border_width;
            f32 border_radius;
            V2 size;
        } rectangle;
        struct {
            V2 corners[Draw_Corner_COUNT];
            u32 border_color;
            f32 border_width;
        } quadrilateral;
        struct {
            V2 end;
            f32 thickness;
        } line;
        struct {
            u64 width, height;
            u8 bytes[32];
        } sprite;
    };
    u64 user_data;
} Draw_Command;
typedef Array(Draw_Command) Array_Draw_Command;

typedef struct {
    App_Frame_Info frame;
    Arena *persistent_arena;
    Arena *frame_arena;
    Array_Draw_Command draw_commands;

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

    static void app_quit(Platform *);
    static void app_start(Platform *);
    static void app_update_and_render(Platform *);
#endif

static void draw(Platform *platform, Draw_Command command);
static V2   draw_command_bounds(Draw_Command command);
static void draw_many(Platform *platform, Draw_Command *commands, u64 count);

#define UI_FUNCTION static
#include "base/ui.h"

static ui_Box *base_ui_draw_command(Arena *frame_arena, ui_State *ui, ui_Box *parent, Draw_Command command, ui_Flags flags, const ui_Style *style, const char *format, ...);
static void    base_ui_input(ui_State *ui, App_Frame_Info *frame);
static void    base_ui_measure_text(void *user_data, const char *string, ui_Style style, f32 size[2]);
static void    base_ui_render(ui_State *ui, ui_Box *box);


#endif // !defined(BASE_H)


#if defined(BASE_IMPLEMENTATION)


static force_inline bool assert_expression(bool value) {
    assert(value);
    return true;
}

// TODO(felix): replace with metaprogram lib specifier
#if BASE_OS == BASE_OS_WINDOWS
    #pragma comment(lib, "kernel32.lib")
    #pragma comment(lib, "shell32.lib") // needed by os_get_arguments
#endif

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
raddbg_type_view(Array_?, $.slice())
raddbg_type_view(String, view:text($.data, size=count))

static void program(void);

#if LINK_CRT || (BASE_OS & BASE_OS_ANY_POSIX)
    static int argument_count_;
    static char **arguments_;

    int main(int argument_count, char **arguments) {
        argument_count_ = argument_count;
        arguments_ = arguments;
        program();
        os_exit(0);
    }
#elif BASE_OS == BASE_OS_WINDOWS || BASE_OS == BASE_OS_WASM
    void entrypoint(void) {
        program();
        os_exit(0);
    }
#endif

static const char **os_get_arguments(Arena *arena, u64 *out_count) {
    const char **arguments = 0;

    #if BASE_OS == BASE_OS_WINDOWS
    {
        int argument_count = 0;
        u16 **arguments_utf16 = CommandLineToArgvW(GetCommandLineW(), &argument_count);
        if (arguments_utf16 == 0) {
            log_error("unable to get command line arguments");
            *out_count = 0;
            return 0;
        }

        arguments = arena_make(arena, (u64)argument_count + 1, const char *);

        for (u64 i = 0; i < (u64)argument_count; i += 1) {
            u16 *argument_utf16 = arguments_utf16[i];

            u64 length = 1;
            while (argument_utf16[length] != 0) length += 1;

            // TODO(felix): convert to UTF-8, not ascii
            char *argument = arena_make(arena, length + 1, char);
            for (u64 j = 0; j < length; j += 1) {
                u16 wide_character = argument_utf16[j];
                assert(wide_character < 128);
                argument[j] = (char)wide_character;
            }

            arguments[i] = argument;
        }
        *out_count = (u64)argument_count;
    }
    #elif BASE_OS & BASE_OS_ANY_POSIX
    {
        (void)arena;
        *out_count = (u64)argument_count_;
        arguments = (const char **)arguments_;
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

static inline bool ascii_is_alpha(u8 c) { return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z'); }

static inline bool ascii_is_decimal(u8 c) { return '0' <= c && c <= '9'; }

static inline bool ascii_is_hexadecimal(u8 c) { return ('0' <= c && c <= '9') || ('A' <= c && c <= 'F') || ('a' <= c && c <= 'f'); }

static inline bool ascii_is_whitespace(u8 c) { return c == ' ' || c == '\n' || c == '\r' || c == '\t'; }

#define POISON_BYTE 0xcd

#define DS_IMPLEMENTATION
#include "base/data_structures.h"

static void reserve_exactly_(Arena *arena, Array_void *a, u64 item_size, u64 new_capacity, bool zero) {
    if (a->capacity >= new_capacity) return;

    u64 byte_count = new_capacity * item_size;
    assert(byte_count >= new_capacity); // overflow

    // TODO(felix): assert the array was allocated on the arena passed in, unless I can think of any situation where it'd be useful for that not to be true
    void *raw = arena_make_(arena, byte_count, 1, false);
    memcpy(raw, a->data, a->count * item_size);

    u8 byte = 0;
    if (BUILD_DEBUG && !zero) byte = POISON_BYTE;
    if (zero || BUILD_DEBUG) {
        u64 growth_bytes = item_size * (new_capacity - a->capacity);
        void *unused_new_memory = (u8 *)raw + item_size * a->capacity;
        memset(unused_new_memory, byte, growth_bytes);
    }

    a->data = raw;
    a->capacity = new_capacity;
}

static void swap_remove_(Array_void *array, u64 index, u64 item_size) {
    assert(index < array->count);

    char *memory = array->data;
    void *destination = &memory[item_size * index];
    void *source = &memory[item_size * (array->count - 1)];
    memcpy(destination, source, item_size);

    if (BUILD_DEBUG) memset(source, POISON_BYTE, item_size);
    array->count -= 1;
}

static u64 grow_capacity(u64 current, u64 minimum) {
    if (current >= minimum) return current;
    if (minimum >= 0x7fffffffffffffff) return minimum;
    current += !current;
    while (current < minimum) current *= 2;
    return current;
}

static Arena arena_from_memory(u8 *bytes, u64 count) {
    Arena arena = { .mem = bytes, .capacity = count };
    return arena;
}

static Arena arena_init(u64 initial_size_bytes) {
    Arena arena = {0};

    // TODO(felix): switch to os_heap_allocate and os_heap_free in these functions as the fallback, since those already do the right os-specific thing

    // TODO(felix): switch to reserve+commit with (virtually) no limit: reserve something like 64gb and commit pages as needed
    #if BASE_OS == BASE_OS_WINDOWS
    {
        enum { heap_zero_memory = 8 }; // https://learn.microsoft.com/en-us/windows/win32/api/heapapi/nf-heapapi-heapalloc
        arena.mem = HeapAlloc(GetProcessHeap(), heap_zero_memory, initial_size_bytes);
    }
    #elif BASE_OS & BASE_OS_ANY_POSIX
    {
        arena.mem = calloc(initial_size_bytes, 1);
    }
    #elif BASE_OS == BASE_OS_WASM
    {
        extern u8 __heap_base;

        static u8 *wasm_heap_pointer = 0;
        if (wasm_heap_pointer == 0) wasm_heap_pointer = &__heap_base;

        arena.mem = wasm_heap_pointer;
        // TODO(felix): implementation using memory.grow?

        // TODO(felix): don't hardcode limit. Note this has to match wasm linker flags!
        ensure((u64)(wasm_heap_pointer - &__heap_base) + initial_size_bytes <= 134217728);
        wasm_heap_pointer += initial_size_bytes;

        int byte = BUILD_DEBUG ? POISON_BYTE : 0;
        memset(arena.mem, byte, initial_size_bytes);
    }
    #else
        #error "unsupported OS"
    #endif

    if (arena.mem == 0) panic("arena_init failure (requested %llu bytes)", initial_size_bytes);
    arena.capacity = initial_size_bytes;
    // TODO(felix): poison with debug byte when asan off
    asan_poison_memory_region(arena.mem, arena.capacity);
    return arena;
}

static void *arena_make_(Arena *arena, u64 item_count, u64 item_size, bool zero) {
    assert(arena != 0);
    assert(item_size > 0);
    u64 byte_count = item_count * item_size;
    if (byte_count == 0) return 0;

    u64 alignment = 2 * sizeof(void *);
    u64 modulo = arena->offset & (alignment - 1);
    u64 pad = alignment - modulo;
    if (byte_count > arena->capacity - arena->offset - pad) panic("allocation failure");

    if (modulo != 0) {
        if (BUILD_DEBUG) {
            asan_unpoison_memory_region((u8 *)arena->mem + arena->offset, pad);
            memset((u8 *)arena->mem + arena->offset, POISON_BYTE, pad);
        }
        asan_poison_memory_region((u8 *)arena->mem + arena->offset, pad);
        arena->offset += pad;
    }

    void *mem = (u8 *)arena->mem + arena->offset;
    arena->last_offset = arena->offset;
    arena->offset += byte_count;

    asan_unpoison_memory_region(mem, byte_count);
    asan_poison_memory_region((u8 *)arena->mem + arena->offset, arena->capacity - arena->offset);

    if (zero) memset(mem, 0, byte_count);
    else if (BUILD_DEBUG) memset(mem, POISON_BYTE, byte_count);

    return mem;
}

static void arena_deinit(Arena *arena) {
    asan_poison_memory_region(arena->mem, arena->capacity);

    #if BASE_OS == BASE_OS_WINDOWS
        HeapFree(GetProcessHeap(), 0, arena->mem);
    #elif BASE_OS & BASE_OS_ANY_POSIX
        free(arena->mem);
    #else
        panic("unimplemented");
    #endif

    *arena = (Arena){0};
}

static void *arena_push(Arena *arena, const void *data, u64 count) {
    u8 *bytes_on_arena = arena_make(arena, count, u8);
    memcpy(bytes_on_arena, data, count);
    return bytes_on_arena;
}

static u64 hash_djb2(const void *bytes, u64 byte_count) {
    u64 hash = 5381;
    for (u64 i = 0; i < byte_count; i += 1) {
        hash += (hash << 5) + ((const u8 *)bytes)[i];
    }
    return hash;
}

static u64 hash_lookup_msi(u64 hash, u64 exponent, u64 index) {
    u64 mask = ((u64)1 << exponent) - 1;
    u64 step = (hash >> (64 - exponent)) | 1;
    u64 new = (index + step) & mask;
    return new;
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

static force_inline f32 radians_from_degrees(f32 degrees) { return degrees * PI_F32 / 180.f; }

static V4 rgba_float_from_hex(u32 hex) {
    f32 pack = 1.f / 255.f;
    V4 result = {
        pack * (hex >> 24),
        pack * ((hex >> 16) & 0xff),
        pack * ((hex >> 8) & 0xff),
        pack * (hex & 0xff),
    };
    return result;
}

static u32 rgba_hex_from_float(V4 rgba) {
    u32 c[4];
    for (u64 i = 0; i < 4; i += 1) c[i] = (u32)(v(rgba)[i] * 255.f + 0.5f);
    u32 result = (c[0] << 24) | (c[1] << 16) | (c[2] << 8) | c[3];
    return result;
}

static force_inline bool f32_difference_within_margin(f32 a, f32 b, f32 margin) {
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

static force_inline f32 f32_lerp(f32 a, f32 b, f32 amount) { return a + amount * (b - a); }

static force_inline V2 v2(f32 value) { return (V2){ .x = value, .y = value }; }

static force_inline V2 v2_add(V2 a, V2 b) { return (V2){ .x = a.x + b.x, .y = a.y + b.y }; }

static force_inline V2 v2_div(V2 a, V2 b) { return (V2){ .x = a.x / b.x, .y = a.y / b.y }; }

static inline f32 v2_dot(V2 a, V2 b) { return a.x * b.x + a.y * b.y; }

static force_inline bool v2_difference_within_margin(V2 a, V2 b, f32 margin) {
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

static force_inline V2 v2_max(V2 a, V2 b) { return (V2){ .x = MAX(a.x, b.x), .y = MAX(a.y, b.y) }; }

static force_inline V2 v2_min(V2 a, V2 b) { return (V2){ .x = MIN(a.x, b.x), .y = MIN(a.y, b.y) }; }

static force_inline V2 v2_mul(V2 a, V2 b) { return (V2){ .x = a.x * b.x, .y = a.y * b.y }; }

static inline V2 v2_norm(V2 v) {
    f32 length = v2_len(v);
    if (length == 0.f) return (V2){0};
    return (V2){ .x = v.x / length, .y = v.y / length };
}

static force_inline V2 v2_reciprocal(V2 v) { return (V2){ .x = 1.f / v.x, .y = 1.f / v.y }; }

static inline V2 v2_rotate(V2 v, f32 angle_radians) {
    f32 sin_angle = sinf(angle_radians);
    f32 cos_angle = cosf(angle_radians);
    V2 result = { .x = v.x * cos_angle - v.y * sin_angle, .y = v.x * sin_angle + v.y * cos_angle };
    return result;
}

static inline V2 v2_round(V2 v) { return (V2){ .x = roundf(v.x), .y = roundf(v.y) }; }

static inline V2 v2_round_down(V2 v) { return v2_round(v2_sub(v, (V2){ .x = 0.5f, .y = 0.5f })); }

static force_inline V2 v2_scale(V2 v, f32 s) { return (V2){ .x = v.x * s, .y = v.y * s }; }

static inline V2 v2_stable_lerp(V2 a, V2 b, f32 k, f32 delta_time_seconds) {
    for (u64 i = 0; i < 2; i += 1) {
        v(a)[i] = f32_stable_lerp(v(a)[i], v(b)[i], k, delta_time_seconds);
    }
    return a;
}

static force_inline V2 v2_sub(V2 a, V2 b) { return (V2){ .x = a.x - b.x, .y = a.y - b.y }; }

static force_inline V3 v3_add(V3 a, V3 b) { return (V3){ .x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z }; }

static inline V3 v3_cross(V3 a, V3 b) {
    return (V3) {
        .x = a.y * b.z - a.z * b.y,
        .y = a.z * b.x - a.x * b.z,
        .z = a.x * b.y - a.y * b.x,
    };
}

static inline f32 v3_dot(V3 a, V3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

static force_inline bool v3_difference_within_margin(V3 a, V3 b, f32 margin) {
    bool result = fabsf(a.x - b.x) <= margin;
    result = result && fabsf(a.y - b.y) <= margin;
    result = result && fabsf(a.z - b.z) <= margin;
    return result;
}

static force_inline V3 v3_forward_from_view(M4 view) {
    return (V3){ .x = -view.c[0][2], .y = -view.c[1][2], .z = -view.c[2][2] };
}

static inline f32 v3_len(V3 v) { return sqrtf(v3_len_squared(v)); }

static inline f32 v3_len_squared(V3 v) { return v3_dot(v, v); }

static inline V3 v3_lerp(V3 a, V3 b, f32 amount) {
    V3 add = v3_scale(v3_sub(b, a), amount);
    return v3_add(a, add);
}

static force_inline V3 v3_neg(V3 v) { return (V3){ .x = -v.x, .y = -v.y, .z = -v.z }; }

static inline V3 v3_norm(V3 v) {
    f32 length = v3_len(v);
    if (length == 0.f) return (V3){0};
    return (V3){
        .x = v.x / length,
        .y = v.y / length,
        .z = v.z / length,
    };
}

static force_inline V3 v3_right_from_view(M4 view) {
    return (V3){ .x = view.c[0][0], .y = view.c[1][0], .z = view.c[2][0] };
}

static force_inline V3 v3_scale(V3 v, f32 s) { return (V3){ .x = v.x * s, .y = v.y * s, .z = v.z * s }; }

static force_inline V3 v3_sub(V3 a, V3 b) { return (V3){ .x = a.x - b.x, .y = a.y - b.y, .z = a.z - b.z }; }

static inline V3 v3_unproject(V3 pos, M4 view_projection) {
    M4 view_projection_inv = m4_inverse(view_projection);
    Quat q = v4v(pos, 1.f);
    Quat q_trans = m4_mul_v4(view_projection_inv, q);

    V3 q_trans_xyz;
    memcpy(&q_trans_xyz, &q_trans, sizeof q_trans_xyz);

    return v3_scale(q_trans_xyz, 1.f / q_trans.w);
}

static force_inline V3 v3_up_from_view(M4 view) {
    return (V3){ .x = view.c[0][1], .y = view.c[1][1], .z = view.c[2][1] };
}

static force_inline V4 v4_add(V4 a, V4 b) { return (V4){ .x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z, .w = a.w + b.w }; }

static inline f32 v4_dot(V4 a, V4 b) {
    f32 dot = 0;
    for (u64 i = 0; i < 4; i += 1) dot += v(a)[i] * v(b)[i];
    return dot;
}

static force_inline bool v4_difference_within_margin(V4 a, V4 b, f32 margin) {
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

static force_inline V4 v4_scale(V4 v, f32 s) { return (V4){ .x = v.x * s, .y = v.y * s, .z = v.z * s, .w = v.w * s }; }

static inline V4 v4_stable_lerp(V4 a, V4 b, f32 k, f32 delta_time_seconds) {
    for (u64 i = 0; i < 4; i += 1) {
        v(a)[i] = f32_stable_lerp(v(a)[i], v(b)[i], k, delta_time_seconds);
    }
    return a;
}

static force_inline V4 v4_sub(V4 a, V4 b) { return (V4){ .x = a.x - b.x, .y = a.y - b.y, .z = a.z - b.z, .w = a.w - b.w }; }

static V4 v4v(V3 xyz, f32 w) { return (V4){ .x = xyz.x, .y = xyz.y, .z = xyz.z, .w = w }; }

static force_inline V3 v4_xyz(V4 v) {
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

    V3 result;
    memcpy(&result, &result_as_quat, sizeof result);
    return result;
}

static force_inline M3 m3_fill_diagonal(f32 value) {
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

static force_inline M4 m4_fill_diagonal(f32 value) {
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

static force_inline M4 m4_transpose(M4 m) {
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

static void cpu_draw_commands(cpu_draw_Surface surface, const cpu_draw_Font *font, Draw_Command *commands, u64 count) {
    u32 (*color)(u32) = 0;
    switch (BASE_OS) {
        case BASE_OS_WINDOWS: color = argb_from_rgba; break;
        case BASE_OS_WASM: color = abgr_from_rgba; break;
        default: unreachable;
    }

    cpu_draw_Surface unclipped_surface = surface;

    for (u64 i = 0; i < count; i += 1) {
        Draw_Command *c = &commands[i];

        surface = unclipped_surface;
        bool has_clip = !!memcmp(&c->clip, &(V4){0}, sizeof c->clip);
        if (has_clip) {
            int x = (int)roundf(c->clip.x);
            int y = (int)roundf(c->clip.y);
            int width = (int)roundf(c->clip.z);
            int height = (int)roundf(c->clip.w);
            surface = cpu_draw_subsurface(unclipped_surface, x, y, width, height);
        }

        u32 argb = color(c->color[Draw_Color_SOLID]);

        switch (c->kind) {
            case Draw_Kind_TEXT: {
                String s = c->text.string;
                int left = (int)(c->position.x + 0.5f);
                int top = (int)(c->position.y + 0.5f);
                cpu_draw_text(surface, font, left, top, argb, s.data, (int)s.count, 0);
            } break;
            case Draw_Kind_RECTANGLE: {
                i32 left = (i32)c->position.x;
                i32 top = (i32)c->position.y;
                i32 width = (i32)c->rectangle.size.x;
                i32 height = (i32)c->rectangle.size.y;
                i32 bottom = top + height;
                i32 right = left + width;

                f32 border_radius_f32 = roundf(c->rectangle.border_radius);
                i32 border_width = (i32)roundf(c->rectangle.border_width);
                if (border_width >= 1 || border_radius_f32 >= 1.f) {
                    i32 border_radius = (i32)border_radius_f32;

                    u32 border_color = color(c->rectangle.border_color);

                    cpu_draw_rounded_rectangle(surface, left, top, right, bottom, argb, border_color, border_radius, border_width);
                } else {
                    cpu_draw_rectangle(surface, left, top, right, bottom, argb);
                }
            } break;
            case Draw_Kind_QUADRILATERAL: {
                int coordinates[2 * Draw_Corner_COUNT] = {0};
                for (Draw_Corner corner = 0; corner < Draw_Corner_COUNT; corner += 1) {
                    V2 position = c->quadrilateral.corners[corner];
                    for (unsigned axis = 0; axis < 2; axis += 1) {
                        coordinates[(unsigned)corner * 2 + axis] = (int)(v(position)[axis] + 0.5f);
                    }
                }

                int *tl = &coordinates[Draw_Corner_TOP_LEFT * 2];
                int *bl = &coordinates[Draw_Corner_BOTTOM_LEFT * 2];
                int *br = &coordinates[Draw_Corner_BOTTOM_RIGHT * 2];
                int *tr = &coordinates[Draw_Corner_TOP_RIGHT * 2];

                bool gradient = false;
                u32 tl_color = argb;
                u32 bl_color = argb;
                u32 br_color = argb;
                u32 tr_color = argb;
                if (c->gradient) {
                    bl_color = color(c->color[Draw_Color_BOTTOM_LEFT]);
                    br_color = color(c->color[Draw_Color_BOTTOM_RIGHT]);
                    tr_color = color(c->color[Draw_Color_TOP_RIGHT]);

                    gradient = tl_color != bl_color || bl_color != br_color || br_color != tr_color || tr_color != tl_color;
                }

                bool is_rectangle = tl[0] == bl[0] && tr[0] == br[0] && tl[1] == tr[1] && bl[1] == br[1];
                if (gradient) {
                    cpu_draw_triangle_interpolate(surface, tl[0], tl[1], bl[0], bl[1], br[0], br[1], tl_color, bl_color, br_color);
                    cpu_draw_triangle_interpolate(surface, tl[0], tl[1], br[0], br[1], tr[0], tr[1], tl_color, br_color, tr_color);
                } else {
                    if (is_rectangle) {
                        cpu_draw_rectangle(surface, tl[0], tl[1], br[0], br[1], argb);
                    } else {
                        cpu_draw_triangle(surface, tl[0], tl[1], bl[0], bl[1], br[0], br[1], argb);
                        cpu_draw_triangle(surface, tl[0], tl[1], br[0], br[1], tr[0], tr[1], argb);
                    }
                }

                if (c->quadrilateral.border_width != 0.f) {
                    u32 border_color = color(c->quadrilateral.border_color);
                    if (is_rectangle) {
                        i32 thickness = (i32)roundf(c->quadrilateral.border_width);
                        cpu_draw_rectangle(surface, tl[0], tl[1], tr[0], tr[1] + thickness, border_color);
                        cpu_draw_rectangle(surface, bl[0], bl[1] - thickness, br[0], br[1], border_color);
                        cpu_draw_rectangle(surface, tl[0], tl[1], bl[0] + thickness, bl[1], border_color);
                        cpu_draw_rectangle(surface, tr[0] - thickness, tr[1], br[0], br[1], border_color);
                    } else {
                        // TODO(felix): don't assume border thickness rounds to 1
                        cpu_draw_line(surface, tl[0], tl[1], tr[0], tr[1], border_color);
                        cpu_draw_line(surface, tr[0], tr[1], br[0], br[1], border_color);
                        cpu_draw_line(surface, br[0], br[1], bl[0], bl[1], border_color);
                        cpu_draw_line(surface, bl[0], bl[1], tl[0], tl[1], border_color);
                    }
                }
            } break;
            case Draw_Kind_LINE: {
                assert(c->line.thickness == 1.f); // TODO(felix)
                int start_x = (int)(c->position.x + 0.5f);
                int start_y = (int)(c->position.y + 0.5f);
                int end_x = (int)(c->line.end.x + 0.5f);
                int end_y = (int)(c->line.end.y + 0.5f);
                cpu_draw_line(surface, start_x, start_y, end_x, end_y, argb);
            } break;
            case Draw_Kind_SPRITE_1BIT: {
                assert(c->sprite.width / 8 <= CPU_DRAW_SPRITE_MAX_WIDTH_OVER_8);
                assert(c->sprite.height <= CPU_DRAW_SPRITE_MAX_HEIGHT);

                cpu_draw_Sprite sprite = {
                    .width = (i8)c->sprite.width,
                    .height = (i8)c->sprite.height,
                };
                memcpy(sprite.bytes, c->sprite.bytes, MIN(sizeof sprite.bytes, sizeof c->sprite.bytes));

                int x = (int)roundf(c->position.x);
                int y = (int)roundf(c->position.y);
                cpu_draw_sprite_1bit(surface, x, y, sprite, color(c->color[Draw_Color_SOLID]));
            } break;
            default: unreachable;
        }
    }
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
    char *cstring = arena_make(arena, string.count + 1, char);
    for (u64 i = 0; i < string.count; i += 1) cstring[i] = (char)string.data[i];
    cstring[string.count] = '\0';
    return cstring;
}

static const char *cstring_print(Arena *arena, const char *format, ...) {
    String_Builder builder = {0};
    reserve(arena, &builder, cstring_length(format));

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

    char *string = arena_make(arena, count, char);

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

static void string_builder_print_(Arena *arena, String_Builder *builder, const char *fmt_c, va_list arguments) {
    u64 size_guess = cstring_length(fmt_c) * 8;
    reserve_unused(arena, builder, size_guess);

    char *buffer = &builder->data[builder->count];
    u64 buffer_size = builder->capacity - builder->count;
    if (BUILD_DEBUG) memset(buffer, POISON_BYTE, buffer_size);

    va_list arguments_1;
    va_copy(arguments_1, arguments);
    int bytes_printed = small_vsnprintf(buffer, buffer_size, fmt_c, arguments_1);
    va_end(arguments_1);

    if ((u64)bytes_printed >= buffer_size) {
        u64 needed_unused = (u64)bytes_printed + 1; // for null terminator
        reserve_unused(arena, builder, needed_unused);

        buffer = &builder->data[builder->count];
        buffer_size = builder->capacity - builder->count;

        assert(buffer_size >= needed_unused);
        if (BUILD_DEBUG) memset(buffer, POISON_BYTE, buffer_size);

        va_list arguments_2;
        va_copy(arguments_2, arguments);
        bytes_printed = small_vsnprintf(buffer, buffer_size, fmt_c, arguments_2);
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
    reserve_unused_exactly(arena, builder, 1);
    builder->data[builder->count] = 0;
}

static void *os_heap_allocate(u64 byte_count) {
    void *pointer = 0;

    #if BASE_OS == BASE_OS_WINDOWS
    {
        enum { heap_zero_memory = 8 }; // https://learn.microsoft.com/en-us/windows/win32/api/heapapi/nf-heapapi-heapalloc
        pointer = HeapAlloc(GetProcessHeap(), heap_zero_memory, byte_count);
    }
    #elif BASE_OS & BASE_OS_ANY_POSIX
    {
        pointer = malloc(byte_count);
    }
    #elif BASE_OS == BASE_OS_WASM
    {
        (void)byte_count;
        panic("TODO(felix)");
    }
    #else
        #error "unsupported OS"
    #endif

    ensure(pointer != 0);
    return pointer;
}

static void os_heap_free(void *pointer) {
    #if BASE_OS == BASE_OS_WINDOWS
        HeapFree(GetProcessHeap(), 0, pointer);
    #elif BASE_OS & BASE_OS_ANY_POSIX
        free(pointer);
    #elif BASE_OS == BASE_OS_WASM
    {
        (void)pointer;
        panic("TODO(felix)");
    }
    #else
        #error "unsupported OS"
    #endif
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
    static void fs_absolute_path(const char *path, char *buffer, unsigned long long buffer_size) {
        (void)path;
        (void)buffer;
        (void)buffer_size;
        panic("unimplemented for WASM");
    }
#else
    #define FS_IMPLEMENTATION
    #define FS_ASSERT assert
    #include "base/filesystem.h"
#endif

static String os_read_entire_file(Arena *arena, const char *path) {
    String result = {0};
    fs_File file = fs_file_open_and_get_size(path, fs_File_Flag_READ, &result.count);

    char *data = arena_make(arena, result.count + 1, char);
    result.count = fs_read_entire_file(file, data, result.count);
    data[result.count] = 0;
    if (result.count == 0) log_error("unable to read '%s'", path);

    fs_close(file);
    result.data = data;
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
        print("%s:%llu:%s(): logged here\n", file, line, func);
    #else
        (void)(file); (void)(line); (void)(func);
    #endif
}

static u32 os_process_run(Arena scratch, const char **arguments, const char *directory, Os_Process_Flags flags) {
    u32 exit_code = 1;

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
        const char *application_name = 0;

        u64 chars = 0, arg_count = 0;
        for (const char **a = arguments; *a != 0; a += 1, arg_count += 1) chars += cstring_length(*a);
        u64 spaces = arg_count - 1;
        chars += spaces;
        chars += 1; // null terminator

        char *command_line = arena_make(&scratch, chars, char);
        u64 command_length = 0;

        for (const char **a = arguments; *a != 0; a += 1) {
            if (a > arguments) command_line[command_length++] = ' ';
            command_length += cstring_copy(command_line + command_length, *a);
        }

        struct _SECURITY_ATTRIBUTES *process_attributes = 0;
        struct _SECURITY_ATTRIBUTES *thread_attributes = 0;
        int inherit_handles = false;
        u32 creation_flags = 0;
        void *environment = 0;
        STARTUPINFOA startup_info = {0};
        PROCESS_INFORMATION process_info = {0};

        int ok = CreateProcessA(
            application_name,
            command_line,
            process_attributes,
            thread_attributes,
            inherit_handles,
            creation_flags,
            environment,
            directory,
            &startup_info,
            &process_info
        );

        if (!ok) {
            log_error("unable to start process '%s'; CreateProcessA() returned %u", arguments[0], GetLastError());
        } else {
            unsigned long infinite = (unsigned long)-1;
            WaitForSingleObject(process_info.hProcess, infinite);

            unsigned long code = 1;
            GetExitCodeProcess(process_info.hProcess, &code);
            exit_code = (u32)code;

            CloseHandle(process_info.hProcess);
            CloseHandle(process_info.hThread);
        }
    }
    #elif BASE_OS & BASE_OS_ANY_POSIX
    {
        pid_t pid = fork();
        if (pid < 0) {
            log_error("unable to fork() to start process '%s' (errno=%d)", arguments[0], errno);
        } else if (pid == 0) { // child
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
        } else { // parent
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
        (void)scratch;
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

    return exit_code;
}

static void os_write_console(const char *bytes, u64 count) {
    // TODO(felix): stderr support
    // NOTE(felix): can't use assert in this function because panic() will call os_write_console, so we'll end up with a recursively failing assert and stack overflow. Instead, use `if (!condition) { breakpoint; abort(); }`
    #if BASE_OS == BASE_OS_WINDOWS
    {
        #if WINDOWS_SUBSYSTEM_WINDOWS
        {
            (void)bytes;
            (void)count;
        }
        #else
        {
            if (count > UINT32_MAX) { breakpoint; os_abort(); }

            // https://learn.microsoft.com/en-us/windows/console/getstdhandle
            unsigned long std_output_handle = (unsigned long)-11;
            // unsigned long std_input_handle = (unsigned long)-10;
            // unsigned long std_error_handle = (unsigned long)-12;

            void *console_handle = GetStdHandle(std_output_handle);

            void *invalid_handle_value = (void *)(long long)-1;
            if (console_handle == invalid_handle_value) { breakpoint; os_abort(); }

            unsigned long num_chars_written = 0;
            int ok = WriteFile(console_handle, bytes, (u32)count, &num_chars_written, 0);
            if (!ok) { breakpoint; os_abort(); }
            if ((u64)num_chars_written != count) { breakpoint; os_abort(); }
        }
        #endif
    }
    #elif BASE_OS & BASE_OS_ANY_POSIX
    {
        int stdout_handle = 1;
        i64 bytes_written = write(stdout_handle, bytes, count);
        (void)bytes_written;
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
    static u8 buffer[4096];
    Arena arena = arena_from_memory(buffer, sizeof buffer);

    String_Builder output = {0};
    reserve(&arena, &output, cstring_length(format));
    string_builder_print_(&arena, &output, format, arguments);
    string_builder_null_terminate(&arena, &output);

    #if (BASE_OS == BASE_OS_WINDOWS) && BUILD_DEBUG
        OutputDebugStringA(output.data);
    #endif

    os_write_console(output.data, output.count);
}

#define CLEX_IMPLEMENTATION
#define CLEX_ASSERT assert
#include "base/clex.h"

static const char *base_clex_define(void *user_data, const char *symbol, const char *definition) {
    assert(user_data != 0);
    assert(symbol != 0);
    const char *result = 0;

    Base_Clex_Define *state = user_data;
    bool should_get = definition == 0;

    const char *symbol_end = clex_identifier_end(symbol);
    u64 hash = ds_hash_fnv1a(symbol, (size_t)(symbol_end - symbol));
    u32 index = (u32)hash;
    const char *name = 0;
    if (should_get) {
        do {
            index = ds_hash_step_index(hash, BASE_CLEX_DEFINE_EXPONENT, index);
            name = state->names[index];
        } while(name != 0 && !base_clex_symbols_match(name, symbol));

        if (name != 0) result = state->definitions[index];
    } else {
        assert(state->used * 100 < BASE_CLEX_DEFINE_CAPACITY * 70);
        state->used += 1;

        do {
            index = ds_hash_step_index(hash, BASE_CLEX_DEFINE_EXPONENT, index);
            assert(index < BASE_CLEX_DEFINE_CAPACITY);
            name = state->names[index];
        } while (name != 0 && !base_clex_symbols_match(name, symbol));

        state->names[index] = symbol;
        state->definitions[index] = definition;
    }

    return result;
}

static bool base_clex_symbols_match(const char *a, const char *b) {
    const char *a_end = clex_identifier_end(a);
    const char *b_end = clex_identifier_end(b);
    String as = string_from_pointers(a, a_end);
    String bs = string_from_pointers(b, b_end);
    return string_equals(as, bs);
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
        "-opt:ref", // TODO(felix): move to release flags and add no_ variants to debug flags
        "-opt:icf",
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
        "-framework", "CoreFoundation",
        "-framework", "Metal",
        "-framework", "Cocoa",
        "-framework", "Metalkit",
        "-framework", "Quartz",
        "-framework", "AudioToolbox",
        0,
    },
    [Build_Linker_CLANG][BASE_OS_WASM] = {
        "clang",
        "--target=wasm32",
        "-nostdlib",
        "-Wl,--no-entry",
        "-Wl,--allow-undefined", // NOTE(felix): consider --allow-undefined-file for explicit whitelist
        "-Wl,--export=platform__get_draw_surface_pointer",
        "-Wl,--export=platform__get_temp_js_string_buffer",
        "-Wl,--export=platform__on_key_down",
        "-Wl,--export=platform__on_key_up",
        "-Wl,--export=platform__on_mouse_down",
        "-Wl,--export=platform__on_mouse_up",
        "-Wl,--export=platform__on_scroll",
        "-Wl,--export=platform__quit",
        "-Wl,--export=platform__start",
        "-Wl,--export=platform__update_and_render",
        "-Wl,--initial-memory=134217728", // 128mb, but TODO(felix): make configurable
        "-Wl,--stack-first",
        "-Wl,-z,stack-size=1048576", // NOTE(felix): 1mb matches windows. I ran into stack overflow on macOS but not on Windows for some reason--on the same project, in the same browser
        0,
    },
};

static const char *build_compiler_mode_flags[Build_Mode_COUNT][Build_Compiler_COUNT][BASE_OS_COUNT][BUILD_FLAGS_MAX] = {
    [Build_Mode_DEBUG] = {
        [Build_Compiler_MSVC][BASE_OS_WINDOWS] = { "-MTd", "-Z7", 0 },
        [Build_Compiler_CLANG][BASE_OS_WINDOWS] = {
            "-g3",
            "-fsanitize=undefined",
            "-fsanitize-trap",
            "-O0",
            0,
        },
        [Build_Compiler_CLANG][BASE_OS_MACOS] = {
            "-g3",
            "-fsanitize=undefined",
            "-fsanitize-trap",
            "-O0",
            0,
        },
        // NOTE(felix): with clang for wasm, ubsan traps with no stack trace, which at the stage in WASM platform development I'm at while writing this, gets in the way of the stack traces for my `panic()`s in C code.
        // TODO(felix): make this configurable - the sanitisers are helpful too
        [Build_Compiler_CLANG][BASE_OS_WASM] = {
            "-fsanitize=undefined",
            "-fsanitize-trap",
            "-g3",
            "-O0",
            0,
        },
    },
    [Build_Mode_RELEASE] = {
        [Build_Compiler_MSVC][BASE_OS_WINDOWS]  = { "-MT", "-O2", 0 },
        [Build_Compiler_CLANG][BASE_OS_WINDOWS] = { "-O3", 0 },
        [Build_Compiler_CLANG][BASE_OS_MACOS]   = { "-O3", 0 },
        [Build_Compiler_CLANG][BASE_OS_WASM]    = { "-O3", 0 },
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
    [BASE_OS_WINDOWS] = "obj",
    [BASE_OS_MACOS] = "o",
    [BASE_OS_WASM] = "o",
};

static const char *build_binary_extension[BASE_OS_COUNT] = {
    [BASE_OS_WINDOWS] = "exe",
    [BASE_OS_MACOS] = "bin",
    [BASE_OS_WASM] = "wasm",
};

typedef Array(const char *) Array_cstring;
static void build_push_arguments(Arena *arena, Array_cstring *strings, const char **arguments) {
    if (arguments == 0) return;
    for (const char **a = arguments; *a != 0; a += 1) push(arena, strings, *a);
}

static bool build_expect_include(Arena *arena, clex_Lexer *lexer, String *file) {
    clex_lex(lexer);
    String token_data = base_string_from_clex_data(lexer->token.data);
    bool ok = lexer->token.kind == clex_Token_INCLUDE;
    ok = ok && string_equals(token_data, S("include"));
    if (!ok) {
        log_error("expected `#include`");
        return false;
    }

    const char *copy = cstring_from_string(arena, base_string_from_clex_data(lexer->token.data2));
    clex_Lexer include = clex_init(copy, 0, base_clex_define, 0);
    clex_lex(&include);
    if (include.token.kind != clex_Token_STRING) {
        log_error("expected string after `#include`");
        return false;
    }
    *file = base_string_from_clex_data(include.token.data);
    return true;
}

static bool build_eat_token(clex_Lexer *lexer, clex_Token_Kind expected, const char *format, ...) {
    clex_lex(lexer);
    bool ok = lexer->token.kind == expected;

    va_list arguments;
    va_start(arguments, format);
    if (!ok) log_internal_v(format, arguments);
    va_end(arguments);

    return ok;
}

static const char build_wasm_html_file[] = STRINGIFY(
    <!doctype html><meta charset=utf-8>

    <style>
        html, body {
            margin: 0;
            height: 100%;
            overflow: hidden;
            background: #808080;
        }
        canvas {
            width: 100vw;
            height: 100vh;
            display: block;
            image-rendering: pixelated;
            image-rendering: crisp-edges;
        }
    </style>

    <canvas id=wasm_canvas width=0 height=0></canvas>

    <script>
        (async () => {
            const canvas = document.getElementById("wasm_canvas");

            const offscreen = document.createElement("canvas");
            // NOTE(felix): these have to match the max values on the C side!
            offscreen.width = 1920;
            offscreen.height = 1200;
            const offscreen_context = offscreen.getContext("2d");

            const imports = { env: {
                js_file_write: (name_cstring, pointer, count) => {
                    const bytes = new Uint8Array(wasm.instance.exports.memory.buffer, pointer, count).slice();

                    const memory = new Uint8Array(wasm.instance.exports.memory.buffer);
                    let end = name_cstring;
                    while (memory[end] != 0) end += 1;
                    const name = new TextDecoder().decode(memory.subarray(name_cstring, end));

                    console.assert(!name.includes("/") && !name.includes("\\"), "js_file_write: path separators forbidden in name", name);

                    const blob = new Blob([bytes]);
                    const a = document.createElement("a");
                    a.href = URL.createObjectURL(blob);
                    a.download = name;
                    a.click();
                    URL.revokeObjectURL(a.href);
                },
                js_performance_now: () => {
                    return performance.now();
                },
                os_write_console_js: (pointer, count) => {
                    const bytes = new Uint8Array(wasm.instance.exports.memory.buffer, pointer, count);
                    const string = new TextDecoder().decode(bytes);
                    console.log(string);
                },
            } };

            // TODO(felix): make .wasm file name changeable in build
            const wasm = await WebAssembly.instantiateStreaming(fetch("module.wasm"), imports);
            const exports = wasm.instance.exports;

            exports.platform__start();

            let mouse_x = 0, mouse_y = 0;
            canvas.addEventListener("mousemove", e => {
                const rect = canvas.getBoundingClientRect();
                mouse_x = (e.clientX - rect.left) * devicePixelRatio;
                mouse_y = (e.clientY - rect.top) * devicePixelRatio;
            });

            function write_js_string(string) {
                const bytes = new TextEncoder().encode(string);
                const buffer = exports.platform__get_temp_js_string_buffer();
                new Uint8Array(wasm.instance.exports.memory.buffer, buffer, bytes.length).set(bytes);
                return [buffer, bytes.length];
            }
            addEventListener("keydown", e => {
                const [buffer, length] = write_js_string(e.code);
                exports.platform__on_key_down(buffer, length);
            });
            addEventListener("keyup", e => {
                const [buffer, length] = write_js_string(e.code);
                exports.platform__on_key_up(buffer, length);
            });

            canvas.addEventListener("mousedown", e => exports.platform__on_mouse_down(e.button));
            canvas.addEventListener("mouseup", e => exports.platform__on_mouse_up(e.button));
            canvas.addEventListener("wheel", e => exports.platform__on_scroll(e.deltaX, e.deltaY));

            const context = canvas.getContext("2d");

            function frame() {
                const real_width = Math.round(window.innerWidth * devicePixelRatio);
                const real_height = Math.round(window.innerHeight * devicePixelRatio);
                if (canvas.width != real_width || canvas.height != real_height) {
                    canvas.width = real_width;
                    canvas.height = real_height;
                    context.imageSmoothingEnabled = false;
                }

                exports.platform__update_and_render(real_width, real_height, mouse_x, mouse_y);

                // must match cpu_draw_Surface layout
                const struct_pointer = exports.platform__get_draw_surface_pointer();
                const memory = new Uint32Array(wasm.instance.exports.memory.buffer);
                const pixels = memory[struct_pointer / 4 + 0];
                const stride = memory[struct_pointer / 4 + 1];
                const width  = memory[struct_pointer / 4 + 5];
                const height = memory[struct_pointer / 4 + 6];

                const source = new Uint8ClampedArray(wasm.instance.exports.memory.buffer, pixels, stride * height * 4);
                const image = new ImageData(source, stride, height);
                offscreen_context.putImageData(image, 0, 0);
                context.drawImage(offscreen, 0, 0, width, height, 0, 0, real_width, real_height);

                requestAnimationFrame(frame);
            }
            requestAnimationFrame(frame);

            addEventListener("beforeunload", () => { exports.platform__quit(); });
        })();
    </script>
);

static u32 build_default_everything(Arena scratch, const char *program_name, u8 target_os) {
    u32 exit_code = 1;

    u64 argument_count = 0;
    const char **arguments = os_get_arguments(&scratch, &argument_count);

    char path_separator = (BASE_OS & BASE_OS_ANY_POSIX) ? '/' : '\\';

    const char *source_folder = cstring_print(&scratch, "..%csrc%c", path_separator, path_separator);
    const char *dependency_folder = cstring_print(&scratch, "..%cdeps%c", path_separator, path_separator);
    const char *c_file_path = cstring_print(&scratch, "%smain.c", source_folder);

    static const Build_Compiler build_default_compiler[BASE_OS_COUNT] = {
        [BASE_OS_WINDOWS] = Build_Compiler_MSVC,
        [BASE_OS_MACOS] = Build_Compiler_CLANG,
        [BASE_OS_WASM] = Build_Compiler_CLANG,
    };
    Build_Compiler compiler = build_default_compiler[target_os];

    typedef struct {
        const char *code_path, *object_path;
        const char **extra_flags;
        bool no_extra_errors;
    } Build_Object;
    Array(Build_Object) objects = {0};

    const char *object_extension = build_object_extension[target_os];

    const char *build_folder = "build";
    bool build_folder_ok = fs_exists(build_folder);
    if (!build_folder_ok) {
        build_folder_ok = fs_make_folder(build_folder);
        if (!build_folder_ok) {
            log_error("failure creating folder '%s'", build_folder);
            return 1;
        }
    }

    String code = os_read_entire_file(&scratch, &c_file_path[3]);

    String_Builder metaprogram_code = {0};
    String metaprogram_output_file = {0};

    String generate_here_directive = S("generate_here");

    bool meta_link_crt_override = false;
    bool meta_link_crt_override_value = false;

    bool TODO_REMOVE_THIS = false;

    Base_Clex_Define *define_state = arena_make(&scratch, 1, Base_Clex_Define);
    clex_Lexer lexer = clex_init(code.data, 0, base_clex_define, define_state);
    f64 time_at_parse_start = time_relative_timestamp();
    while (clex_lex(&lexer)) {
        String preprocess_data2 = base_string_from_clex_data(lexer.token.data2);
        switch (lexer.token.kind) {
            case clex_Token_INCLUDE: {
                if (TODO_REMOVE_THIS) print("#include %S\n", preprocess_data2);

                String path = preprocess_data2;
                if (path.data[0] == '<') { /* skip system includes for now */ }
                else if (path.data[0] == '"') {
                    if (path.data[path.count - 1] != '"') {
                        log_error("`#include` path has unterminated `\"`");
                        return 1;
                    }
                    if (path.count <= 2) {
                        log_error("cannot `#include` empty path `\"\"`");
                        return 1;
                    }
                    path = string_range(path, 1, path.count - 1);

                    const char *read_path = cstring_print(&scratch, "%s%S", &source_folder[3], path);
                    String file = os_read_entire_file(&scratch, read_path);
                    bool ok = clex_push_file(&lexer, path.data, &path.data[path.count], file.data);
                    assert(ok);
                } else {
                    log_error("expected `<` or `\"` following `#include`; got `%c`", path.data[0]);
                    return 1;
                }
            } break;
            case clex_Token_IF: {
                if (TODO_REMOVE_THIS) print("#if %S\n", preprocess_data2);
                // panic("TODO(felix): #if");
            } break;
            case clex_Token_ELIF: {
                if (TODO_REMOVE_THIS) print("#elif %S\n", preprocess_data2);
                // panic("TODO(felix): #elif");
            } break;
            case clex_Token_ENDIF: {
                if (TODO_REMOVE_THIS) print("#endif\n");
                // panic("TODO(felix): #endif");
            } break;
            case clex_Token_ERROR_DIRECTIVE: {
                if (TODO_REMOVE_THIS) print("#error %S\n", preprocess_data2);
                // panic("TODO(felix): #error");
            } break;
            default: break;
        }

        if (TODO_REMOVE_THIS) { // TODO(felix): remove
            String token_data = base_string_from_clex_data(lexer.token.data);
            print("%S ", token_data);
            char d = token_data.data[0];
            if (d == ';' || d == '{') print("\n");
        }

        // TODO(felix): Replace with real preprocessing logic.

        static bool in_active_preprocessor_block = true;

        bool if_hack = lexer.token.kind == clex_Token_IF;
        if_hack = if_hack || lexer.token.kind == clex_Token_ELSE;
        if_hack = if_hack || lexer.token.kind == clex_Token_ENDIF;
        if_hack = false;

        if (if_hack) {
            static bool aware_of_this_static_if_condition = false;
            String token_data = base_string_from_clex_data(lexer.token.data);

            if (string_equals(token_data, S("if"))) {
                if (aware_of_this_static_if_condition) {
                    log_error("nested #if not yet supported in `#if BASE_OS...` block parsed by metaprogram");
                    return 1;
                }

                const char *error = "expected `#if BASE_OS == BASE_OS_...`";

                Arena definition_scratch = scratch;
                const char *copy = cstring_from_string(&definition_scratch, base_string_from_clex_data(lexer.token.data2));
                clex_Lexer definition = clex_init(copy, 0, base_clex_define, 0);
                while (clex_lex(&definition)) {
                    token_data = base_string_from_clex_data(definition.token.data);
                    aware_of_this_static_if_condition = definition.token.kind == clex_Token_IDENTIFIER && string_equals(token_data, S("BASE_OS"));

                    if (aware_of_this_static_if_condition) {
                        if (!build_eat_token(&definition, clex_Token_EQUALITY_CHECK, "error: %s", error)) return 1;

                        if (!build_eat_token(&definition, clex_Token_IDENTIFIER, "error: %s", error)) return 1;
                        token_data = base_string_from_clex_data(definition.token.data);

                        u8 os_to_check = 0;
                        if (string_equals(token_data, S("BASE_OS_WINDOWS"))) os_to_check = BASE_OS_WINDOWS;
                        else if (string_equals(token_data, S("BASE_OS_MACOS"))) os_to_check = BASE_OS_MACOS;
                        else if (string_equals(token_data, S("BASE_OS_LINUX"))) os_to_check = BASE_OS_LINUX;
                        else if (string_equals(token_data, S("BASE_OS_WASM"))) os_to_check = BASE_OS_WASM;
                        else {
                            log_error("expected `BASE_OS_{WINDOWS or MACOS or LINUX or WASM}`, got `#if BASE_OS == %S`", token_data);
                            return 1;
                        }

                        in_active_preprocessor_block = (target_os == os_to_check);
                    }
                }

                continue;
            }

            if (!aware_of_this_static_if_condition) continue;

            if (string_equals(token_data, S("else"))) {
                in_active_preprocessor_block = !in_active_preprocessor_block;
            }

            if (string_equals(token_data, S("endif"))) {
                in_active_preprocessor_block = true;
                aware_of_this_static_if_condition = false;
            }

            continue;
        }

        if (!in_active_preprocessor_block) continue;

        if (lexer.token.kind != clex_Token_IDENTIFIER) continue;
        String token_data = base_string_from_clex_data(lexer.token.data);

        if (!string_equals(token_data, S("meta"))) continue;

        clex_lex(&lexer);
        if (lexer.token.kind != '(') continue;

        if (!build_eat_token(&lexer, clex_Token_IDENTIFIER, "error: expected directive at start of `meta(...)`")) return 1;
        token_data = base_string_from_clex_data(lexer.token.data);

        // TODO(felix): remove
        if (TODO_REMOVE_THIS) print("=========================================================GOT META |%S|\n", token_data);

        if (string_equals(token_data, generate_here_directive)) {
            if (metaprogram_output_file.count != 0) {
                log_error("directive `meta(%S)` can only be used once", generate_here_directive);
                return 1;
            }

            if (!build_eat_token(&lexer, ')', "error: expected ')' closing meta(...) block")) return 1;

            if (!build_expect_include(&scratch, &lexer, &metaprogram_output_file)) return 1;
        } else if (string_equals(token_data, S("embed"))) {
            const char *usage = "`meta(embed \"path/to/file\")\\ extern const char array_name[];\\ extern const u64 length_name;`";

            if (!build_eat_token(&lexer, clex_Token_STRING, "error: expected path, like this: %s", usage)) return 1;
            String input_file = base_string_from_clex_data(lexer.token.data);

            if (!build_eat_token(&lexer, ')', "error: expected ')' closing meta(...) block")) return 1;

            // TODO(felix): get actual type info once we've got a C parser

            String names[2] = {0};
            for (u64 i = 0; i < 2; i += 1) {
                clex_lex(&lexer);
                String extern_ = base_string_from_clex_data(lexer.token.data);
                if (lexer.token.kind != clex_Token_IDENTIFIER || !string_equals(extern_, S("extern"))) {
                    log_error("expected this: %s", usage);
                    return 1;
                }

                clex_lex(&lexer);
                String const_ = base_string_from_clex_data(lexer.token.data);
                if (lexer.token.kind != clex_Token_IDENTIFIER || !string_equals(const_, S("const"))) {
                    log_error("expected this: %s", usage);
                    return 1;
                }

                clex_lex(&lexer);
                String type_ = base_string_from_clex_data(lexer.token.data);
                String expected_type = i == 0 ? S("char") : S("u64");
                if (lexer.token.kind != clex_Token_IDENTIFIER || !string_equals(type_, expected_type)) {
                    log_error("expected this: %s", usage);
                    return 1;
                }

                if (!build_eat_token(&lexer, clex_Token_IDENTIFIER, "error: expected this: %s", usage)) return 1;
                names[i] = base_string_from_clex_data(lexer.token.data);

                if (i == 0) {
                    if (!build_eat_token(&lexer, '[', "error: expected this: %s", usage)) return 1;
                    if (!build_eat_token(&lexer, ']', "error: expected this: %s", usage)) return 1;
                }

                if (!build_eat_token(&lexer, ';', "error: expected this: %s", usage)) return 1;
            }

            String variable_name = names[0];
            String array_length_name = names[1];

            String input = {0};
            const char *input_path = cstring_from_string(&scratch, input_file);

            bool embed_via_object = target_os == BASE_OS_WINDOWS || target_os == BASE_OS_MACOS;
            if (embed_via_object) {
                static bool embedded_once_already = false;
                if (embedded_once_already && embed_via_object) {
                    log_error("TODO(felix): support embedding multiple files in object files");
                    return 1;
                }
                embedded_once_already = true;

                fs_File input_file_handle = fs_file_open_and_get_size(input_path, fs_File_Flag_READ, &input.count);
                if (input.count == 0) {
                    log_error("unable to open file '%s'", input_path);
                    return 1;
                }

                object_Bundle_Flags flags = object_Bundle_READ;
                if (target_os == BASE_OS_WINDOWS) flags |= object_Bundle_COFF;
                else if (target_os == BASE_OS_MACOS) flags |= object_Bundle_MACHO;
                else unreachable;

                const char *array_name = variable_name.data;
                u32 array_name_length = (u32)variable_name.count;

                const char *length_name = array_length_name.data;
                u32 length_name_length = (u32)array_length_name.count;

                u64 byte_count;
                u8 *to_write = object_bundle(array_name, array_name_length, length_name, length_name_length, 0, (u32)input.count, 0, &byte_count, flags);
                assert(to_write == 0 && byte_count > 0);

                u8 *bytes = arena_make(&scratch, byte_count, u8);
                to_write = object_bundle(array_name, array_name_length, length_name, length_name_length, 0, (u32)input.count, bytes, &byte_count, flags);
                if (to_write == 0) {
                    log_error("failure generating object file to embed '%S'", input_file);
                    return 1;
                }

                u64 read_count = fs_read_entire_file(input_file_handle, (char *)to_write, input.count);
                assert(read_count == input.count);
                fs_close(input_file_handle);

                const char *output_name = cstring_print(&scratch, "meta.generated.%s", object_extension);
                const char *output_in_build_folder = cstring_print(&scratch, "%s/%s", build_folder, output_name);
                bool ok = os_write_entire_file(output_in_build_folder, bytes, byte_count);
                if (!ok) return 1;

                Build_Object object = { .object_path = output_name };
                push(&scratch, &objects, object);
            } else {
                input = os_read_entire_file(&scratch, input_path);
                if (input.count == 0) return 1;

                String qualifiers = S("static const char ");
                String assignment = S("[] = {");
                String closing = S("\n};\n");
                String example_byte_as_hex = S("0x00,");
                u64 chars_per_row_exponent = 5;
                u64 chars_per_row = (u64)1 << chars_per_row_exponent;
                u64 rows = input.count / chars_per_row + 1;

                String indent = S("\n    ");
                u64 bytes_for_indentation = rows * indent.count;
                u64 bytes_total = bytes_for_indentation + rows * (chars_per_row * example_byte_as_hex.count);
                bytes_total += qualifiers.count + variable_name.count + assignment.count + closing.count;

                String length_qualifiers = S("static const u64 ");
                String length_assignment = S(" = ");
                String length_closing = S(";\n");
                u64 max_digits = sizeof(u64) * 8;
                bytes_total += length_qualifiers.count + array_length_name.count + length_assignment.count + max_digits + length_closing.count;

                reserve_unused(&scratch, &metaprogram_code, bytes_total);
                push_slice_assume_capacity(&metaprogram_code, qualifiers);
                push_slice_assume_capacity(&metaprogram_code, variable_name);
                push_slice_assume_capacity(&metaprogram_code, assignment);

                u64 column_mask = chars_per_row - 1;
                const char *chars = "0123456789abcdef";
                char byte_string[] = "0x!!,";
                for (u64 i = 0; i < input.count; i += 1) {
                    u64 column = i & column_mask;
                    if (column == 0) push_slice_assume_capacity(&metaprogram_code, indent);

                    u8 byte = (u8)input.data[i];

                    // NOTE(felix): this is much faster than string_builder_print in a hot, unoptimised loop. it is equivalent to:
                    // string_builder_print(&metaprogram_code, "0x%c%c,", chars[byte >> 4], chars[byte & 0x0f]);
                    byte_string[2] = chars[byte >> 4];
                    byte_string[3] = chars[byte & 0x0f];
                    push_many_assume_capacity(&metaprogram_code, byte_string, sizeof byte_string - 1);
                }
                push_slice_assume_capacity(&metaprogram_code, closing);

                push_slice_assume_capacity(&metaprogram_code, length_qualifiers);
                push_slice_assume_capacity(&metaprogram_code, array_length_name);
                push_slice_assume_capacity(&metaprogram_code, length_assignment);
                string_builder_print(&scratch, &metaprogram_code, "%llu", input.count);
                push_slice_assume_capacity(&metaprogram_code, length_closing);
            }
        } else if (string_equals(token_data, S("link_libc"))) {
            static bool specified_already = false;
            if (specified_already) {
                log_error("you can't specify `link_libc` multiple times");
                return 1;
            }
            specified_already = true;

            const char *error = "error: expected `link_libc = true/false`";
            if (!build_eat_token(&lexer, '=', error)) return 1;

            if (!build_eat_token(&lexer, clex_Token_IDENTIFIER, error)) return 1;
            token_data = base_string_from_clex_data(lexer.token.data);

            meta_link_crt_override = true;
            if (string_equals(token_data, S("true"))) {
                meta_link_crt_override_value = true;
            } else if (string_equals(token_data, S("false"))) {
                meta_link_crt_override_value = false;
            } else {
                log_error("expected `true` or `false` in `meta(link_libc = true/false)`, got `%S`", token_data);
                return 1;
            }

            if (!build_eat_token(&lexer, ')', "error: expected `)` to close `meta(...)` directive")) return 1;
        } else if (string_equals(token_data, S("link"))) {
            // TODO(felix)
        } else {
            log_error("unsupported metaprogram directive '%S'", token_data);
            return 1;
        }
    }
    if (lexer.token.kind == clex_Token_ERROR) {
        log_error("parser error");
        return 1;
    }

    f64 time_at_parse_end = time_relative_timestamp();
    f64 parse_seconds = time_seconds_between_relative_timestamps(time_at_parse_start, time_at_parse_end);
    print("Parsing took %.2lf seconds.\n", parse_seconds);

    if (metaprogram_code.count > 0) {
        if (metaprogram_output_file.count == 0) {
            log_error("missing `meta(%S)` followed by #include to designate codegen destination", generate_here_directive);
            return 1;
        }

        const char *file = cstring_from_string(&scratch, metaprogram_output_file);
        if (!os_write_entire_file(file, metaprogram_code.data, metaprogram_code.count)) return 1;
    }

    Build_Object main_object = { .code_path = c_file_path };
    push(&scratch, &objects, main_object);

    bool link_crt = false;
    if (meta_link_crt_override) {
        link_crt = meta_link_crt_override_value;
    } else {
        link_crt = target_os == BASE_OS_MACOS;
    }

    // TODO(felix): should be handled by metaprogram
    const char *include_paths[] = {
        cstring_print(&scratch, "-I%s", dependency_folder),
        cstring_print(&scratch, "-I%s", source_folder),
        cstring_print(&scratch, "-I..%c", path_separator),
        0,
    };

    Build_Mode build_mode = Build_Mode_DEBUG;
    for (u64 i = 0; i < argument_count; i += 1) {
        if (cstring_equals(arguments[i], "clang")) compiler = Build_Compiler_CLANG;
        if (cstring_equals(arguments[i], "release")) build_mode = Build_Mode_RELEASE;
    }

    Build_Linker linker = compiler == Build_Compiler_CLANG ? Build_Linker_CLANG : Build_Linker_MSVC;
    bool use_asan = link_crt && build_mode == Build_Mode_DEBUG;
    if (use_asan && linker == Build_Linker_CLANG && target_os == BASE_OS_WINDOWS) {
        // NOTE(felix): for some reason with clang I can only get an asan-enabled executable to run if I copy clang_rt.asan_dynamic-x86_64.dll to the build directory.
        // TODO(felix): implement logic for finding C:\Program Files\LLVM\lib\clang\[VERSION]\lib\windows\[asan dll], so we can do this
        // OR it's possible this is a clang/msvc version mismatch since there shouldn't be a difference
        print("info: disabling asan with clang on windows (see base.h, line %llu)\n", __LINE__ - 2);
        use_asan = false;
    }

    Os_Process_Flags process_flags =
        Os_Process_Flag_PRINT_COMMAND_BEFORE_RUNNING |
        Os_Process_Flag_PRINT_EXIT_CODE |
        Os_Process_Flag_PRINT_EXECUTION_TIME;

    for (u64 i = 0; i < objects.count; i += 1) {
        Build_Object *object = &objects.data[i];

        bool should_compile = object->code_path != 0;

        if (should_compile) {
            bool no_custom_object_path = object->object_path == 0;
            if (no_custom_object_path) {
                String code_path = string_from_cstring(object->code_path);
                String basename_with_path = string_cut_last(code_path, '.').head;
                String basename_no_path = string_cut_last(basename_with_path, path_separator).tail;

                object->object_path = cstring_print(&scratch, "%S.%s", basename_no_path, object_extension);
            }

            Array_cstring compile = {0};

            build_push_arguments(&scratch, &compile, build_compiler_initial_command[compiler]);

            // TODO(felix): should be handled by reading meta() annotations
            if (target_os == BASE_OS_WASM) {
                assert(compiler == Build_Compiler_CLANG);

                const char *clang_wasm_flags[BUILD_FLAGS_MAX] = {
                    "--target=wasm32",
                    0,
                };

                build_push_arguments(&scratch, &compile, clang_wasm_flags);
            }

            build_push_arguments(&scratch, &compile, include_paths);
            if (link_crt) push(&scratch, &compile, "-DLINK_CRT=1");

            if (!object->no_extra_errors) build_push_arguments(&scratch, &compile, build_compiler_extra_errors[compiler]);
            build_push_arguments(&scratch, &compile, build_compiler_disabled_errors[compiler]);

            build_push_arguments(&scratch, &compile, build_compiler_mode_flags[build_mode][compiler][target_os]);
            if (build_mode == Build_Mode_DEBUG) push(&scratch, &compile, "-DBUILD_DEBUG=1");
            if (use_asan) push(&scratch, &compile, "-fsanitize=address");

            build_push_arguments(&scratch, &compile, object->extra_flags);

            bool delete_pdb_first_because_msvc_incremental_is_broken = (target_os == BASE_OS_WINDOWS && compiler == Build_Compiler_MSVC);
            if (delete_pdb_first_because_msvc_incremental_is_broken) {
                String object_path = string_from_cstring(object->object_path);
                String object_basename = string_cut_last(object_path, '.').head;
                object_basename = string_cut_last(object_basename, path_separator).tail;

                const char *pdb_file = cstring_print(&scratch, "%S.pdb", object_basename);
                if (fs_exists(pdb_file)) {
                    bool ok = fs_remove_file(pdb_file);
                    if (!ok) log_internal("warning: failure removing old PDB '%s'", pdb_file);
                }
            }

            push(&scratch, &compile, object->code_path);

            const char *output = cstring_print(&scratch, "%s%s", build_compiler_out[compiler], object->object_path);
            push(&scratch, &compile, output);

            push(&scratch, &compile, 0);
            exit_code = os_process_run(scratch, compile.data, build_folder, process_flags);

            if (exit_code != 0) return exit_code;
        }
    }

    {
        Array_cstring link = {0};

        build_push_arguments(&scratch, &link, build_linker_initial_command[linker][target_os]);

        if (target_os == BASE_OS_WINDOWS) {
            if (build_mode == Build_Mode_DEBUG) {
                if (linker == Build_Linker_MSVC) {
                    push(&scratch, &link, "-debug");
                    push(&scratch, &link, "-subsystem:console");
                    if (link_crt) push(&scratch, &link, "libucrtd.lib");
                } else {
                    push(&scratch, &link, "-g");
                }
            } else {
                if (linker == Build_Linker_MSVC) {
                    push(&scratch, &link, "-subsystem:console");
                    // TODO(felix): subsystem:windows and entry:main don't work currently (silent crash). I'm pretty sure this has to do with the entrypoint being set incorrectly. I'm also not sure about the WINDOWS_SUBSYSTEM_[...] #defines in base_context.h
                    // push(&link, "-subsystem:windows");
                    if (link_crt) {
                        // push(&link, "-entry:main");
                        push(&scratch, &link, "libucrt.lib");
                    }
                }
            }

            // TODO(felix): handle in metaprogram
            if (!link_crt) switch (compiler) {
                case Build_Compiler_MSVC: push(&scratch, &link, "-entry:entrypoint"); break;
                case Build_Compiler_CLANG: {
                    push(&scratch, &link, "-Wl,-entry:entrypoint");
                    push(&scratch, &link, "-Wl,-subsystem:windows");
                } break;
                default: unreachable;
            }
        }

        if (linker == Build_Linker_CLANG && use_asan) push(&scratch, &link, "-fsanitize=address");

        for (u64 i = 0; i < objects.count; i += 1) push(&scratch, &link, objects.data[i].object_path);

        const char *syntax = target_os == BASE_OS_WINDOWS && (linker == Build_Linker_MSVC) ? "-out:" : "-o";
        const char *extension = build_binary_extension[target_os];

        const char *output_name = target_os == BASE_OS_WASM ? "module" : program_name;
        const char *output = cstring_print(&scratch, "%s%s.%s", syntax, output_name, extension);
        push(&scratch, &link, output);

        push(&scratch, &link, 0);
        exit_code = os_process_run(scratch, link.data, build_folder, process_flags);

        if (target_os == BASE_OS_WASM && exit_code == 0) {
            const char *html_output_path = cstring_print(&scratch, "%s%c%s.html", build_folder, path_separator, program_name);

            fs_remove_file(html_output_path);
            exit_code = !os_write_entire_file(html_output_path, build_wasm_html_file, sizeof build_wasm_html_file - 1);

            if (exit_code == 0) print("wrote '%s'\n", html_output_path);
            else print("unable to write '%s'\n", html_output_path);
        }
    }

    return exit_code;
}

static void app_frame_info_reset_end_of_frame(App_Frame_Info *frame) {
    frame->scroll = 0;
    memset(frame->mouse_clicked, 0, sizeof frame->mouse_clicked);
    memset(frame->key_pressed, 0, sizeof frame->key_pressed);
}

static void draw(Platform *platform, Draw_Command command) {
    push(platform->frame_arena, &platform->draw_commands, command);
}

static V2 draw_command_bounds(Draw_Command command) {
    V2 size = {0};

    switch (command.kind) {
        case Draw_Kind_SPRITE_1BIT: {
            size.x = (f32)command.sprite.width;
            size.y = (f32)command.sprite.height;
        } break;
        default: unreachable;
    }

    return size;
}

static void draw_many(Platform *platform, Draw_Command *commands, u64 count) {
    push_many(platform->frame_arena, &platform->draw_commands, commands, count);
}

#define UI_ASSERT assert
#define UI_VSNPRINTF small_vsnprintf
#define ui_size_t size_t
#define UI_IMPLEMENTATION
#include "base/ui.h"

static ui_Box *base_ui_draw_command(Arena *frame_arena, ui_State *ui, ui_Box *parent, Draw_Command command, ui_Flags flags, const ui_Style *style, const char *format, ...) {
    va_list arguments;
    va_start(arguments, format);
    ui_Box *box = ui_pushv(ui, parent, ui_Flag_DRAW_COMMAND | flags, format, arguments);
    va_end(arguments);

    assert(style != 0);
    box->style = *style;

    Draw_Command *copy = arena_make(frame_arena, 1, Draw_Command);
    *copy = command;
    box->user_data = copy;

    V2 bounds = draw_command_bounds(command);
    for (int axis = 0; axis < ui_Axis_COUNT; axis += 1) {
        box->size[axis] = (ui_Size){ .kind = ui_Size_PIXELS, .value = v(bounds)[axis] };
    }

    return box;
}

static void base_ui_input(ui_State *ui, App_Frame_Info *frame) {
    ui->window_size[ui_X] = frame->window_size.x;
    ui->window_size[ui_Y] = frame->window_size.y;

    ui->mouse_position[ui_X] = frame->mouse_position.x;
    ui->mouse_position[ui_Y] = frame->mouse_position.y;

    _Static_assert(sizeof ui->mouse_clicked == sizeof frame->mouse_clicked, "base & ui types must be identical");
    memcpy(ui->mouse_clicked, frame->mouse_clicked, sizeof ui->mouse_clicked);

    _Static_assert(sizeof ui->mouse_down == sizeof frame->mouse_down, "base & ui types must be identical");
    memcpy(ui->mouse_down, frame->mouse_down, sizeof ui->mouse_down);

    _Static_assert(sizeof ui->key_pressed == sizeof frame->key_pressed, "base & ui types must be identical");
    memcpy(ui->key_pressed, frame->key_pressed, sizeof ui->key_pressed);
}

static void base_ui_render(ui_State *ui, ui_Box *box) {
    Platform *platform = ui->user_data;

    if (box->flags & UI_FLAG_ANY_VISIBLE) {
        V4 clip = {0};
        if (box->flags & ui_Flag_CLIP_TO_PARENT) {
            assert(box->parent != 0);
            memcpy(&clip, &box->parent->rectangle, sizeof clip);
            f32 border_width = box->parent->style.border_width * (f32)!!(box->parent->flags & ui_Flag_DRAW_BORDER);
            clip.x += border_width;
            clip.y += border_width;
            clip.z -= 2.f * border_width;
            clip.w -= 2.f * border_width;
        }

        ui_Style style = box->style;
        u32 background_color = style.bg_color;
        u32 border_color = style.border_color;
        u32 foreground_color = style.fg_color;
        f32 border_width = style.border_width;
        f32 border_radius = style.border_radius;
        V2 shadow_offset = { style.shadow_offset[ui_X], style.shadow_offset[ui_Y] };
        u32 shadow_color = style.shadow_color;

        bool draw_shadow = !!(box->flags & ui_Flag_DRAW_SHADOW);
        if (draw_shadow) {
            Draw_Command command = {
                .kind = Draw_Kind_RECTANGLE,
                .rectangle = {
                    .border_radius = border_radius,
                    .size = { box->rectangle.size[0], box->rectangle.size[1] },
                },
            };
            command.position = v2_add((V2){ box->rectangle.position[0], box->rectangle.position[1] }, shadow_offset);
            command.color[Draw_Color_SOLID] = shadow_color;

            command.clip = clip;
            draw(platform, command);
        }

        bool draw_rectangle = !!(box->flags & (ui_Flag_DRAW_BORDER | ui_Flag_DRAW_BACKGROUND));
        if (draw_rectangle) {
            Draw_Command command = {
                .kind = Draw_Kind_RECTANGLE,
                .rectangle = {
                    .border_width = border_width * (f32)!!(box->flags & ui_Flag_DRAW_BORDER),
                    .border_radius = border_radius,
                },
            };
            command.position = (V2){ box->rectangle.position[0], box->rectangle.position[1] };
            command.color[Draw_Color_SOLID] = (u32)!!(box->flags & ui_Flag_DRAW_BACKGROUND) * background_color;
            command.rectangle.size = (V2){ box->rectangle.size[0], box->rectangle.size[1] };
            command.rectangle.border_color = border_color;

            command.clip = clip;
            draw(platform, command);
        }

        bool draw_text = !!(box->flags & ui_Flag_DRAW_TEXT);
        if (draw_text) {
            assert(box->display_string != 0 && box->display_string[0] != 0);

            V2 position = (V2){ box->rectangle.position[0], box->rectangle.position[1] };
            for (int axis = 0; axis < ui_Axis_COUNT; axis += 1) {
                f32 add = box->style.padding[axis];
                v(position)[axis] += add;
            }

            Draw_Command command = {
                .kind = Draw_Kind_TEXT,
                .text = {
                    .string = { .data = box->display_string, .count = cstring_length(box->display_string) },
                    .font_size = box->style.font_size,
                },
            };
            command.position = position;
            command.color[Draw_Color_SOLID] = foreground_color;

            command.clip = clip;
            draw(platform, command);
        }

        bool draw_command = !!(box->flags & ui_Flag_DRAW_COMMAND);
        if (draw_command) {
            Draw_Command *command = box->user_data;

            V2 command_size = draw_command_bounds(*command);

            V2 leftover = v2_sub((V2){ box->rectangle.size[0], box->rectangle.size[1] }, command_size);
            V2 pad_for_align = v2_scale(leftover, 0.5f);
            command->position = v2_add((V2){ box->rectangle.position[0], box->rectangle.position[1] }, pad_for_align);

            command->color[Draw_Color_SOLID] = foreground_color;

            draw(platform, *command);
        }
    }

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
