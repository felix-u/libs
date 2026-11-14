// Based on information from https://sourceforge.net/p/predef/wiki/Home/

#if !defined(BASE_CONTEXT_H)
#define BASE_CONTEXT_H

#if __STDC_VERSION__ >= 202311L
    #define BASE_C_VERSION 2023
#elif __STDC_VERSION__ >= 201112L
    #define BASE_C_VERSION 2011
#elif __STDC_VERSION__ >= 199901L
    #define BASE_C_VERSION 1999
#elif defined(__STDC__)
    #define BASE_C_VERSION 1989
#else
    #define BASE_C_VERSION 0
#endif

#define BASE_OS_WINDOWS       (1 << 0)
#define BASE_OS_MACOS         (1 << 1)
#define BASE_OS_IOS_DEVICE    (1 << 2)
#define BASE_OS_IOS_SIMULATOR (1 << 3)
#define BASE_OS_LINUX         (1 << 4)
#define BASE_OS_WASM          (1 << 5)
#define BASE_OS_COUNT         (1 << 6)

#define BASE_OS_ANY_IOS (BASE_OS_IOS_DEVICE | BASE_OS_IOS_SIMULATOR)
#define BASE_OS_ANY_APPLE (BASE_OS_MACOS | BASE_OS_ANY_IOS)
#define BASE_OS_ANY_POSIX (BASE_OS_ANY_APPLE | BASE_OS_LINUX)

#if defined(BASE_OS)
    //
#elif defined(__wasm__) || defined(__wasm32__)
    #define BASE_OS BASE_OS_WASM
#elif defined(__linux__) || defined(__gnu_linux__)
    #define BASE_OS BASE_OS_LINUX
#elif defined(__APPLE__) && defined(__MACH__)
    #if defined(__has_builtin) && __has_builtin(__is_target_os) && __is_target_os(ios)
        // TODO(felix): Work out what's going on with this.
        #if __has_builtin(__is_target_environment) && __is_target_environment(simulator)
            #define BASE_OS BASE_OS_IOS_SIMULATOR
        #else
            #define BASE_OS BASE_OS_IOS_DEVICE
        #endif
    #else
        #define BASE_OS BASE_OS_MACOS
    #endif
#elif defined(_WIN32)
    #define BASE_OS BASE_OS_WINDOWS
#else
    #error "could not context-crack OS; define BASE_OS manually before including this file"
#endif // OS_...

#define BASE_ARCH_X86_64 (1 << 0)
#define BASE_ARCH_ARM64  (1 << 1)
#define BASE_ARCH_WASM32 (1 << 2)
#define BASE_ARCH_COUNT  (1 << 3)

#if defined(BASE_ARCH)
    //
#elif defined(__wasm32__)
    #define BASE_ARCH BASE_ARCH_WASM32
#elif defined(__aarch64__) || defined(_M_ARM64) || defined(_M_ARM64EC)
    #define BASE_ARCH BASE_ARCH_ARM64
#elif defined(__x86_64__) || defined(_M_X64)
    #define BASE_ARCH BASE_ARCH_X86_64
#else
    #error "UNIMPLEMENTED. Define BASE_ARCH manually."
#endif // ARCH_...

#if BASE_ARCH == BASE_ARCH_X86_64 || BASE_ARCH == BASE_ARCH_ARM64
    #define BASE_ARCH_BITS 64
#elif BASE_ARCH == BASE_ARCH_WASM32
    #define BASE_ARCH_BITS 32
#else
    #error "UNREACHABLE"
#endif

#if defined(__clang__)
    #define COMPILER_CLANG 1
#elif defined(__GNUC__)
    #define COMPILER_GCC 1
#elif defined(_MSC_VER)
    #define COMPILER_MSVC 1
#else
    #define COMPILER_STANDARD 1
#endif // COMPILER_...

#ifndef COMPILER_CLANG
    #define COMPILER_CLANG 0
#endif
#ifndef COMPILER_GCC
    #define COMPILER_GCC 0
#endif
#ifndef COMPILER_MSVC
    #define COMPILER_MSVC 0
#endif

#if COMPILER_CLANG
    #if __has_feature(address_sanitizer)
        #define BUILD_ASAN 1
    #endif
#elif COMPILER_GCC || COMPILER_MSVC
    #ifdef __SANITIZE_ADDRESS__
        #define BUILD_ASAN 1
    #endif
#endif // COMPILER_...

#ifndef BUILD_ASAN
    #define BUILD_ASAN 0
#endif
#ifndef BUILD_DEBUG
    #define BUILD_DEBUG 0
#endif

#define WINDOWS_SUBSYSTEM_WINDOWS (1 << 0)
#define WINDOWS_SUBSYSTEM_CONSOLE (1 << 1)

#if !defined(WINDOWS_SUBSYSTEM)
    #define WINDOWS_SUBSYSTEM WINDOWS_SUBSYSTEM_WINDOWS
#endif

#if !defined(PLATFORM_NONE)
    #define PLATFORM_NONE 0
#endif


#endif // BASE_CONTEXT_H
