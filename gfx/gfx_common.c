static bool gfx_bounds_check(Gfx_Rect *rect) {
    int *x = &rect->x, *y = &rect->y; 
    int *width = &rect->width, *height = &rect->height;

    if (*x > g_gfx.width || *x + *width < 0 || *y > g_gfx.height || *y + *height < 0) return false;

    if (*x < 0) {
        *width += *x;
        *x = 0;
    } else {
        int width_diff = *x + *width - g_gfx.width;
        if (width_diff > 0) *width -= width_diff;
    }

    if (*y < 0) {
        *height += *y;
        *y = 0;
    } else {
        int height_diff = *y + *height - g_gfx.height;
        if (height_diff > 0) *height -= height_diff;
    }

    return true;
}

static void gfx_clear(void) {
    usize num_bytes = sizeof(u32) * g_gfx.width * g_gfx.height;
    memset(g_gfx.pixels, 0, num_bytes);
}

static void gfx_clear_colour(u32 colour) {
    for (int i = 0; i < g_gfx.width; i += 1) {
        g_gfx.pixels[i] = colour;
    }
    int max_row_i = g_gfx.width * g_gfx.height;
    int width_len = g_gfx.width * sizeof(u32);
    for (int row_i = g_gfx.width; row_i < max_row_i; row_i += g_gfx.width) {
        memcpy(g_gfx.pixels + row_i, g_gfx.pixels, width_len);
    }
}

static void gfx_clear_palette(usize palette_i) {
    gfx_clear_colour(g_palette[palette_i]);
}

static void gfx_draw_ascii_3x5(String8 ascii, Gfx_V2 pos, u32 colour) {
    gfx_draw_ascii_3x5_loop(gfx_draw_ascii_char_3x5(c, pos, colour));
}

static void gfx_draw_ascii_3x5_palette(String8 ascii, Gfx_V2 pos, u32 palette_i) {
    gfx_draw_ascii_3x5_loop(gfx_draw_ascii_char_3x5(c, pos, g_palette[palette_i]));
}

static void gfx_draw_ascii_char_3x5(u8 c, Gfx_V2 pos, u32 colour) {
    Gfx_Rect area = { .pos = pos, .dim = { 3, 5 } };
    if (!gfx_bounds_check(&area)) return;
    gfx_draw_ascii_char_3x5_no_bounds_check(c, area, colour);
}

static void gfx_draw_ascii_char_3x5_no_bounds_check(u8 c, Gfx_Rect area, u32 colour) {
    Gfx_V2 dim = { 3, 5 };
    bool *sprite = gfx_font_3x5[c];
    gfx_draw_no_bounds_check_loop(g_gfx.pixels[win_i] = colour);
}

static void gfx_draw_rect(Gfx_Rect rect, u32 colour) {
    if (!gfx_bounds_check(&rect)) return;
    gfx_draw_rect_no_bounds_check(rect, colour);
}

static void gfx_draw_rect_no_bounds_check(Gfx_Rect rect, u32 colour) {
    int width_len = sizeof(u32) * rect.width;
    int row_i = rect.y * g_gfx.width + rect.x;
    int max_row_i = (rect.y + rect.height) * g_gfx.width + rect.x;
    
    int max_i = row_i + rect.width;
    for (int i = row_i; i < max_i; i += 1) g_gfx.pixels[i] = colour;

    u32 *first_row = g_gfx.pixels + row_i;
    row_i += g_gfx.width;
    for (; row_i < max_row_i; row_i += g_gfx.width) {
        memcpy(g_gfx.pixels + row_i, first_row, width_len);
    }
}

static void gfx_draw_rect_palette(Gfx_Rect rect, usize palette_i) {
    if (!gfx_bounds_check(&rect)) return;
    gfx_draw_rect_no_bounds_check(rect, g_palette[palette_i]);
}

static void gfx_draw_sprite(u32 *sprite, Gfx_V2 dim, Gfx_Rect area) {
    if (!gfx_bounds_check(&area)) return;
    gfx_draw_sprite_no_bounds_check(sprite, dim, area);
}

static void gfx_draw_sprite_no_bounds_check(u32 *sprite, Gfx_V2 dim, Gfx_Rect area) {
    gfx_draw_no_bounds_check_loop(g_gfx.pixels[win_i] = sprite[sprite_i]);
}

static void gfx_draw_sprite_palette(u32 *sprite, Gfx_V2 dim, Gfx_Rect area) {
    if (!gfx_bounds_check(&area)) return;
    gfx_draw_sprite_palette_no_bounds_check(sprite, dim, area);
}

static void gfx_draw_sprite_palette_no_bounds_check(u32 *sprite, Gfx_V2 dim, Gfx_Rect area) {
    gfx_draw_no_bounds_check_loop(g_gfx.pixels[win_i] = g_palette[sprite[sprite_i]]);
}

static bool gfx_is_point_in_rect(Gfx_V2 point, Gfx_Rect rect) {
    if (point.x < rect.x || rect.x + rect.width  < point.x) return false;
    if (point.y < rect.y || rect.y + rect.height < point.y) return false;
    return true;
}

static void gfx_upscale_pixels_to_buf_(void) {
    int width = g_gfx.width;
    int height = g_gfx.height;
    int scale = g_gfx.scale;
    int screen_width = width * scale;
    int screen_width_len = sizeof(u32) * screen_width;
    int max_row_i = width * height;

    int screen_row_i = 0;
    for (int row_i = 0; row_i < max_row_i; row_i += width) {
        int max_i = row_i + width;

        int screen_i = screen_row_i;
        for (int i = row_i; i < max_i; i += 1, screen_i += scale) {
            for (int s = 0; s < scale; s += 1) {
                g_gfx.screen_buffer[screen_i + s] = g_gfx.pixels[i];
            }
        }
        
        u32 *first_screen_row = g_gfx.screen_buffer + screen_row_i;
        screen_row_i += screen_width;
        for (int s = 1; s < scale; s += 1, screen_row_i += screen_width) {
            memcpy(g_gfx.screen_buffer + screen_row_i, first_screen_row, screen_width_len);
        }
    }
}

