static usize int_from_hex_string(String s) {
    usize result = 0, magnitude = s.count;
    for (usize i = 0; i < s.count; i += 1, magnitude -= 1) {
        usize hex_digit = decimal_from_hex_digit_table[s.data[i]];
        for (usize j = 1; j < magnitude; j += 1) hex_digit *= 16;
        result += hex_digit;
    }
    return result;
}

static bool string_equal(String s1, String s2) {
    if (s1.count != s2.count) return false;
    return memcmp(s1.data, s2.data, s1.count) == 0;
}

static String string_from_cstring(char *s) {
    if (s == NULL) return (String){ 0 };
    usize count = 0;
    while (s[count] != '\0') count += 1;
    return (String){ .data = (u8 *)s, .count = count };
}

static char *cstring_from_string(Arena *arena, String s) {
    char *cstring = arena_make(arena, s.count + 1, sizeof(char));
    for (usize i = 0; i < s.count; i += 1) cstring[i] = (char)s.data[i];
    cstring[s.count] = '\0';
    return cstring;
}

// Only bases <= 10
static String string_from_int_base(Arena *arena, usize _num, u8 base) {
    String str = {0};
    usize num = _num;

    do {
        num /= base;
        str.count += 1;
    } while (num > 0);

    str.data = arena_make(arena, str.count, sizeof(u8));

    num = _num;
    for (isize i = (isize)str.count - 1; i >= 0; i -= 1) {
        str.data[i] = (u8)((num % base) + '0');
        num /= base;
    }

    return str;
}

static String string_print(Arena *arena, char *format, ...) {
    va_list args;
    va_start(args, format);
    String result = string_vprint(arena, format, args);
    va_end(args);
    return result;
}

static String string_range(String string, usize start, usize end) {
    assert(end <= string.count);
    assert(start < end);
    String result = { .data = string.data + start, .count = end - start };
    return result;
}

static String string_vprint(Arena *arena, char *fmt, va_list args) {
    String_Builder builder = { .arena = arena };
    string_builder_print_var_args(&builder, fmt, args);
    return bit_cast(String) builder;
}

static String16 string16_from_string(Arena *arena, String s) {
    #if OS_WINDOWS
        String16 wstr = {
            .data = arena_make(arena, s.count, sizeof(u16)),
            .count = MultiByteToWideChar(CP_UTF8, 0, (char *)s.data, (int)s.count, wstr.data, (int)s.count),
        };
        win32_assert_not_0(wstr.count);
        return wstr;
    #else
        discard(arena);
        discard(s);
        panic("unimplemented for this OS");
    #endif
}

static void string_builder_print(String_Builder *builder, char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    string_builder_print_var_args(builder, fmt, args);
    va_end(args);
}

// TODO(felix): in debug mode, magic number to catch variadic arguments which are not Format structs
// TODO(felix): special format argument to mark end of va_list
static void string_builder_print_var_args(String_Builder *builder, char *fmt_c, va_list args) {
    String fmt_str = string_from_cstring(fmt_c);
    assert(fmt_str.count > 0);

    for (usize i = 0; i < fmt_str.count; i += 1) {
        usize beg_i = i;
        while (fmt_str.data[i] != '%' && i < fmt_str.count) i += 1;

        if (i == fmt_str.count) {
            string_builder_push_string(builder, string_range(fmt_str, beg_i, i));
            return;
        }

        assert(fmt_str.data[i] == '%');

        if (i > beg_i) string_builder_push_string(builder, string_range(fmt_str, beg_i, i));

        Format format = va_arg(args, Format);
        switch (format.type) {
            case format_type_char: string_builder_push_char(builder, format.value_char); break;
            case format_type_u64: string_builder_push_u64(builder, format.value_u64); break;
            case format_type_f32: string_builder_push_f32(builder, format.value_f32); break;
            case format_type_f64: string_builder_push_f64(builder, format.value_f64); break;
            case format_type_V2: string_builder_push_V2(builder, format.value_V2); break;
            case format_type_cstring: string_builder_push_string(builder, string_from_cstring(format.value_cstring)); break;
            case format_type_String: string_builder_push_string(builder, format.value_String); break;
            default: unreachable;
        }
    }
}

static void string_builder_push_f32(String_Builder *builder, f32 value) {
    // TODO(felix): actual implementation
    string_builder_push_f64(builder, (f64)value);
}

static void string_builder_push_f64(String_Builder *builder, f64 value) {
    // References:
    // https://dl.acm.org/doi/pdf/10.1145/3360595
    // https://en.wikipedia.org/wiki/Double-precision_floating-point_format
    bool is_ryu_implemented = false;

    if (is_ryu_implemented) {
        u64 bits = *(u64 *)&value;
        i64 exponent_biased = (bits >> 52) & 0x7ff;
        u64 mantissa = bits & 0x000fffffffffffff;

        b8 is_negative = (b8)(bits >> 63);
        if (is_negative) string_builder_push_char(builder, '-');

        if (exponent_biased == 0x7ff) {
            String string = (mantissa == 0) ? string("Infinity") : string("NaN");
            string_builder_push_string(builder, string);
            return;
        }

        i64 exponent;
        if (exponent_biased == 0) {
            if (mantissa == 0) {
                if (is_negative) string_builder_push_char(builder, '-');
                string_builder_push_char(builder, '0');
                return;
            }
            mantissa |= (u64)1 << 52;
            exponent = -1022;
        } else exponent = exponent_biased - 1023;

        // TODO(felix): rest of ryu algorithm
        discard(exponent);
    } else {
        // placeholder
        u8 buf[128] = {0};
        String printed = { .data = buf, .count = (usize)snprintf((char *)buf, sizeof(buf), "%.2f", value) };
        string_builder_push_string(builder, printed);
    }
}

static void string_builder_push_V2(String_Builder *builder, V2 value) {
    string_builder_push_string(builder, string("{ x = "));
    string_builder_push_f32(builder, value.x);
    string_builder_push_string(builder, string(", y = "));
    string_builder_push_f32(builder, value.y);
    string_builder_push_string(builder, string(" }"));
}

static void string_builder_push_u64(String_Builder *builder, u64 value) {
    if (value == 0) {
        string_builder_push_char(builder, '0');
        return;
    }

    // UINT64_MAX = 18,446,744,073,709,551,615
    // has 20 digits
    u8 buf_mem[20];
    usize buf_index = 20;

    for (; value > 0; value /= 10) {
        buf_index -= 1;
        buf_mem[buf_index] = (u8)((value % 10) + '0');
    }

    String decimal = { .data = buf_mem + buf_index, .count = (20 - buf_index) };
    string_builder_push_string(builder, decimal);
}

static void string_builder_push_char(String_Builder *builder, u8 c) {
    array_push(builder, &c);
}

static void string_builder_push_string(String_Builder *builder, String str) {
    array_push_slice(builder, &str);
}
