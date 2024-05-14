structdef(UI_V2) { f32 x, y; };

structdef(UI_Mouse) { 
    UI_V2 pos; 
    bool left_clicked, left_down;
    bool right_clicked, right_down;
    UI_V2 delta_wheel;
};

structdef(UI_Rect) { UI_V2 pos, dim; };

structdef(UI_Conf) {
    void *data;
    f32 (*measure_text)(void *data, char *text, f32 size);
    void (*draw_text)(void *data, char *text, UI_V2 pos, f32 size, u32 colour);
    void (*draw_rect)(void *data, UI_Rect rect, u32 colour);
    UI_Mouse (*get_mouse)(void *data);
};

structdef(UI_Layout) {
    UI_V2 pos;
    f32 font_size;
    f32 height;
    f32 margin;
};

static bool  ui_button(char *text);
static UI_V2 ui_next_pos(f32 width);
static bool  ui_point_is_in_rect(UI_V2 pos, UI_Rect rect);
static void  ui_set_layout(UI_Layout *layout);
static void  ui_set_conf(UI_Conf *conf);
static void  ui_textbox(char *text);
