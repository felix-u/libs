#define PLATFORM_NONE 1
#define BASE_IMPLEMENTATION
#include "src/base/base.h"

static void program(void) {
    Arena arena = arena_init(8 * 1024 * 1024);
    u32 exit_code = build_default_everything(arena, APP_NAME, 0);
    os_exit(exit_code);
}
