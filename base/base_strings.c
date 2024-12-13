static usize decimal_from_hex_str8(Str8 s) {
    usize result = 0, magnitude = s.len;
    for (usize i = 0; i < s.len; i += 1, magnitude -= 1) {
        usize hex_digit = decimal_from_hex_digit_table[s.ptr[i]];
        for (usize j = 1; j < magnitude; j += 1) hex_digit *= 16;
        result += hex_digit;
    }
    return result;
}

static bool str8_eql(Str8 s1, Str8 s2) {
    if (s1.len != s2.len) return false;
    return memcmp(s1.ptr, s2.ptr, s1.len) == 0;
}

static Str8 str8_from_strc(char *s) {
    if (s == NULL) return (Str8){ 0 };
    usize len = 0;
    while (s[len] != '\0') len += 1;
    return (Str8){ .ptr = (u8 *)s, .len = len };
}

static char *strc_from_str8(Arena *arena, Str8 s) {
    char *strc = arena_alloc(arena, s.len + 1, sizeof(char));
    for (usize i = 0; i < s.len; i += 1) strc[i] = s.ptr[i];
    strc[s.len] = '\0';
    return strc;
}

// Only bases <= 10
static Str8 str8_from_int_base(Arena *arena, usize _num, u8 base) {
    Str8 str = {0};
    usize num = _num;

    do {
        num /= base;
        str.len += 1;
    } while (num > 0);

    str.ptr = arena_alloc(arena, str.len, sizeof(u8));

    num = _num;
    for (i64 i = str.len - 1; i >= 0; i -= 1) {
        str.ptr[i] = (num % base) + '0';
        num /= base;
    }

    return str;
}

static Str8 str8_printf(Arena *arena, char *format, ...) {
    va_list args;
    va_start(args, format);
    Str8 result = str8_vprintf(arena, format, args);
    va_end(args);
    return result;
}

static Str8 str8_range(Str8 str, usize beg, usize end) {
    assert(end <= str.len);
    assert(beg < end);
    return (Str8){ .ptr = str.ptr + beg, .len = end - beg };
}

static Str8 str8_vprintf(Arena *arena, char *fmt, va_list args) {
    Str8_Builder builder = { .arena = arena };
    str8_builder_printf_var_args(&builder, fmt, args);
    return str8_from_str8_builder(builder);
}

static Str16 str16_from_str8(Arena *arena, Str8 s) {
    Str16 wstr = {
        .ptr = arena_alloc(arena, s.len, sizeof(u16)),
        .len = MultiByteToWideChar(CP_UTF8, 0, (char *)s.ptr, (int)s.len, wstr.ptr, (int)s.len),
    };
    win32_assert_not_0(wstr.len);
    return wstr;
}

static void str8_builder_null_terminate(Str8_Builder *builder) {
    u8 null_terminator = 0;
    array_push(builder->arena, builder, &null_terminator);
}

static void str8_builder_printf(Str8_Builder *builder, char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    str8_builder_printf_var_args(builder, fmt, args);
    va_end(args);
}

static void str8_builder_printf_var_args(Str8_Builder *builder, char *fmt_c, va_list args) {
    Str8 fmt_str = str8_from_strc(fmt_c);
    assert(fmt_str.len > 0);

    bool found_at_least_one_format_specifier = false;
    for (usize i = 0; i < fmt_str.len; i += 1) {
        usize beg_i = i;
        while (fmt_str.ptr[i] != '%' && i < fmt_str.len) i += 1;

        if (i == fmt_str.len) {
            if (!found_at_least_one_format_specifier) panicf("no indicator '%%' in format string '%'", fmt(Str8, fmt_str));
            str8_builder_push_str8(builder, str8_range(fmt_str, beg_i, i));
            return;
        }

        assert(fmt_str.ptr[i] == '%');

        if (i > beg_i) str8_builder_push_str8(builder, str8_range(fmt_str, beg_i, i));

        if (i + 1 < fmt_str.len && fmt_str.ptr[i + 1] == '%') {
            str8_builder_push_char(builder, '%');
            i += 1;
            continue;
        }

        Format format = va_arg(args, Format);
        assert(format.magic == fmt_magic_number);
        switch (format.type) {
            case format_type_char: str8_builder_push_char(builder, format.value_char); break;
            case format_type_u64: str8_builder_push_u64(builder, format.value_u64); break;
            case format_type_f64: str8_builder_push_f64(builder, format.value_f64); break;
            case format_type_cstring: str8_builder_push_str8(builder, str8_from_strc(format.value_cstring)); break;
            case format_type_Str8: str8_builder_push_str8(builder, format.value_Str8); break;
            default: unreachable;
        }
        found_at_least_one_format_specifier = true;
    }
}

static void str8_builder_push_f64(Str8_Builder *builder, f64 value) {
    // References:
    // https://dl.acm.org/doi/pdf/10.1145/3360595
    // https://en.wikipedia.org/wiki/Double-precision_floating-point_format
    bool is_ryu_implemented = false;

    if (is_ryu_implemented) {
        u64 bits = *(u64 *)&value;
        i64 exponent_biased = (bits >> 52) & 0x7ff;
        u64 mantissa = bits & 0x000fffffffffffff;

        b8 is_negative = bits >> 63;
        if (is_negative) str8_builder_push_char(builder, '-');

        if (exponent_biased == 0x7ff) {
            Str8 string = (mantissa == 0) ? str8("Infinity") : str8("NaN");
            str8_builder_push_str8(builder, string);
            return;
        }

        i64 exponent;
        if (exponent_biased == 0) {
            if (mantissa == 0) {
                if (is_negative) str8_builder_push_char(builder, '-');
                str8_builder_push_char(builder, '0');
                return;
            }
            mantissa |= (u64)1 << 52;
            exponent = -1022;
        } else exponent = exponent_biased - 1023;

        // TODO(felix): rest of ryu algorithm
    } else {
        // placeholder
        u8 buf[128] = {0};
        Str8 printed = { .ptr = buf, .len = snprintf((char *)buf, sizeof(buf), "%.2f", value) };
        str8_builder_push_str8(builder, printed);
    }
}

static void str8_builder_push_u64(Str8_Builder *builder, u64 value) {
    if (value == 0) {
        str8_builder_push_char(builder, '0');
        return;
    }

    // UINT64_MAX = 18,446,744,073,709,551,615
    // has 20 digits
    u8 buf_mem[20];
    usize buf_index = 20;

    for (; value > 0; value /= 10) {
        buf_index -= 1;
        buf_mem[buf_index] = (value % 10) + '0';
    }

    Str8 decimal = { .ptr = buf_mem + buf_index, .len = (20 - buf_index) };
    str8_builder_push_str8(builder, decimal);
}

static void str8_builder_push_char(Str8_Builder *builder, u8 c) {
    // TODO(felix): might be worth looking into array_push and making a fast path for single-item pushes
    array_push(builder->arena, builder, &c);
}

static void str8_builder_push_str8(Str8_Builder *builder, Str8 str) {
    array_push_slice(builder->arena, builder, &str);
}

static Str8 str8_from_str8_builder(Str8_Builder builder) {
    return (Str8)slice_from_array(builder);
}
