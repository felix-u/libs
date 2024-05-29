// link: gdi32
// link: user32
// include: windows.h
// include: windowsx.h

structdef(Gui) {
    char *name; 
    int width, height, scale, fps;
    u32 *pixels, *screen_buffer;

    f32 t_delta, t_elapsed;
    int last_mouse_x, mouse_x; 
    int last_mouse_y, mouse_y;
    bool last_mouse_left_down, mouse_left_down;
    bool last_mouse_right_down, mouse_right_down;
    bool key_down[256];

    HWND handle;
    LARGE_INTEGER ticks_per_second;
    LARGE_INTEGER last_tick_count, tick_count;
};

structdef(Gui_V2) { int x, y; };
static force_inline Gui_V2 gui_v2_scale(Gui_V2 v, int s) { return (Gui_V2){ v.x * s, v.y * s }; }

uniondef(Gui_Rect) {  
    struct { int x, y, width, height; };
    struct { Gui_V2 pos, dim; };
};

static Gui g_gui;

static u32 *g_palette;
#define gui_set_palette(palette) g_palette = palette

static Gui  *gui_create(Gui gui);
static void  gui_update_delta_time(void);
static void  gui_upscale_pixels_to_buf_(void);
static bool  gui_render(void);
static bool  gui_bounds_check(Gui_Rect *rect);
static void  gui_draw_rect_no_bounds_check(Gui_Rect rect, u32 colour);
static void  gui_draw_rect(Gui_Rect rect, u32 colour);
static void  gui_draw_rect_palette(Gui_Rect rect, usize palette_i);
static void  gui_draw_sprite_no_bounds_check(u32 *sprite, Gui_V2 dim, Gui_Rect area);
static void  gui_draw_sprite_palette_no_bounds_check(u32 *sprite, Gui_V2 dim, Gui_Rect area);
static void  gui_draw_sprite(u32 *sprite, Gui_V2 dim, Gui_Rect area);
static void  gui_draw_sprite_palette(u32 *sprite, Gui_V2 dim, Gui_Rect area);
static void  gui_draw_ascii_3x5(String8 ascii, Gui_V2 pos, u32 colour);
static void  gui_draw_ascii_3x5_palette(String8 ascii, Gui_V2 pos, u32 palette_i);
static void  gui_draw_ascii_char_3x5(u8 c, Gui_V2 pos, u32 colour);
static void  gui_draw_ascii_char_3x5_no_bounds_check(u8 c, Gui_Rect area, u32 colour);
static void  gui_clear(void);
static void  gui_clear_colour(u32 colour);
static void  gui_clear_palette(usize palette_i);
static bool  gui_is_point_in_rect(Gui_V2 point, Gui_Rect rect);

#define gui_draw_ascii_3x5_loop(action) {\
    int left = pos.x;\
    Gui_V2 pos_add = {0};\
    for (usize i = 0; i < ascii.len; i += 1, pos.x += pos_add.x, pos.y += pos_add.y) {\
        pos_add = (Gui_V2){ .x = 4 };\
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

#define gui_draw_no_bounds_check_loop(action) {\
    int x_offset = dim.x - area.width;\
    int y_offset = dim.y - area.height;\
    int sprite_row_i = (y_offset * dim.x) + x_offset;\
    int win_row_i = area.y * g_gui.width + area.x;\
    for (int y_i = 0; y_i < area.height;) {\
        for (int x_i = 0; x_i < area.width; x_i += 1) {\
            int sprite_i = sprite_row_i + x_i;\
            if (sprite[sprite_i] == 0) continue;\
            int win_i = win_row_i + x_i; \
            action;\
        }\
        y_i += 1;\
        win_row_i += g_gui.width;\
        sprite_row_i += dim.x;\
    }\
}

static bool gui_font_3x5[128][15] = {
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
