#if defined(BASE_NO_IMPLEMENTATION) || defined(BASE_NO_IMPLEMENTATION_STRINGS)

uniondef(String_Builder) {
    using(Array_u8, bytes);
    struct { String string; u64 _capacity; struct Arena *_arena; };
};

typedef void ***EXPECTED_A_FORMAT_STRUCT;

structdef(Format) {
    #if BUILD_DEBUG
        EXPECTED_A_FORMAT_STRUCT magic_value_for_debug;
    #endif
    enum {
        format_type_dummy = 0,
        format_type_bool,
        format_type_char,
        format_type_u8, format_type_u16, format_type_u32, format_type_u64,
        format_type_i8, format_type_i16, format_type_i32, format_type_i64,
        format_type_f32,
        format_type_f64,
        format_type_V2,
        format_type_cstring,
        format_type_String,
    } type;
    union {
        bool value_bool;
        u8 value_char;
        u8 value_u8; u16 value_u16; u32 value_u32; u64 value_u64;
        i8 value_i8; i16 value_i16; i32 value_i32; i64 value_i64;
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

// why not? this is the background colour of the acme editor from plan9
#define format_magic_value_for_debug (EXPECTED_A_FORMAT_STRUCT)(0xfeffec)

#if BUILD_DEBUG
    #define fmt(type_, ...) (Format){ .magic_value_for_debug = format_magic_value_for_debug,\
                                      .type = format_type_##type_, .value_##type_ = __VA_ARGS__ }
#else
    #define fmt(type_, ...) (Format){ .type = format_type_##type_, .value_##type_ = __VA_ARGS__ }
#endif

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

#define formats_from_va_args(...) ((Slice_Format)slice_of(Format, (Format){0}, __VA_ARGS__))

static u64 int_from_string_base(String s, u64 base);

static char *cstring_from_string(Arena *arena, String string);

static bool   string_equals(String s1, String s2);
static String string_from_cstring(char *s);
static String string_from_int_base(Arena *arena, u64 _num, u8 base);

#define string_print(arena, format, ...) string_print_((arena), (format), formats_from_va_args(__VA_ARGS__))
static String string_print_(Arena *arena, char *fmt, Slice_Format arguments);

static String string_range(String string, u64 start, u64 end);

static String16 string16_from_string(Arena *arena, String s);

#define string_builder_print(builder, format, ...) string_builder_print_((builder), (format), formats_from_va_args(__VA_ARGS__))
static void string_builder_print_(String_Builder *builder, char *fmt, Slice_Format arguments);

#define string_builder_push(builder, type_, ...) string_builder_push_format(builder, fmt(type_, __VA_ARGS__))
static void string_builder_push_format(String_Builder *builder, Format format);
static inline void string_builder_null_terminate(String_Builder *builder);


#else // IMPLEMENTATION


static u64 int_from_string_base(String s, u64 base) {
    u64 result = 0, magnitude = s.count;
    for (u64 i = 0; i < s.count; i += 1, magnitude -= 1) {
        result *= base;
        u64 digit = decimal_from_hex_digit_table[s.data[i]];
        if (digit >= base) {
            log_error("digit '%' is invalid in base %", fmt(char, s.data[i]), fmt(u64, base));
            return 0;
        }
        result += digit;
    }
    return result;
}

static bool string_equals(String s1, String s2) {
    if (s1.count != s2.count) return false;
    return memcmp_(s1.data, s2.data, s1.count) == 0;
}

static String string_from_cstring(char *s) {
    if (s == NULL) return (String){ 0 };
    u64 count = 0;
    while (s[count] != '\0') count += 1;
    return (String){ .data = (u8 *)s, .count = count };
}

static char *cstring_from_string(Arena *arena, String string) {
    char *cstring = arena_make(arena, string.count + 1, sizeof(char));
    for (u64 i = 0; i < string.count; i += 1) cstring[i] = (char)string.data[i];
    cstring[string.count] = '\0';
    return cstring;
}

// Only bases <= 10
static String string_from_int_base(Arena *arena, u64 _num, u8 base) {
    String str = {0};
    u64 num = _num;

    do {
        num /= base;
        str.count += 1;
    } while (num > 0);

    str.data = arena_make(arena, str.count, sizeof(u8));

    num = _num;
    for (i64 i = (i64)str.count - 1; i >= 0; i -= 1) {
        str.data[i] = (u8)((num % base) + '0');
        num /= base;
    }

    return str;
}

static String string_range(String string, u64 start, u64 end) {
    assert(end <= string.count);
    assert(start <= end);
    String result = { .data = string.data + start, .count = end - start };
    return result;
}

static String string_print_(Arena *arena, char *fmt, Slice_Format arguments) {
    String_Builder builder = { .arena = arena };
    string_builder_print_(&builder, fmt, arguments);
    return builder.string;
}

static String16 string16_from_string(Arena *arena, String s) {
    #if OS_WINDOWS
        String16 wstr = {
            .data = arena_make(arena, s.count, sizeof(u16)),
            .count = (u64)MultiByteToWideChar(CP_UTF8, 0, (char *)s.data, (int)s.count, wstr.data, (int)s.count),
        };
        win32_ensure_not_0(wstr.count);
        return wstr;
    #else
        discard(arena);
        discard(s);
        panic("unimplemented for this OS");
    #endif
}

static void format_skip_dummy_if_there(Slice_Format *arguments) {
    if (arguments->count > 0 && arguments->data[0].type == format_type_dummy) slice_shift(arguments);
}

static void string_builder_print_(String_Builder *builder, char *fmt_c, Slice_Format arguments) {
    String fmt_str = string_from_cstring(fmt_c);
    assert(fmt_str.count > 0);

    format_skip_dummy_if_there(&arguments);
    if (BUILD_DEBUG) {
        u64 format_count = 0;
        for (u64 i = 0; i < fmt_str.count; i += 1) format_count += (fmt_str.data[i] == '%');
        assert(arguments.count == format_count);
    }

    for (u64 i = 0; i < fmt_str.count; i += 1) {
        u64 beg_i = i;
        while (fmt_str.data[i] != '%' && i < fmt_str.count) i += 1;

        if (i == fmt_str.count) {
            string_builder_push(builder, String, string_range(fmt_str, beg_i, i));
            return;
        }

        assert(fmt_str.data[i] == '%');

        if (i > beg_i) string_builder_push(builder, String, string_range(fmt_str, beg_i, i));

        Format format = *slice_shift(&arguments);
        string_builder_push_format(builder, format);
    }
}

static void string_builder_push_format(String_Builder *builder, Format format) {
    #if BUILD_DEBUG
    {
        // This isn't *guaranteed* to trigger on a non-Format argument in print() because we may segfault when attempting to pop a Format value from variadic arguments.
        // The point is that we'll crash either way.
        bool format_argument_is_correctly_formed = format.magic_value_for_debug == format_magic_value_for_debug;
        assert(format_argument_is_correctly_formed);
    }
    #endif
    switch (format.type) {
        case format_type_bool: {
            // TODO(felix): support uppercase
            String string = format.value_bool ? string("true") : string("false");
            string_builder_push(builder, String, string);
        } break;
        case format_type_char: push(builder, format.value_char); break;
        case format_type_i8:  format.type = format_type_i64; format.value_i64 = (i64)format.value_i8; string_builder_push_format(builder, format); break;
        case format_type_i16: format.type = format_type_i64; format.value_i64 = (i64)format.value_i16; string_builder_push_format(builder, format); break;
        case format_type_i32: format.type = format_type_i64; format.value_i64 = (i64)format.value_i32; string_builder_push_format(builder, format); break;
        case format_type_i64: {
            i64 value = format.value_i64;
            if (value < 0) {
                string_builder_push(builder, char, '-');
                value = -value;
            }
            format.type = format_type_u64;
            format.value_u64 = (u64)value;
            string_builder_push_format(builder, format);
        } break;
        case format_type_u8:  format.type = format_type_u64; format.value_u64 = (u64)format.value_u8; string_builder_push_format(builder, format); break;
        case format_type_u16: format.type = format_type_u64; format.value_u64 = (u64)format.value_u16; string_builder_push_format(builder, format); break;
        case format_type_u32: format.type = format_type_u64; format.value_u64 = (u64)format.value_u32; string_builder_push_format(builder, format); break;
        case format_type_u64: {
            u64 value = format.value_u64;
            if (value == 0) {
                string_builder_push(builder, char, '0');
                break;
            }

            u8 buf_mem[sizeof(u64) * 8];
            u64 buf_index = sizeof(buf_mem);

            u8 base = format.base == 0 ? 10 : format.base;
            assert(base <= 16);

            // TODO(felix): support capitalised
            char *character_from_digit = "0123456789abcdef";

            for (; value > 0; value /= base) {
                buf_index -= 1;
                u8 digit = (u8)(value % base);
                buf_mem[buf_index] = (u8)character_from_digit[digit];
            }

            String decimal = { .data = buf_mem + buf_index, .count = sizeof(buf_mem) - buf_index };
            string_builder_push(builder, String, decimal);
        } break;
        case format_type_f32: string_builder_push(builder, f64, (f64)format.value_f32); break;
        case format_type_f64: {
            // References:
            // https://github.com/nothings/stb/blob/master/stb_sprintf.h
            // https://en.wikipedia.org/wiki/Double-precision_floating-point_format
            // https://dl.acm.org/doi/pdf/10.1145/3360595
            //
            // Most valuable:
            // https://github.com/charlesnicholson/nanoprintf
            // http://0x80.pl/notesen/2015-12-29-float-to-string.html
            // TODO(felix): implement this properly! ^

            f64 value = format.value_f64;

            u64 bits = bit_cast(u64) value;
            i64 biased_exponent = (bits >> 52) & 0x7ff;
            u64 mantissa_mask = UINT64_MAX >> 12;
            u64 mantissa = bits & mantissa_mask;

            u8 is_negative = (u8)(bits >> 63);
            if (is_negative) string_builder_push(builder, char, '-');

            if (biased_exponent == 0x7ff) {
                String string = (mantissa == 0) ? string("Infinity") : string("NaN");
                string_builder_push(builder, String, string);
                break;
            }

            if (biased_exponent == 0 && mantissa == 0) {
                string_builder_push(builder, char, '0');
                break;
            }

            // NOTE(felix): this can only represent integer parts up to UINT64_MAX!
            f64 absolute_value = is_negative ? -value : value;
            if (absolute_value > (f64)UINT64_MAX) {
                string_builder_push(builder, String, string("(...)"));
                break;
            }

            string_builder_push(builder, u64, (u64)absolute_value);

            string_builder_push(builder, char, '.');

            f64 fraction = absolute_value;

            // TODO(felix): add ability to configure in format argument
            u64 precision = 2;

            for (u64 i = 0; i < precision; i += 1) {
                fraction -= (f64)(u64)fraction;
                fraction *= 10.0;
                u8 fraction_as_char = (u8)fraction + '0';
                string_builder_push(builder, char, fraction_as_char);
            }
        } break;
        case format_type_V2: {
            V2 value = format.value_V2;
            string_builder_push(builder, String, string("{ x = "));
            string_builder_push(builder, f32, value.x);
            string_builder_push(builder, String, string(", y = "));
            string_builder_push(builder, f32, value.y);
            string_builder_push(builder, String, string(" }"));
        } break;
        case format_type_cstring: string_builder_push(builder, String, string_from_cstring(format.value_cstring)); break;
        case format_type_String: push_slice(builder, format.value_String); break;
        default: unreachable;
    }
}

static inline void string_builder_null_terminate(String_Builder *builder) {
    // Avoid push(0) because we don't want to increase the string length
    reserve(builder, builder->count + 1);
    builder->data[builder->count] = 0;
}

#endif // defined(BASE_NO_IMPLEMENTATION) || defined(BASE_NO_IMPLEMENTATION_STRINGS)
