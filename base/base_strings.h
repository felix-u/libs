structdef(Str8) { u8 *ptr; usize len; };

#define str8c(s)  { .ptr = (u8 *)s, .len = sizeof(s) - 1 }
#define str8(s) (Str8){ .ptr = (u8 *)s, .len = sizeof(s) - 1 }
#define str_fmt(s) (int)(s).len, (s).ptr
#define cstr_printf(arena_ptr, fmt, ...)\
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

static char *cstr_from_str8(Arena *arena, Str8 s); 

static bool str8_eql(Str8 s1, Str8 s2); 
static Str8 str8_from_cstring(char *s); 
static Str8 str8_from_int_base(Arena *arena, usize _num, u8 base); 
static Str8 str8_printf(Arena *arena, usize alloc_size, char *fmt, ...);
static Str8 str8_range(Str8 s, usize beg, usize end); 

static usize decimal_from_hex_str8(Str8 s); 
