#define PLATFORM_NONE 1
#define BASE_IMPLEMENTATION
#include "src/base/base.h"

#define APP_NAME "app"

static void program(void) {
    Arena arena = arena_init(8 * 1024 * 1024);

    u8 target_os = BASE_OS;

    u64 argument_count = 0;
    const char **arguments = os_get_arguments(&arena, &argument_count);

    for (u64 i = 0; i < argument_count; i += 1) {
        String argument = string_from_cstring(arguments[i]);
        if (string_equals(argument, string("web"))) target_os = BASE_OS_WASM;
    }

    u32 exit_code = build_default_everything(arena, APP_NAME, target_os);
    os_exit(exit_code);
}
