// https://github.com/felix-u 2026-11-03
// Public domain. NO WARRANTY - use at your own risk.

#if !defined(TIME_H)
#define TIME_H


#if defined(__wasm__) || defined(__wasm32__)
    #define TIME_OS_WASM
#elif defined(_WIN32)
    #define TIME_OS_WINDOWS
#else
    // NOTE(felix): it may be good to distinguish between non-Windows OSes
    #define TIME_OS_POSIX
#endif

#if !defined(TIME_NO_SYSTEM_INCLUDE)
    #if defined(TIME_OS_WINDOWS)
        #if defined(_MSC_VER)
            #pragma comment(lib, "Ntdll.lib")
        #endif
    #elif defined(TIME_OS_POSIX)
        #include <time.h>
    #endif
#endif

#if !defined(TIME_FUNCTION)
    #define TIME_FUNCTION
#endif

typedef struct {
    double *samples;
    unsigned count;
    unsigned index;
    double last_timestamp;
    _Bool filled_all_samples_at_least_once;
} Time_Averaged_Sampler;

TIME_FUNCTION double                time_relative_timestamp(void);
TIME_FUNCTION double                time_averaged_sample_elapsed_seconds(Time_Averaged_Sampler *sampler);
TIME_FUNCTION Time_Averaged_Sampler time_averaged_sampler(double *samples, unsigned sample_count);
TIME_FUNCTION double                time_seconds_between_relative_timestamps(double former, double latter);


#endif // TIME_H


#if defined(TIME_IMPLEMENTATION)


#if defined(TIME_OS_WASM) && !defined(TIME_JS_PERFORMANCE_NOW)
    #error "define `double TIME_JS_PERFORMANCE_NOW(void)` (intended to be used with performance.now() from JS)"
#endif

#if defined(TIME_OS_WINDOWS)
    unsigned char RtlQueryPerformanceCounter(long long *PerformanceCounter);
    unsigned char RtlQueryPerformanceFrequency(long long *PerformanceFrequency);
#elif defined(TIME_OS_POSIX)
    #if defined(CLOCK_MONOTONIC)
        #define TIME__CLOCK_MONOTONIC CLOCK_MONOTONIC
    #elif defined(__APPLE__)
        #define TIME__CLOCK_MONOTONIC 6
    #elif defined(__linux__)
        #define TIME__CLOCK_MONOTONIC 1
    #else
        #error "UNIMPLEMENTED: the value of CLOCK_MONOTONIC on this OS"
    #endif
#endif

TIME_FUNCTION double time_relative_timestamp(void) {
    double result = 0;

    #if defined(TIME_OS_WINDOWS)
    {
        static long long ticks_per_second = 0;
        if (ticks_per_second == 0) RtlQueryPerformanceFrequency(&ticks_per_second);

        long long ticks = 0;
        RtlQueryPerformanceCounter(&ticks);
        result = (double)ticks / (double)ticks_per_second;
    }
    #elif defined(TIME_OS_POSIX)
    {
        struct timespec spec = {0};
        clock_gettime(TIME__CLOCK_MONOTONIC, &spec);
        double nanoseconds = (double)spec.tv_nsec;
        result = (double)spec.tv_sec + nanoseconds * 1e-9;
    }
    #elif defined(TIME_OS_WASM)
    {
        double milliseconds = TIME_JS_PERFORMANCE_NOW();
        result = milliseconds * 0.001;
    }
    #else
        #error "UNIMPLEMENTED"
    #endif

    return result;
}

TIME_FUNCTION double time_averaged_sample_elapsed_seconds(Time_Averaged_Sampler *sampler) {
    double average = 0;
    double now = time_relative_timestamp();

    if (sampler->last_timestamp != 0.0) {
        double elapsed = time_seconds_between_relative_timestamps(sampler->last_timestamp, now);

        sampler->samples[sampler->index] = elapsed;

        if (sampler->index + 1 == sampler->count) sampler->filled_all_samples_at_least_once = 1;

        unsigned sample_max = sampler->filled_all_samples_at_least_once ? sampler->count : sampler->index + 1;
        sampler->index += 1;
        sampler->index %= sampler->count;

        for (unsigned i = 0; i < sample_max; i += 1) average += sampler->samples[i];
        average /= (double)sample_max;
    }

    sampler->last_timestamp = now;
    return average;
}

TIME_FUNCTION Time_Averaged_Sampler time_averaged_sampler(double *samples, unsigned sample_count) {
    Time_Averaged_Sampler sampler = { .samples = samples, .count = sample_count };
    return sampler;
}

TIME_FUNCTION double time_seconds_between_relative_timestamps(double former, double latter) {
    return latter - former;
}


#endif // TIME_IMPLEMENTATION
