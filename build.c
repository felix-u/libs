#define PLATFORM_NONE 1
#define BASE_IMPLEMENTATION
#include "src/base/base.h"

#define APP_NAME "app"

static void program(void) {
    Arena arena = arena_init(8 * 1024 * 1024);
    u8 target_os = BASE_OS;
    u32 exit_code = build_default_everything(arena, APP_NAME, target_os);
    os_exit(exit_code);
}
