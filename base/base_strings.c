static bool string8_eql(String8 s1, String8 s2) {
    if (s1.len != s2.len) return false;
    for (usize i = 0; i < s1.len; i += 1) {
        if (s1.ptr[i] != s2.ptr[i]) return false;
    }
    return true;
}

static String8 string8_from_cstring(char *s) {
    if (s == NULL) return (String8){ 0 };
    usize len = 0;
    while (s[len] != '\0') len += 1;
    return (String8){ .ptr = (u8 *)s, .len = len };
}

static char *cstring_from_string8(Arena *arena, String8 s) {
    char *cstring = arena_alloc(arena, s.len + 1, sizeof(char));
    for (usize i = 0; i < s.len; i += 1) cstring[i] = s.ptr[i];
    cstring[s.len] = '\0';
    return cstring;
}

// Only bases <= 10
static String8 string8_from_int_base(Arena *arena, usize _num, u8 base) {
    String8 str = {0};
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

static String8 string8_range(String8 s, usize beg, usize end) {
    return (String8){ .ptr = s.ptr + beg, .len = end - beg };
}

static usize decimal_from_hex_string8(String8 s) {
    usize result = 0, magnitude = s.len;
    for (usize i = 0; i < s.len; i += 1, magnitude -= 1) {
        usize hex_digit = decimal_from_hex_char_table[s.ptr[i]];
        for (usize j = 1; j < magnitude; j += 1) hex_digit *= 16;
        result += hex_digit;
    }
    return result;
}
