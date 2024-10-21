enumdef(UI_Axis, u8) { ui_axis_x, ui_axis_y, ui_axis_count };
#define for_ui_axis(axis_var_name) for (UI_Axis axis_var_name = 0; axis_var_name < ui_axis_count; axis_var_name += 1)

structdef(UI_Interaction) { bool hovered, clicked; };

enumdef(UI_Size_Kind, u8) {
    ui_size_kind_nil,
    ui_size_kind_text,
    ui_size_kind_sum_of_children,
    ui_size_kind_largest_child,
};

typedef V4 UI_Rect, UI_Rgba;

structdef(UI_Size) {
    UI_Size_Kind kind;
    f32 value, strictness;
};

structdef(UI_Str_Parsed) { Str8 to_hash, to_display; };

// TODO(felix): add flags for text alignment
enumdef(UI_Box_Flags, u8) {
    ui_box_flag_child_axis      = 1 << 0,
    ui_box_flag_clickable       = 1 << 1,
    ui_box_flag_draw_text       = 1 << 2,
    ui_box_flag_draw_border     = 1 << 3,
    ui_box_flag_draw_background = 1 << 4,
};

structdef(UI_Box_Build) {
    UI_Str_Parsed str;
    UI_Box_Flags flags;
    UI_Size size[ui_axis_count];
};

structdef(UI_Box_Style) {
    V2 pad, margin;
    UI_Rgba clr_fg, clr_bg, clr_border;
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

structdef(UI_State) {
    Arena *arena_persistent, *arena_frame;
    Dwrite_Ctx *dw_ctx;
    f32 pos[ui_axis_count];

    UI_Box *root;
    UI_Box *current_parent;
    Array_UI_Box_Ptr box_hashmap;
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

#define ui_define_fmt_fns(ui_fn_takes_str8_returns_interaction)\
    static UI_Interaction ui_fn_takes_str8_returns_interaction##f(char *fmt, ...) {\
        va_list args; va_start(args, fmt);\
        usize reasonable_max = 256;\
        Str8 text = str8_vprintf(ui_state.arena_frame, reasonable_max, fmt, args);\
        va_end(args);\
        return ui_fn_takes_str8_returns_interaction##S(text);\
    }\
    \
    static inline UI_Interaction ui_fn_takes_str8_returns_interaction(char *strc) {\
        return ui_fn_takes_str8_returns_interaction##S(str8_from_strc(strc));\
    }

static inline  UI_Box *ui_box(Str8 str);
static           void  ui_box_add_child(UI_Box *parent, UI_Box *child);
static         UI_Box *ui_box_frame_local_not_keyed(void);
static   UI_Box_Style  ui_box_style_lerp(UI_Box_Style a, UI_Box_Style b, f32 amount);
static inline    void  ui_box_zero_tree_links(UI_Box *b);

static void ui_begin_build(void);

static UI_Interaction ui_buttonS(Str8 str);

static UI_Box *ui_column(void);

static void ui_compute_layout_dependent_ancestor(UI_Box *box);
static void ui_compute_layout_dependent_descendant(UI_Box *box);
static void ui_compute_layout_relative_positions_and_rect(UI_Box *box);
static void ui_compute_layout_solve_violations(UI_Box *box);
static void ui_compute_layout_standalone(UI_Box *box);

static void ui_end_build_and_compute_layout(void);

static UI_Box *ui_hash_find_or_alloc_new(usize key, UI_Str_Parsed str);
static   usize ui_hash_key_from_str(Str8 str);

static UI_Interaction ui_interaction_compute(UI_Box *box);

static inline bool ui_mouse_in_rect(UI_Rect rect);

static UI_Str_Parsed ui_parse_str(Str8 s);

static inline    void ui_pop_parent(void);
static UI_Interaction ui_push(UI_Box *box);
static UI_Interaction ui_push_parent(UI_Box *box);

static inline void ui_rect_shift_axis(UI_Rect *rect, UI_Axis axis, f32 shift);
static inline void ui_rect_shift_x(UI_Rect *rect, f32 shift);
static inline void ui_rect_shift_y(UI_Rect *rect, f32 shift);

#define     ui_render() ui_render_recurse(ui_state.root)
static void ui_render_recurse(UI_Box *box);

static UI_Box *ui_row(void);

static UI_Interaction ui_textS(Str8 str);
