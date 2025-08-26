#if defined(BASE_NO_IMPLEMENTATION) || defined(BASE_NO_IMPLEMENTATION_GFX)

#define gfx_max_font_width 8
#define gfx_max_font_height 16
structdef(Gfx_Font) {
    u8 width, height;
    u8 glyphs[gfx_max_font_width * gfx_max_font_height][128]; // NOTE(felix): worth looking into whether we want to support > 128. IBM codepage 437 might be cool!
};

structdef(Gfx_Frame_Info) {
    V2 virtual_window_size;
    V2 mouse_position;
    bool mouse_down[SAPP_MAX_MOUSEBUTTONS];
    bool mouse_up[SAPP_MAX_MOUSEBUTTONS];
    bool mouse_down_previously[SAPP_MAX_MOUSEBUTTONS];
    bool mouse_up_previously[SAPP_MAX_MOUSEBUTTONS];
    bool key_down[SAPP_MAX_KEYCODES];
    bool key_up[SAPP_MAX_KEYCODES];
    bool key_down_previously[SAPP_MAX_KEYCODES];
    bool key_up_previously[SAPP_MAX_KEYCODES];
};

structdef(Gfx_Pixel_Buffer) {
    u32 *pixels;
    V2 size;
};

#define gfx_key_pressed(frame_info, key) ((frame_info).key_down[key] && !(frame_info).key_down_previously[key])
#define gfx_mouse_clicked(frame_info, button) ((frame_info).mouse_down[button] && !(frame_info).mouse_down_previously[button])

static               void gfx_pixel_buffer_clear(Gfx_Pixel_Buffer pixel_buffer, u32 rgb);
static               void gfx_pixel_buffer_draw_line(Gfx_Pixel_Buffer pixel_buffer, V2 start_position, V2 end_position, f32 thickness, u32 rgb);
static               void gfx_pixel_buffer_draw_rectangle(Gfx_Pixel_Buffer pixel_buffer, i32 top, i32 left, i32 width, i32 height, u32 rgb);
static               void gfx_pixel_buffer_draw_text(Gfx_Pixel_Buffer pixel_buffer, Gfx_Font font, String string, i32 x, i32 y, u32 rgb);
static inline        void gfx_pixel_buffer_set_pixel(Gfx_Pixel_Buffer pixel_buffer, i32 x, i32 y, u32 rgb);


#else // IMPLEMENTATION


static void gfx_pixel_buffer_clear(Gfx_Pixel_Buffer pixel_buffer, u32 rgb) {
    u32 width = (u32)pixel_buffer.size.x;
    u32 height = (u32)pixel_buffer.size.y;
    for (u32 y = 0; y < height; y += 1) {
        u32 *row = pixel_buffer.pixels + y * width;
        for (u32 x = 0; x < width; x += 1) {
            u32 *pixel = row + x;
            *pixel = rgb;
        }
    }
}

static void gfx_pixel_buffer_draw_line(Gfx_Pixel_Buffer pixel_buffer, V2 start, V2 end, f32 thickness, u32 rgb) {
    // Reference: https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

    // TODO(felix)
    assert(thickness == 1.f);

    i32 start_x = (i32)(start.x + 0.5f);
    i32 end_x = (i32)(end.x + 0.5f);

    i32 start_y = (i32)(start.y + 0.5f);
    i32 end_y = (i32)(end.y + 0.5f);

    bool is_steep = abs_i32(end_y - start_y) > abs_i32(end_x - start_x);
    if (is_steep) {
        swap(i32, &start_x, &start_y);
        swap(i32, &end_x, &end_y);
    }

    bool right_to_left = start_x > end_x;
    if (right_to_left) {
        swap(i32, &start_x, &end_x);
        swap(i32, &start_y, &end_y);
    }

    i32 distance_x = end_x - start_x;
    i32 distance_y = abs_i32(end_y - start_y);
    i32 error = distance_x / 2;
    i32 y_step = start_y < end_y ? 1 : -1;

    i32 width = (i32)pixel_buffer.size.x;
    i32 height = (i32)pixel_buffer.size.y;

    i32 y = start_y;
    for (i32 x = start_x; x < end_x; x += 1) {
        if (is_steep) {
            if (y < 0 || y >= width) continue;
            if (x < 0 || x >= height) continue;
            gfx_pixel_buffer_set_pixel(pixel_buffer, y, x, rgb);
        } else {
            if (x < 0 || x >= width) continue;
            if (y < 0 || y >= height) continue;
            gfx_pixel_buffer_set_pixel(pixel_buffer, x, y, rgb);
        }

        error -= distance_y;
        if (error < 0) {
            y += y_step;
            error += distance_x;
        }
    }
}

static inline void gfx_pixel_buffer_set_pixel(Gfx_Pixel_Buffer pixel_buffer, i32 x, i32 y, u32 rgb) {
    // TODO(felix): alpha blending (probably should have two set_pixel functions)

    i32 width = (i32)pixel_buffer.size.x;
    i32 height = (i32)pixel_buffer.size.y;

    assert(x >= 0);
    assert(x < width);

    assert(y >= 0);
    assert(y < height);

    pixel_buffer.pixels[y * width + x] = rgb;
}

static void gfx_pixel_buffer_draw_rectangle(Gfx_Pixel_Buffer pixel_buffer, i32 top, i32 left, i32 width, i32 height, u32 rgb) {
    i32 virtual_width = (i32)pixel_buffer.size.x;
    i32 virtual_height = (i32)pixel_buffer.size.y;

    i32 bottom = clamp(top + height, 0, virtual_height);
    i32 right = clamp(left + width, 0, virtual_width);
    top = clamp(top, 0, virtual_height - 1);
    left = clamp(left, 0, virtual_width - 1);

    // TODO(felix): can optimise by doing memcpy after first row
    for (i32 y = top; y < bottom; y += 1) {
        for (i32 x = left; x < right; x += 1) {
            gfx_pixel_buffer_set_pixel(pixel_buffer, x, y, rgb);
        }
    }
}

static void gfx_pixel_buffer_draw_text(Gfx_Pixel_Buffer pixel_buffer, Gfx_Font font, String string, i32 x, i32 y, u32 rgb) {
    i32 top = y;
    i32 left = x;

    // TODO(felix): make customisable
    i32 tracking = 1;
    i32 step = font.width + tracking;

    assert(string.count > 0);
    for (u64 i = 0; i < string.count; i += 1, left += step) {
        u8 character = string.data[i];
        assert(character < 128);

        u8 *glyph = font.glyphs[character];
        for (i32 font_y = 0; font_y < font.height; font_y += 1) {
            y = top + font_y;
            u8 *glyph_row = &glyph[font_y * font.width];

            for (i32 font_x = 0; font_x < font.width; font_x += 1) {
                u8 glyph_value = *(glyph_row + font_x);
                if (glyph_value == 0) continue;

                x = left + font_x;
                gfx_pixel_buffer_set_pixel(pixel_buffer, x, y, rgb);
            }
        }
    }
}

// here for posterity, I suppose
static Gfx_Font gfx_font_default_3x5 = {
    .width = 3, .height = 5,
    .glyphs = {
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
    },
};

#endif // defined(BASE_NO_IMPLEMENTATION) || defined(BASE_NO_IMPLEMENTATION_GFX)
