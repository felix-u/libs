#if defined(BASE_NO_IMPLEMENTATION)
    #include "base_context.h"
#endif

#include "base_core.c"
#include "base_ascii.c"
#include "base_arena.c"
#include "base_map.c"
#include "base_math.c"
#include "base_strings.c"
#include "base_os.c"
#if !PLATFORM_NONE
    #include "base_app.c"
    #include "base_ui.c"
#endif
