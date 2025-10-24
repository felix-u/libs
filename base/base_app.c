#if defined(BASE_NO_IMPLEMENTATION) || defined(BASE_NO_IMPLEMENTATION_APP)


enumdef(App_Key, u8) {
    App_Key_NIL = 0,
    // 32 -> 96 use ASCII values
    App_Key__ASCII_DELIMITER = 128,

    App_Key_CONTROL,

    App_Key_LEFT,
    App_Key_RIGHT,

    App_Key_MAX_VALUE,
};

enumdef(App_Mouse_Button, u8) {
    App_Mouse_Button_NIL = 0,
    App_Mouse_Button_LEFT,
    App_Mouse_Button_RIGHT,
    App_Mouse_Button_MIDDLE,
    App_Mouse_Button_MAX_VALUE,
};

structdef(App_Frame_Info) {
    V2 window_size;
    f32 dpi_scale;
    f32 seconds_since_last_frame;
    V2 mouse_position;
    bool mouse_clicked[App_Mouse_Button_MAX_VALUE];
    bool mouse_down[App_Mouse_Button_MAX_VALUE];
    bool key_down[App_Key_MAX_VALUE];
    bool key_pressed[App_Key_MAX_VALUE];
};

typedef enum Draw_Kind {
    Draw_Kind_NIL = 0,
    Draw_Kind_TEXT,
    Draw_Kind_RECTANGLE,
    Draw_Kind_QUADRILATERAL,
    Draw_Kind_MAX_VALUE,
} Draw_Kind;

typedef enum Draw_Color {
    Draw_Color_SOLID = 0,

    Draw_Color_TOP_LEFT = Draw_Color_SOLID,
    Draw_Color_BOTTOM_LEFT,
    Draw_Color_BOTTOM_RIGHT,
    Draw_Color_TOP_RIGHT,

    Draw_Color_MAX_VALUE,
} Draw_Color;

typedef enum Draw_Corner {
    Draw_Corner_TOP_LEFT,
    Draw_Corner_BOTTOM_LEFT,
    Draw_Corner_BOTTOM_RIGHT,
    Draw_Corner_TOP_RIGHT,

    Draw_Corner_COUNT,
} Draw_Corner;

structdef(Draw_Command) {
    Draw_Kind kind;
    using(V2, position);
    V4 color[Draw_Color_MAX_VALUE];
    bool gradient;
    union {
        struct {
            String string;
            f32 font_size;
        } text;
        struct {
            V2 pivot;
            f32 rotation_radians;
            V4 border_color;
            f32 border_width;
            f32 border_radius;
            V2 size;
        } rectangle;
        V2 quadrilateral[Draw_Corner_COUNT];
    };
};

struct Platform;
#define draw(platform, ...) draw_((platform), (Draw_Command){ __VA_ARGS__ })
static void draw_(struct Platform *platform, Draw_Command command);


#else // IMPLEMENTATION


#if !PLATFORM_NONE
static void draw_(struct Platform *platform, Draw_Command command) {
    push(&platform->draw_commands, command);
}
#endif


#endif // defined(BASE_NO_IMPLEMENTATION) || defined(BASE_NO_IMPLEMENTATION_APP)


#if defined(PLATFORM_SOKOL)
    #include "base_platform_sokol.c"
#endif
