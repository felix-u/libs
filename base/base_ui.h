enumdef(UI_Axis, u8) { ui_axis_x, ui_axis_y, ui_axis_count };
#define for_ui_axis(axis_var_name) for (UI_Axis axis_var_name = 0; axis_var_name < ui_axis_count; axis_var_name += 1)

structdef(UI_Interaction) { bool hovered, clicked; };

enumdef(UI_Size_Kind, u8) {
    ui_size_kind_nil,
    ui_size_kind_text,
    ui_size_kind_sum_of_children,
    ui_size_kind_largest_child,
};

typedef V4 UI_Rect;

structdef(UI_Size) {
    UI_Size_Kind kind;
    f32 value, strictness;
};

// TODO(felix): add flags for text alignment
enumdef(UI_Box_Flags, u8) {
    ui_box_flag_child_axis      = 1 << 0,
    ui_box_flag_clickable       = 1 << 1,
    ui_box_flag_draw_text       = 1 << 2,
    ui_box_flag_draw_border     = 1 << 3,
    ui_box_flag_draw_background = 1 << 4,
};

structdef(UI_Box_Build) {
    String str_to_hash, str_to_display;
    UI_Box_Flags flags;
    UI_Size size[ui_axis_count];
};

structdef(UI_Box_Style) {
    V2 pad, margin;
    V4 clr_fg, clr_bg, clr_border;
};

structdef(UI_Box) {
    UI_Box_Build build;
    usize key;

    UI_Box_Style style, target_style;

    UI_Box *parent;
    UI_Box *prev, *next;
    UI_Box *first, *last;

    f32 computed_size_px[ui_axis_count];
    f32 pos_rel_parent[ui_axis_count];
    UI_Rect rect, target_rect;

    UI_Interaction interaction;
};
typedef Array(UI_Box *) Array_UI_Box_Ptr;

structdef(UI_Style_Stack) {
    Array_V2 pad, margin;
    Array_V4 clr_fg, clr_bg, clr_border;
};

structdef(UI_State) {
    Arena *arena_persistent, *arena_frame;
    Gfx_Render_Ctx *render_ctx;
    f32 pos[ui_axis_count];

    UI_Box *root;
    UI_Box *current_parent;
    Array_UI_Box_Ptr box_hashmap;

    UI_Style_Stack style_stack;
};

#define ui_animation_speed 0.4f
#define ui_box_map_size    1024

#define ui_expand_idx(line) i##line
#define ui_defer_loop_idx(line) ui_expand_idx(line)
#define ui_defer_loop(begin, end)\
    for (\
        int ui_defer_loop_idx(__LINE__) = ((begin), 0);\
        ui_defer_loop_idx(__LINE__) != 1;\
        ui_defer_loop_idx(__LINE__) += 1, (end)\
    )
#define ui_parent(box) ui_defer_loop(ui_push_parent(box), ui_pop_parent())

#define ui_define_fmt_fns(ui_fn_takes_string_returns_interaction)\
    static UI_Interaction ui_fn_takes_string_returns_interaction##f(char *fmt, ...) {\
        va_list args; va_start(args, fmt);\
        String text = string_vprintf(ui_state.arena_frame, fmt, args);\
        va_end(args);\
        return ui_fn_takes_string_returns_interaction##S(text);\
    }\
    \
    static inline UI_Interaction ui_fn_takes_string_returns_interaction(char *cstring) {\
        return ui_fn_takes_string_returns_interaction##S(string_from_cstring(cstring));\
    }

static          UI_Box *ui_border_box(void);
static  inline  UI_Box *ui_box(String str);
static          UI_Box *ui_box_frame_local_not_keyed(void);
static            void  ui_begin_build(void);
static  UI_Interaction  ui_buttonS(String str);
static          UI_Box *ui_column(void);
static            void  ui_compute_layout_dependent_ancestor(UI_Box *box);
static            void  ui_compute_layout_dependent_descendant(UI_Box *box);
static            void  ui_compute_layout_relative_positions_and_rect(UI_Box *box);
static            void  ui_compute_layout_solve_violations(UI_Box *box);
static            void  ui_compute_layout_standalone(UI_Box *box);
static inline     void  ui_pop_parent(void);
static  UI_Interaction  ui_push(UI_Box *box);
static  UI_Interaction  ui_push_parent(UI_Box *box);
static inline     void  ui_rect_shift_axis(UI_Rect *rect, UI_Axis axis, f32 shift);
static inline     void  ui_rect_shift_x(UI_Rect *rect, f32 shift);
static inline     void  ui_rect_shift_y(UI_Rect *rect, f32 shift);
static            void  ui_render_recursive(UI_Box *box);
static          UI_Box *ui_row(void);
static  UI_Interaction  ui_textS(String str);
