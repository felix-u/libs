#define PLATFORM_NONE 1
#define BASE_IMPLEMENTATION
#include "src/base/base.h"

#define APP_NAME "app"

static void program(void) {
    Arena arena = arena_init(64 * 1024 * 1024);

    u8 target_os = BASE_OS;
    Slice_String arguments = os_get_arguments(&arena);
    for_slice (String *, argument, arguments) {
        if (string_equals(*argument, string("web"))) target_os = BASE_OS_EMSCRIPTEN;
    }

    u32 exit_code = build_default_everything(arena, string(APP_NAME), target_os);
    os_exit(exit_code);
}
