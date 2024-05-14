static UI_Conf *g_ui_conf;
static UI_Layout *g_ui_layout;

#define ui_vcall(fn) g_ui_conf->fn(g_ui_conf->data)
#define ui_vcallv(fn, ...) g_ui_conf->fn(g_ui_conf->data, __VA_ARGS__)

static u32 ui_white = 0xffffffff;
static u32 ui_black = 0x000000ff;

static bool ui_button(char *text) {
    f32 width = ui_vcallv(measure_text, text, g_ui_layout->font_size) + 
        g_ui_layout->margin * 2.f;
    UI_V2 pos = ui_next_pos(width);

    UI_Rect rect = {
        .pos = pos, .dim = (UI_V2){ width, g_ui_layout->height },
    };
    ui_vcallv(draw_rect, rect, ui_black);

    f32 border = rect.dim.x / 10.f;
    rect.pos.x += border; rect.pos.y += border;
    rect.dim.x -= border * 2.f; rect.dim.y -= border * 2.f;
    ui_vcallv(draw_rect, rect, ui_white);

    UI_V2 text_pos = pos;
    text_pos.x += g_ui_layout->margin;
    ui_vcallv(draw_text, text, text_pos, g_ui_layout->font_size, ui_black);

    UI_Mouse mouse = ui_vcall(get_mouse);
    return ui_point_is_in_rect(mouse.pos, rect) && mouse.left_clicked;
}

static UI_V2 ui_next_pos(f32 width) {
    g_ui_layout->pos.x += g_ui_layout->margin;
    UI_V2 result = g_ui_layout->pos;
    g_ui_layout->pos.x += width;
    return result;
}

static bool ui_point_is_in_rect(UI_V2 pos, UI_Rect rect) {
    return rect.pos.x <= pos.x && pos.x <= rect.pos.x + rect.dim.x &&
           rect.pos.y <= pos.y && pos.y <= rect.pos.y + rect.dim.y;
}

static void ui_set_layout(UI_Layout *layout) { g_ui_layout = layout; }

static void ui_set_conf(UI_Conf *conf) { g_ui_conf = conf; }

static void ui_textbox(char *text) {
    f32 width = ui_vcallv(measure_text, text, g_ui_layout->font_size) + 
        g_ui_layout->margin * 2.f;
    UI_V2 pos = ui_next_pos(width);

    UI_Rect rect = {
        .pos = pos, .dim = (UI_V2){ width, g_ui_layout->height },
    };
    ui_vcallv(draw_rect, rect, ui_white);

    UI_V2 text_pos = pos;
    text_pos.x += g_ui_layout->margin;
    ui_vcallv(draw_text, text, text_pos, g_ui_layout->font_size, ui_black);

}
