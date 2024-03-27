#include "base.c"

#include "args.c"

#define PROGRAM_NAME "name"

#define version_lit "0.1-dev"
const Str8 version_text = str8(PROGRAM_NAME " version " version_lit "\n");

const Str8 help_text = str8(
PROGRAM_NAME " (version " version_lit ")\n"
"\n"
"Usage: " PROGRAM_NAME " [options]\n"
"\n"
"Options:\n"
"  -h, --help\n"
"        Print this help and exit\n"
"      --version\n"
"        Print version information and exit\n"
);

typedef struct {
    Arena arena;
    int argc;
    char **argv;
} Context;

static error main_wrapper(Context *ctx) {
    try (arena_init(&ctx->arena, 4 * 1024));

    Args_Flag help_flag_short = { .name = str8("h") };
    Args_Flag help_flag_long = { .name = str8("help") };
    Args_Flag version_flag = { .name = str8("version") };
    Args_Flag *flags[] = {
        &help_flag_short, &help_flag_long,
        &version_flag,
    };
    Args_Desc args_desc = {
        .flags = slice(flags),
    };
    try (args_parse(ctx->argc, ctx->argv, &args_desc));

    if (help_flag_short.is_present || help_flag_long.is_present) {
        printf("%.*s", str8_fmt(help_text));
        return 0;
    }

    if (version_flag.is_present) {
        printf("%.*s", str8_fmt(version_text));
        return 0;
    }

    return 0;
}

int main(int argc, char **argv) {
    if (argc == 1) {
        printf("%.*s", str8_fmt(help_text));
        return 1;
    }

    Context ctx = { .argc = argc, .argv = argv };
    error e = main_wrapper(&ctx);
    arena_deinit(&ctx.arena);
    return e;
}
