static Gfx g_gfx;
static u32 *g_palette;

#include "gfx_common.c"

#if OS_WINDOWS
    #include "gfx_windows.c"
#elif OS_EMSCRIPTEN
    #include "gfx_emscripten.c"
#else
    #error unsupported OS
#endif // OS
