// https://github.com/felix-u 2026-01-08
// Public domain. NO WARRANTY - use at your own risk.

#if !defined(TIME_H)
#define TIME_H


#if defined(_WIN32)
    #define TIME_OS_WINDOWS
#else
    // NOTE(felix): it may be good to distinguish between non-Windows OSes
    #define TIME_OS_POSIX
#endif

#if !defined(TIME_NO_SYSTEM_INCLUDE)
    #if defined(TIME_OS_WINDOWS)
        #if defined(_MSC_VER)
            #pragma comment(lib, "Kernel32.lib")
        #endif
        #include <windows.h>
    #endif
#endif

#if !defined(TIME_FUNCTION)
    #define TIME_FUNCTION
#endif

TIME_FUNCTION double time_elapsed_seconds(double former, double latter);
TIME_FUNCTION double time_now(void);


#endif // TIME_H


#if defined(TIME_IMPLEMENTATION)


TIME_FUNCTION double time_elapsed_seconds(double former, double latter) {
    return latter - former;
}

TIME_FUNCTION double time_now(void) {
    double result = 0;

    #if defined(TIME_OS_WINDOWS)
    {
        static LARGE_INTEGER ticks_per_second = {0};
        if (ticks_per_second.QuadPart == 0) QueryPerformanceFrequency(&ticks_per_second);

        LARGE_INTEGER ticks = {0};
        QueryPerformanceCounter(&ticks);
        result = (f64)ticks.QuadPart / (double)(ticks_per_second.QuadPart);
    }
    #else
        #error "UNIMPLEMENTED"
    #endif

    return result;
}


#endif // TIME_IMPLEMENTATION
