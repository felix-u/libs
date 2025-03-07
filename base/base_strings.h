structdef(String) { u8 *data; usize count; };
#define stringc(s)  { .data = (u8 *)s, .count = sizeof(s) - 1 }
#define string(s) (String)stringc(s)

structdef(String16) { u16 *data; usize count; };
#define string16c(s) { .data = (u16 *)s, .count = sizeof(s) / sizeof(u16) - 1 }
#define string16(s) (String16)string16c(s)

typedef Array_u8 String_Builder;

structdef(Format) {
    enum {
        format_type_char,
        format_type_u64,
        format_type_f32,
        format_type_f64,
        format_type_V2,
        format_type_cstring,
        format_type_String,
    } type;
    union {
        u8 value_char;
        u64 value_u64;
        f32 value_f32;
        f64 value_f64;
        V2 value_V2;
        char *value_cstring;
        String value_String;
    };
    u8 base; // TODO(felix)
    bool prefix; // TODO(felix)
    bool uppercase; // TODO(felix)
};

#define fmt(type_, ...) (Format){ .type = format_type_##type_, .value_##type_ = __VA_ARGS__ }

#define cstring_print(arena_ptr, fmt, ...)\
    (char *)(string_print(arena_ptr, fmt "\0", __VA_ARGS__).data)

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

const bool is_hex_digit[256] = {
    #define _make_hex_digit_truth_table(c, val) [c] = true,
    _for_valid_hex_digit(_make_hex_digit_truth_table)
};

static usize int_from_hex_string(String s);

static char *cstring_from_string(Arena *arena, String s);

static bool string_equal(String s1, String s2);
static String string_from_cstring(char *s);
static String string_from_int_base(Arena *arena, usize _num, u8 base);
static String string_print(Arena *arena, char *fmt, ...);
static String string_range(String string, usize start, usize end);
static String string_vprint(Arena *arena, char *fmt, va_list args);

static String16 string16_from_string(Arena *arena, String s);

static void string_builder_print(String_Builder *builder, char *fmt, ...);
static void string_builder_print_var_args(String_Builder *builder, char *fmt, va_list args);
static void string_builder_push_f32(String_Builder *builder, f32 value);
static void string_builder_push_f64(String_Builder *builder, f64 value);
static void string_builder_push_V2(String_Builder *builder, V2 value);
static void string_builder_push_u64(String_Builder *builder, u64 value);
static void string_builder_push_char(String_Builder *builder, u8 c);
static void string_builder_push_string(String_Builder *builder, String str);
static void string_builder_null_terminate(String_Builder *builder);
