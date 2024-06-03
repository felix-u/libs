structdef(Gfx) {
    char *name; 
    int width, height, scale, fps;
    u32 *pixels, *screen_buffer;

    f32 t_delta, t_elapsed;
    int last_mouse_x, mouse_x; 
    int last_mouse_y, mouse_y;
    bool last_mouse_left_down, mouse_left_down;
    bool last_mouse_right_down, mouse_right_down;
    bool key_down[256];

    #if OS_WINDOWS
        HWND handle;
        LARGE_INTEGER ticks_per_second;
        LARGE_INTEGER last_tick_count, tick_count;
    #endif // OS
};

structdef(Gfx_V2) { int x, y; };
static force_inline Gfx_V2 gfx_v2_scale(Gfx_V2 v, int s) { return (Gfx_V2){ v.x * s, v.y * s }; }

uniondef(Gfx_Rect) {  
    struct { int x, y, width, height; };
    struct { Gfx_V2 pos, dim; };
};

#define gfx_set_palette(palette) g_palette = palette

// platform-specific implementation
static              Gfx  *gfx_create(Gfx gfx);
static              void  gfx_get_input(void);
static              void  gfx_main_loop(void (*user_loop_fn)(void));
static              bool  gfx_render(void);
static force_inline u32   gfx_rgb(u32 rgb);
static              void  gfx_update_t_delta(void);

// common implementation
static bool  gfx_bounds_check(Gfx_Rect *rect);
static void  gfx_clear(void);
static void  gfx_clear_colour(u32 colour);
static void  gfx_clear_palette(usize palette_i);
static void  gfx_draw_ascii_3x5(String8 ascii, Gfx_V2 pos, u32 colour);
static void  gfx_draw_ascii_3x5_palette(String8 ascii, Gfx_V2 pos, u32 palette_i);
static void  gfx_draw_ascii_char_3x5(u8 c, Gfx_V2 pos, u32 colour);
static void  gfx_draw_ascii_char_3x5_no_bounds_check(u8 c, Gfx_Rect area, u32 colour);
static void  gfx_draw_rect(Gfx_Rect rect, u32 colour);
static void  gfx_draw_rect_no_bounds_check(Gfx_Rect rect, u32 colour);
static void  gfx_draw_rect_palette(Gfx_Rect rect, usize palette_i);
static void  gfx_draw_sprite(u32 *sprite, Gfx_V2 dim, Gfx_Rect area);
static void  gfx_draw_sprite_no_bounds_check(u32 *sprite, Gfx_V2 dim, Gfx_Rect area);
static void  gfx_draw_sprite_palette(u32 *sprite, Gfx_V2 dim, Gfx_Rect area);
static void  gfx_draw_sprite_palette_no_bounds_check(u32 *sprite, Gfx_V2 dim, Gfx_Rect area);
static bool  gfx_is_point_in_rect(Gfx_V2 point, Gfx_Rect rect);
static void  gfx_upscale_pixels_to_buf_(void);

#define gfx_draw_ascii_3x5_loop(action) {\
    int left = pos.x;\
    Gfx_V2 pos_add = {0};\
    for (usize i = 0; i < ascii.len; i += 1, pos.x += pos_add.x, pos.y += pos_add.y) {\
        pos_add = (Gfx_V2){ .x = 4 };\
        u8 c = ascii.ptr[i];\
        switch (c) {\
            case ' ': continue; break;\
            case '\t': pos_add.x = 16; continue; break;\
            case '\n': pos_add.y = 6; pos.x = left; continue; break;\
            default: break;\
        }\
        action;\
    }\
}

#define gfx_draw_no_bounds_check_loop(action) {\
    int x_offset = dim.x - area.width;\
    int y_offset = dim.y - area.height;\
    int sprite_row_i = (y_offset * dim.x) + x_offset;\
    int win_row_i = area.y * g_gfx.width + area.x;\
    for (int y_i = 0; y_i < area.height;) {\
        for (int x_i = 0; x_i < area.width; x_i += 1) {\
            int sprite_i = sprite_row_i + x_i;\
            if (sprite[sprite_i] == 0) continue;\
            int win_i = win_row_i + x_i; \
            action;\
        }\
        y_i += 1;\
        win_row_i += g_gfx.width;\
        sprite_row_i += dim.x;\
    }\
}

static bool gfx_font_3x5[128][15] = {
    [' '] = {
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
    },
    ['!'] = {
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,
        0, 0, 0,
        0, 1, 0,
    },
    ['"'] = {
        1, 0, 1,
        1, 0, 1,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
    },
    ['#'] = {
        1, 0, 1,
        1, 1, 1,
        1, 0, 1,
        1, 1, 1,
        1, 0, 1,
    },
    ['$'] = {
        0, 1, 0,
        0, 1, 1,
        0, 1, 0,
        1, 1, 0,
        0, 1, 0,
    },
    ['%'] = {
        1, 0, 1,
        0, 0, 1,
        0, 1, 0,
        1, 0, 0,
        1, 0, 1,
    },
    ['&'] = {
        0, 1, 0,
        1, 0, 1,
        1, 1, 0,
        1, 0, 1,
        0, 1, 1,
    },
    ['\''] = {
        0, 1, 0,
        0, 1, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
    },
    ['('] = {
        0, 1, 0,
        1, 0, 0,
        1, 0, 0,
        1, 0, 0,
        0, 1, 0,
    },
    [')'] = {
        0, 1, 0,
        0, 0, 1,
        0, 0, 1,
        0, 0, 1,
        0, 1, 0,
    },
    ['*'] = {
        0, 0, 0,
        1, 0, 1,
        0, 1, 0,
        1, 0, 1,
        0, 0, 0,
    },
    ['+'] = {
        0, 0, 0,
        0, 1, 0,
        1, 1, 1,
        0, 1, 0,
        0, 0, 0,
    },
    [','] = {
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 1, 0,
        1, 0, 0,
    },
    ['-'] = {
        0, 0, 0,
        0, 0, 0,
        1, 1, 1,
        0, 0, 0,
        0, 0, 0,
    },
    ['.'] = {
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 1, 0,
    },
    ['/'] = {
        0, 0, 1,
        0, 0, 1,
        0, 1, 0,
        1, 0, 0,
        1, 0, 0,
    },
    ['0'] = {
        1, 1, 1,
        1, 0, 1,
        1, 0, 1,
        1, 0, 1,
        1, 1, 1,
    },
    ['1'] = {
        0, 1, 0,
        1, 1, 0,
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,
    },
    ['2'] = {
        1, 1, 1,
        1, 0, 1,
        0, 0, 1,
        1, 1, 0,
        1, 1, 1,
    },
    ['3'] = {
        1, 1, 1,
        0, 0, 1,
        1, 1, 1,
        0, 0, 1,
        1, 1, 1,
    },
    ['4'] = {
        1, 0, 1,
        1, 0, 1,
        1, 1, 1,
        0, 0, 1,
        0, 0, 1,
    },
    ['5'] = {
        1, 1, 1,
        1, 0, 0,
        1, 1, 0,
        0, 0, 1,
        1, 1, 0,
    },
    ['6'] = {
        1, 1, 1,
        1, 0, 0,
        1, 1, 1,
        1, 0, 1,
        1, 1, 1,
    },
    ['7'] = {
        1, 1, 1,
        0, 0, 1,
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,
    },
    ['8'] = {
        1, 1, 1,
        1, 0, 1,
        1, 1, 1,
        1, 0, 1,
        1, 1, 1,
    },
    ['9'] = {
        1, 1, 1,
        1, 0, 1,
        1, 1, 1,
        0, 0, 1,
        1, 1, 1,
    },
    [':'] = {
        0, 0, 0,
        0, 0, 0,
        0, 1, 0,
        0, 0, 0,
        0, 1, 0,
    },
    [';'] = {
        0, 0, 0,
        0, 1, 0,
        0, 0, 0,
        0, 1, 0,
        1, 0, 0,
    },
    ['<'] = {
        0, 0, 1,
        0, 1, 0,
        1, 0, 0,
        0, 1, 0,
        0, 0, 1,
    },
    ['='] = {
        0, 0, 0,
        1, 1, 1,
        0, 0, 0,
        1, 1, 1,
        0, 0, 0,
    },
    ['>'] = {
        1, 0, 0,
        0, 1, 0,
        0, 0, 1,
        0, 1, 0,
        1, 0, 0,
    },
    ['?'] = {
        1, 1, 0,
        0, 0, 1,
        0, 1, 0,
        0, 0, 0,
        0, 1, 0,
    },
    ['@'] = {
        1, 1, 1,
        1, 0, 1,
        1, 0, 1,
        1, 0, 0,
        1, 1, 1,
    },
    ['A'] = {
        0, 1, 0,
        1, 0, 1,
        1, 1, 1,
        1, 0, 1,
        1, 0, 1,
    },
    ['B'] = {
        1, 1, 0,
        1, 0, 1,
        1, 1, 1,
        1, 0, 1,
        1, 1, 0,
    },
    ['C'] = {
        0, 1, 1,
        1, 0, 0,
        1, 0, 0,
        1, 0, 0,
        0, 1, 1,
    },
    ['D'] = {
        1, 1, 0,
        1, 0, 1,
        1, 0, 1,
        1, 0, 1,
        1, 1, 0,
    },
    ['E'] = {
        1, 1, 1,
        1, 0, 0,
        1, 1, 1,
        1, 0, 0,
        1, 1, 1,
    },
    ['F'] = {
        1, 1, 1,
        1, 0, 0,
        1, 1, 1,
        1, 0, 0,
        1, 0, 0,
    },
    ['G'] = {
        0, 1, 1,
        1, 0, 0,
        1, 0, 1,
        1, 0, 1,
        0, 1, 0,
    },
    ['H'] = {
        1, 0, 1,
        1, 0, 1,
        1, 1, 1,
        1, 0, 1,
        1, 0, 1,
    },
    ['I'] = {
        1, 1, 1,
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,
        1, 1, 1,
    },
    ['J'] = {
        1, 1, 1,
        0, 0, 1,
        1, 0, 1,
        1, 0, 1,
        0, 1, 0,
    },
    ['K'] = {
        1, 0, 1,
        1, 0, 1,
        1, 1, 0,
        1, 0, 1,
        1, 0, 1,
    },
    ['L'] = {
        1, 0, 0,
        1, 0, 0,
        1, 0, 0,
        1, 0, 0,
        1, 1, 1,
    },
    ['M'] = {
        1, 0, 1,
        1, 1, 1,
        1, 0, 1,
        1, 0, 1,
        1, 0, 1,
    },
    ['N'] = {
        1, 0, 1,
        1, 1, 1,
        1, 1, 1,
        1, 0, 1,
        1, 0, 1,
    },
    ['O'] = {
        0, 1, 0,
        1, 0, 1,
        1, 0, 1,
        1, 0, 1,
        0, 1, 0,
    },
    ['P'] = {
        1, 1, 0,
        1, 0, 1,
        1, 1, 0,
        1, 0, 0,
        1, 0, 0,
    },
    ['Q'] = {
        0, 1, 0,
        1, 0, 1,
        1, 0, 1,
        0, 1, 0,
        0, 0, 1,
    },
    ['R'] = {
        1, 1, 0,
        1, 0, 1,
        1, 1, 0,
        1, 0, 1,
        1, 0, 1,
    },
    ['S'] = {
        0, 1, 1,
        1, 0, 0,
        0, 1, 0,
        0, 0, 1,
        1, 1, 0,
    },
    ['T'] = {
        1, 1, 1,
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,
    },
    ['U'] = {
        1, 0, 1,
        1, 0, 1,
        1, 0, 1,
        1, 0, 1,
        1, 1, 1,
    },
    ['V'] = {
        1, 0, 1,
        1, 0, 1,
        1, 0, 1,
        1, 0, 1,
        0, 1, 0,
    },
    ['W'] = {
        1, 0, 1,
        1, 0, 1,
        1, 0, 1,
        1, 1, 1,
        1, 0, 1,
    },
    ['X'] = {
        1, 0, 1,
        1, 0, 1,
        0, 1, 0,
        1, 0, 1,
        1, 0, 1,
    },
    ['Y'] = {
        1, 0, 1,
        1, 0, 1,
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,
    },
    ['Z'] = {
        1, 1, 1,
        0, 0, 1,
        0, 1, 0,
        1, 0, 0,
        1, 1, 1,
    },
    ['['] = {
        1, 1, 0,
        1, 0, 0,
        1, 0, 0,
        1, 0, 0,
        1, 1, 0,
    },
    ['\\'] = {
        1, 0, 0,
        1, 0, 0,
        0, 1, 0,
        0, 0, 1,
        0, 0, 1,
    },
    [']'] = {
        0, 1, 1,
        0, 0, 1,
        0, 0, 1,
        0, 0, 1,
        0, 1, 1,
    },
    ['^'] = {
        0, 1, 0,
        1, 0, 1,
        1, 0, 1,
        0, 0, 0,
        0, 0, 0,
    },
    ['_'] = {
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        1, 1, 1,
    },
    ['`'] = {
        1, 0, 0,
        0, 1, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
    },
    ['a'] = {
        0, 0, 0,
        0, 1, 1,
        1, 0, 1,
        1, 0, 1,
        1, 1, 1,
    },
    ['b'] = {
        1, 0, 0,
        1, 1, 1,
        1, 0, 1,
        1, 0, 1,
        1, 1, 1,
    },
    ['c'] = {
        0, 0, 0,
        1, 1, 1,
        1, 0, 0,
        1, 0, 0,
        1, 1, 1,
    },
    ['d'] = {
        0, 0, 1,
        1, 1, 1,
        1, 0, 1,
        1, 0, 1,
        1, 1, 1,
    },
    ['e'] = {
        0, 0, 0,
        1, 1, 1,
        1, 1, 1,
        1, 0, 0,
        1, 1, 1,
    },
    ['f'] = {
        0, 1, 1,
        0, 1, 0,
        1, 1, 1,
        0, 1, 0,
        0, 1, 0,
    },
    ['g'] = {
        0, 0, 0,
        1, 1, 1,
        1, 1, 1,
        0, 0, 1,
        1, 1, 1,
    },
    ['h'] = {
        1, 0, 0,
        1, 1, 1,
        1, 0, 1,
        1, 0, 1,
        1, 0, 1,
    },
    ['i'] = {
        0, 0, 0,
        1, 1, 0,
        0, 1, 0,
        0, 1, 0,
        1, 1, 1,
    },
    ['j'] = {
        0, 0, 0,
        0, 1, 1,
        0, 0, 1,
        1, 0, 1,
        0, 1, 1,
    },
    ['k'] = {
        1, 0, 0,
        1, 0, 1,
        1, 1, 0,
        1, 1, 0,
        1, 0, 1,
    },
    ['l'] = {
        1, 1, 0,
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,
        0, 1, 1,
    },
    ['m'] = {
        0, 0, 0,
        1, 0, 1,
        1, 1, 1,
        1, 0, 1,
        1, 0, 1,
    },
    ['n'] = {
        0, 0, 0,
        1, 1, 0,
        1, 0, 1,
        1, 0, 1,
        1, 0, 1,
    },
    ['o'] = {
        0, 0, 0,
        1, 1, 1,
        1, 0, 1,
        1, 0, 1,
        1, 1, 1,
    },
    ['p'] = {
        0, 0, 0,
        1, 1, 1,
        1, 0, 1,
        1, 1, 1,
        1, 0, 0,
    },
    ['q'] = {
        0, 0, 0,
        1, 1, 1,
        1, 0, 1,
        1, 1, 1,
        0, 0, 1,
    },
    ['r'] = {
        0, 0, 0,
        1, 1, 0,
        1, 0, 1,
        1, 0, 0,
        1, 0, 0,
    },
    ['s'] = {
        0, 0, 0,
        0, 1, 1,
        1, 1, 0,
        0, 0, 1,
        1, 1, 0,
    },
    ['t'] = {
        0, 1, 0,
        1, 1, 1,
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,
    },
    ['u'] = {
        0, 0, 0,
        1, 0, 1,
        1, 0, 1,
        1, 0, 1,
        1, 1, 1,
    },
    ['v'] = {
        0, 0, 0,
        1, 0, 1,
        1, 0, 1,
        1, 0, 1,
        0, 1, 0,
    },
    ['w'] = {
        0, 0, 0,
        1, 0, 1,
        1, 0, 1,
        1, 1, 1,
        1, 0, 1,
    },
    ['x'] = {
        0, 0, 0,
        1, 0, 1,
        0, 1, 0,
        0, 1, 0,
        1, 0, 1,
    },
    ['y'] = {
        0, 0, 0,
        1, 0, 1,
        1, 1, 1,
        0, 0, 1,
        1, 1, 0,
    },
    ['z'] = {
        0, 0, 0,
        1, 1, 1,
        0, 1, 1,
        1, 0, 0,
        1, 1, 1,
    },
    ['{'] = {
        0, 1, 1,
        0, 1, 0,
        1, 0, 0,
        0, 1, 0,
        0, 1, 1,
    },
    ['|'] = {
        0, 1, 0,
        0, 1, 0,
        0, 0, 0,
        0, 1, 0,
        0, 1, 0,
    },
    ['}'] = {
        1, 1, 0,
        0, 1, 0,
        0, 0, 1,
        0, 1, 0,
        1, 1, 0,
    },
    ['~'] = {
        0, 0, 0,
        1, 0, 0,
        1, 1, 1,
        0, 0, 1,
        0, 0, 0,
    },
};
