structdef(String8) { 
    u8 *ptr; 
    usize len; 
};

#define string8(s) (String){ .ptr = (u8 *)s, .len = sizeof(s) - 1 }
#define string_fmt(s) (int)(s).len, (s).ptr
#define cstring_printf(arena_ptr, fmt, ...)\
    (char *)(string8_printf(arena_ptr, fmt "\0", __VA_ARGS__).ptr)

const u8 decimal_from_hex_char_table[256] = {
    ['0'] = 0, ['1'] = 1, ['2'] = 2, ['3'] = 3, ['4'] = 4,
    ['5'] = 5, ['6'] = 6, ['7'] = 7, ['8'] = 8, ['9'] = 9,
    ['A'] = 10, ['B'] = 11, ['C'] = 12, ['D'] = 13, ['E'] = 14, ['F'] = 15,
    ['a'] = 10, ['b'] = 11, ['c'] = 12, ['d'] = 13, ['e'] = 14, ['f'] = 15,
};

static char   *cstring_from_string8(Arena *arena, String8 s); 

static bool    string8_eql(String8 s1, String8 s2); 
static String8 string8_from_cstring(char *s); 
static String8 string8_from_int_base(Arena *arena, usize _num, u8 base); 
static String8 string8_printf(Arena *arena, char *fmt, ...);
static String8 string8_range(String8 s, usize beg, usize end); 

static usize decimal_from_hex_string8(String8 s); 
