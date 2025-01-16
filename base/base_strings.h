structdef(Str8) { u8 *ptr; usize len; };
#define str8c(s)  { .ptr = (u8 *)s, .len = sizeof(s) - 1 }
#define str8(s) (Str8)str8c(s)

structdef(Str16) { u16 *ptr; usize len; };
#define str16c(s) { .ptr = (u16 *)s, .len = sizeof(s) / sizeof(u16) - 1 }
#define str16(s) (Str16)str16c(s)

structdef(Str8_Builder) { u8 *ptr; usize len, cap; Arena *arena; };

structdef(Format) {
    u32 magic; // TODO(felix): remove
    enum {
        format_type_char,
        format_type_u64,
        format_type_f32,
        format_type_f64,
        format_type_cstring,
        format_type_Str8,
    } type;
    union {
        u8 value_char;
        u64 value_u64;
        f32 value_f32;
        f64 value_f64;
        char *value_cstring;
        Str8 value_Str8;
    };
    u8 base; // TODO(felix)
    bool prefix; // TODO(felix)
    bool uppercase; // TODO(felix)
};

#define fmt_magic_number 1234567890
#define fmt(type_, ...) (Format){ .magic = fmt_magic_number, .type = format_type_##type_, .value_##type_ = __VA_ARGS__ }

#define strc_printf(arena_ptr, fmt, ...)\
    (char *)(str8_printf(arena_ptr, fmt "\0", __VA_ARGS__).ptr)

#define _for_valid_hex_digit(action)\
    action('0', 0x0) action('1', 0x1) action('2', 0x2) action('3', 0x3)\
    action('4', 0x4) action('5', 0x5) action('6', 0x6) action('7', 0x7)\
    action('8', 0x8) action('9', 0x9) action('A', 0xa) action('B', 0xb)\
    action('C', 0xc) action('D', 0xd) action('E', 0xe) action('F', 0xf)\
    action('a', 0xa) action('b', 0xb) action('c', 0xc) action('d', 0xd)\
    action('e', 0xe) action('f', 0xf)

const u8 decimal_from_hex_digit_table[256] = {
    #define _make_hex_digit_value_table(c, val) [c] = val,
    _for_valid_hex_digit(_make_hex_digit_value_table)
};

const b8 is_hex_digit_table[256] = {
    #define _make_hex_digit_truth_table(c, val) [c] = true,
    _for_valid_hex_digit(_make_hex_digit_truth_table)
};

static usize decimal_from_hex_str8(Str8 s);

static char *strc_from_str8(Arena *arena, Str8 s);

static bool str8_eql(Str8 s1, Str8 s2);
static Str8 str8_from_strc(char *s);
static Str8 str8_from_int_base(Arena *arena, usize _num, u8 base);
static Str8 str8_printf(Arena *arena, char *fmt, ...);
static Str8 str8_range(Str8 s, usize beg, usize end);
static Str8 str8_vprintf(Arena *arena, char *fmt, va_list args);

static Str16 str16_from_str8(Arena *arena, Str8 s);

static void str8_builder_null_terminate(Str8_Builder *builder);
static void str8_builder_printf(Str8_Builder *builder, char *fmt, ...);
static void str8_builder_printf_var_args(Str8_Builder *builder, char *fmt, va_list args);
static void str8_builder_push_f64(Str8_Builder *builder, f64 value);
static void str8_builder_push_u64(Str8_Builder *builder, u64 value);
static void str8_builder_push_char(Str8_Builder *builder, u8 c);
static void str8_builder_push_str8(Str8_Builder *builder, Str8 str);
static Str8 str8_from_str8_builder(Str8_Builder builder);

