// Based on information from https://sourceforge.net/p/predef/wiki/Home/

#ifndef BASE_CONTEXT_CUSTOM


#ifdef _WIN32
    #define OS_WINDOWS 1
#else
    #error unsupported OS
#endif // OS_...

#ifndef OS_WINDOWS
    #define OS_WINDOWS 0
#endif
 

#if defined(_MSC_VER)
    #define COMPILER_MSVC 1
    #if !OS_WINDOWS
        #error not sure how we got here, but MSVC on a non-Windows OS is unsupported
    #endif // !OS_WINDOWS
#else
    #error unsupported compiler
#endif // COMPILER_...

#ifndef COMPILER_MSVC
    #define COMPILER_MSVC 0
#endif


#endif // !BASE_CONTEXT_CUSTOM
