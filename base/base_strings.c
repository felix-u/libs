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

static Str8 str8_printf(Arena *arena, usize alloc_size, char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    Str8 result = str8_vprintf(arena, alloc_size, fmt, args);
    va_end(args);
    return result;
}

static Str8 str8_range(Str8 s, usize beg, usize end) {
    return (Str8){ .ptr = s.ptr + beg, .len = end - beg };
}

static Str8 str8_vprintf(Arena *arena, usize alloc_size, char *fmt, va_list args) {
    assume(alloc_size > 1);
    char *buf = arena_alloc(arena, alloc_size, sizeof(char));
    int printed_len = vsnprintf(buf, alloc_size - 1, fmt, args);
    buf[printed_len] = 0;
    return (Str8){
        .ptr = (u8 *)buf,
        .len = printed_len,
    };
}

static Str16 str16_from_str8(Arena *arena, Str8 s) {
    #if OS_WINDOWS
        Str16 wstr = {
            .ptr = arena_alloc(arena, s.len, sizeof(u16)),
            .len = MultiByteToWideChar(CP_UTF8, 0, (char *)s.ptr, (int)s.len, wstr.ptr, (int)s.len),
        };
        win32_assert_not_0(wstr.len);
        return wstr;
    #else
        panic("unimplemented");
    #endif
}
