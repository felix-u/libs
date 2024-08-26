static bool str8_eql(Str8 s1, Str8 s2) {
    if (s1.len != s2.len) return false;
    return memcmp(s1.ptr, s2.ptr, s1.len) == 0;
}

static Str8 str8_from_cstr(char *s) {
    if (s == NULL) return (Str8){ 0 };
    usize len = 0;
    while (s[len] != '\0') len += 1;
    return (Str8){ .ptr = (u8 *)s, .len = len };
}

static char *cstr_from_str8(Arena *arena, Str8 s) {
    char *cstr = arena_alloc(arena, s.len + 1, sizeof(char));
    for (usize i = 0; i < s.len; i += 1) cstr[i] = s.ptr[i];
    cstr[s.len] = '\0';
    return cstr;
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

static Str8 str8_printf(Arena *arena, usize alloc_size, char *fmt, ...) {
    printf("ok\n");
    assume(alloc_size > 1);

    va_list args;
    va_start(args, fmt);

    char *buf = arena_alloc(arena, alloc_size, sizeof(char));
    int printed_len = vsnprintf(buf, alloc_size - 1, fmt, args); 

    va_end(args);

    buf[printed_len] = 0;
    return (Str8){
        .ptr = (u8 *)buf,
        .len = printed_len,
    };
}

static Str8 str8_range(Str8 s, usize beg, usize end) {
    return (Str8){ .ptr = s.ptr + beg, .len = end - beg };
}

static usize decimal_from_hex_str8(Str8 s) {
    usize result = 0, magnitude = s.len;
    for (usize i = 0; i < s.len; i += 1, magnitude -= 1) {
        usize hex_digit = decimal_from_hex_digit_table[s.ptr[i]];
        for (usize j = 1; j < magnitude; j += 1) hex_digit *= 16;
        result += hex_digit;
    }
    return result;
}
