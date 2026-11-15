// https://github.com/felix-u 2026-11-14
// Public domain. NO WARRANTY - use at your own risk.

#if !defined(UI_H)
#define UI_H


#if !defined(UI_MAX_BOX_SLOTS_EXPONENT)
    #define UI_MAX_BOX_SLOTS_EXPONENT 10
#endif

// includes null terminator
#if !defined(UI_MAX_HASH_STRING_SIZE)
    #define UI_MAX_HASH_STRING_SIZE 32
#endif

#if !defined(UI_MAX_SIZE_GROUPS)
    #define UI_MAX_SIZE_GROUPS 16
#endif

#if !defined(UI_DISPLAY_STRING_BUFFER_SIZE)
    #define UI_DISPLAY_STRING_BUFFER_SIZE 4096
#endif

#if !defined(UI_HASH_STRING_BUFFER_SIZE)
    #define UI_HASH_STRING_BUFFER_SIZE 2048
#endif

#if !defined(UI_MAX_STRING_LENGTH)
    #define UI_MAX_STRING_LENGTH 256
#endif

typedef unsigned ui_Flags;
enum {
    ui_X = 0,
    ui_Y = 1,
    ui_Axis_COUNT = 2,

    ui_Flag_CHILD_AXIS                     = 1 <<  0,
    ui_Flag_DRAW_BACKGROUND                = 1 <<  1,
    ui_Flag_DRAW_BORDER                    = 1 <<  2,
    ui_Flag_DRAW_TEXT                      = 1 <<  3,
    ui_Flag_WRAP_TEXT                      = 1 <<  4,
    ui_Flag_DRAW_SHADOW                    = 1 <<  5,
    ui_Flag_FLOATING                       = 1 <<  6,
    ui_Flag_HOVERABLE                      = 1 <<  7,
    ui_Flag_CLICKABLE                      = 1 <<  8,
    ui_Flag_CLIP_TO_PARENT                 = 1 <<  9,
    ui_Flag_SCROLL_X                       = 1 << 10,
    ui_Flag_SCROLL_Y                       = 1 << 11,
    ui_Flag_KEEP_INSIDE_ROOT_WHEN_FLOATING = 1 << 12,
    ui_Flag_FOCUSABLE                      = 1 << 13,
    ui_Flag_DISABLED_BUT_SWALLOWS_INPUT    = 1 << 14,
    ui_Flag_TRUNCATE_TEXT                  = 1 << 15,

    UI_SIGNAL_FIRST_BIT                         = 16,
    ui_Flag_HOVERED                        = 1 << 16,
    ui_Flag_PRESSED_LEFT                   = 1 << 17,
    ui_Flag_PRESSED_RIGHT                  = 1 << 18,
    ui_Flag_CLICKED                        = 1 << 19,
    ui_Flag_HELD_LEFT                      = 1 << 20,
    ui_Flag_RELEASED_LEFT                  = 1 << 21,
    ui_Flag_FOCUSED                        = 1 << 22,
    ui_Flag_HELD_OVER                      = 1 << 23,
    UI_SIGNAL_COUNT = 8,
};
_Static_assert(ui_Flag_HOVERED == 1 << UI_SIGNAL_FIRST_BIT && ui_Flag_HELD_OVER == 1 << (UI_SIGNAL_FIRST_BIT + UI_SIGNAL_COUNT - 1), "the signal flags must be the UI_SIGNAL_COUNT bits from UI_SIGNAL_FIRST_BIT");
#define UI_FLAG_ANY_VISIBLE (ui_Flag_DRAW_BACKGROUND | ui_Flag_DRAW_BORDER | ui_Flag_DRAW_TEXT | ui_Flag_DRAW_SHADOW)
#define UI_FLAG_ANY_INTERACTABLE (ui_Flag_HOVERABLE | ui_Flag_CLICKABLE | ui_Flag_FOCUSABLE)
#define UI_FLAG_PRESSED_OR_CLICKED (ui_Flag_PRESSED_LEFT | ui_Flag_CLICKED)
#define UI_FLAG_ANY_INTERACTION (ui_Flag_HOVERED | ui_Flag_PRESSED_LEFT | ui_Flag_CLICKED | ui_Flag_HELD_LEFT | ui_Flag_RELEASED_LEFT | ui_Flag_PRESSED_RIGHT | ui_Flag_HELD_OVER)

typedef struct {
    float position[ui_Axis_COUNT];
    float size[ui_Axis_COUNT];
} ui_Rectangle;

typedef unsigned char ui_Size_Kind;
enum {
    ui_Size_Kind_FIT = 0,
    ui_Size_Kind_FIXED,
    ui_Size_Kind_GROW,
    ui_Size_Kind_GROW_FROM_MINIMUM,
    ui_Size_Kind_PERCENT,

    ui_Size_Kind_COUNT,
};

typedef unsigned char ui_Align;
enum {
    ui_Align_START = 0,
    ui_Align_CENTER,
    ui_Align_END,
    ui_Align_BASELINE,

    ui_Align_COUNT,
};

typedef enum {
    ui_Side_LEFT   = ui_X,
    ui_Side_TOP    = ui_Y,
    ui_Side_RIGHT  = ui_Axis_COUNT + ui_X,
    ui_Side_BOTTOM = ui_Axis_COUNT + ui_Y,

    ui_Side_COUNT,
} ui_Side;

typedef struct {
    float font_size;
    unsigned font_id;
    float font_weight;
    _Bool font_italic;
    float padding[ui_Side_COUNT];
    float child_gap;
    ui_Align child_alignment[ui_Axis_COUNT];
    unsigned bg_color;
    unsigned fg_color;
    unsigned border_color;
    unsigned shadow_color;
    float border_width;
    float border_radius;
    float shadow_offset[ui_Axis_COUNT];
    float transparency;
} ui_Style;

typedef struct {
    float size[ui_Axis_COUNT];
    float ascent;
} ui_Text_Measurement;

typedef struct {
    ui_Size_Kind kind;
    float value;
    float min;
    float max;
} ui_Size;

typedef struct ui_Box ui_Box;
struct ui_Box {
    ui_Flags flags;
    const char *display_string;
    ui_Size size[ui_Axis_COUNT];
    ui_Style style;
    float float_offset_from_parent_top_left[ui_Axis_COUNT];
    unsigned long long user_data;
    unsigned size_group[ui_Axis_COUNT];

    ui_Box *parent;
    ui_Box *previous_sibling;
    ui_Box *next_sibling;
    ui_Box *first_child;
    ui_Box *last_child;

    char hash_string[UI_MAX_HASH_STRING_SIZE];
    unsigned key_hash;
    float scroll_offset[ui_Axis_COUNT];
    float min_size[ui_Axis_COUNT];
    float content_size[ui_Axis_COUNT];
    ui_Text_Measurement text_measurement;
    float text_wrap_width;
    int text_cut_length; // -1 when the whole text fits
    float text_position[ui_Axis_COUNT];
    ui_Rectangle rectangle;
    ui_Rectangle clip_rectangle;
    ui_Rectangle rounded_clip_rectangle;
    float rounded_clip_radius;

    unsigned long long frame_id;
};

#define UI_MAX_BOX_SLOTS (1 << UI_MAX_BOX_SLOTS_EXPONENT)
#define UI_MAX_KEYED_BOXES_EXPONENT (UI_MAX_BOX_SLOTS_EXPONENT - 1)
#define UI_MAX_KEYED_BOXES (1 << UI_MAX_KEYED_BOXES_EXPONENT)

typedef struct ui_State ui_State;
typedef ui_Text_Measurement (ui_text_measure_function)(ui_State *ui, const char *string, ui_Style style, float wrap_width);

typedef enum {
    ui_Mouse_Button_LEFT,
    ui_Mouse_Button_MIDDLE,
    ui_Mouse_Button_RIGHT,
    ui_Mouse_Button_COUNT,
} ui_Mouse_Button;

typedef enum {
    // 32 -> 96 use ASCII values
    ui_Key__ASCII_DELIMITER = 128,

    ui_Key_SHIFT,
    ui_Key_CONTROL,
    ui_Key_RETURN,
    ui_Key_ESCAPE,
    ui_Key_TAB,
    ui_Key_LEFT,
    ui_Key_DOWN,
    ui_Key_UP,
    ui_Key_RIGHT,

    ui_Key_F1, ui_Key_F2, ui_Key_F3, ui_Key_F4, ui_Key_F5,
    ui_Key_F6, ui_Key_F7, ui_Key_F8, ui_Key_F9, ui_Key_F10,
    ui_Key_F11, ui_Key_F12, ui_Key_F13, ui_Key_F14, ui_Key_F15,
    ui_Key_F16, ui_Key_F17, ui_Key_F18, ui_Key_F19, ui_Key_F20,
    ui_Key_F21, ui_Key_F22, ui_Key_F23, ui_Key_F24,

    ui_Key_MAX_VALUE,
} ui_Key;

// NOTE(felix): Every length in ui.h is in points, and scale is the pixels per point.
struct ui_State {
    float scale;
    float window_size[ui_Axis_COUNT];
    float mouse_position[ui_Axis_COUNT];
    _Bool mouse_clicked[ui_Mouse_Button_COUNT];
    _Bool mouse_down[ui_Mouse_Button_COUNT];
    float scroll_offset[ui_Axis_COUNT];
    _Bool key_down[ui_Key_MAX_VALUE];
    _Bool key_pressed[ui_Key_MAX_VALUE];

    void *user_data;
    ui_text_measure_function *measure_text;

    _Bool interacted_this_frame;

    int keyed_box_count;
    int per_frame_unkeyed_boxes_index;
    ui_Box boxes[UI_MAX_BOX_SLOTS];

    ui_Box *root;
    ui_Box *current_parent;
    ui_Box *box_under_mouse;
    union { // in the order of the signal flags
        struct { ui_Box *hovered_box, *pressed_box, *right_pressed_box, *clicked_box, *held_box, *released_box, *focused_box, *held_over_box; };
        ui_Box *signal_boxes[UI_SIGNAL_COUNT];
    };
    int tab_start_draw_index; // -1 for none
    _Bool new_boxes_are_disabled;
    _Bool uses_size_groups;
    float size_group_sizes[UI_MAX_SIZE_GROUPS];
    float mouse_position_at_press[ui_Axis_COUNT];
    int draw_order_count;
    ui_Box *draw_order[UI_MAX_BOX_SLOTS];

    int display_string_used;
    char display_string_buffer[UI_DISPLAY_STRING_BUFFER_SIZE];
    int hash_string_used;
    char hash_string_buffer[UI_HASH_STRING_BUFFER_SIZE];
    char format_buffer[UI_MAX_STRING_LENGTH];
    char hash_buffer[UI_MAX_STRING_LENGTH];

    unsigned long long frame_id;
};


#if !defined(UI_FUNCTION)
    #define UI_FUNCTION
#endif

#if !defined(ui_va_list)
    #include <stdarg.h>
    #define ui_va_list va_list
    #define ui_va_start va_start
    #define ui_va_end va_end
#endif

UI_FUNCTION void     ui_begin(ui_State *ui);
UI_FUNCTION void     ui_create(ui_State *ui, ui_text_measure_function *text_measure_function);
UI_FUNCTION void     ui_end(ui_State *ui);
UI_FUNCTION ui_Box  *ui_push(ui_State *ui, ui_Flags flags, const char *format, ...);
UI_FUNCTION ui_Box  *ui_push_styled(ui_State *ui, ui_Flags flags, const ui_Style *, const char *format, ...);
UI_FUNCTION ui_Box  *ui_pushv(ui_State *ui, ui_Flags flags, const char *format, ui_va_list arguments);
UI_FUNCTION int      ui_set_disabled(ui_State *ui, int disabled);
UI_FUNCTION ui_Box  *ui_set_parent(ui_State *ui, ui_Box *parent);
UI_FUNCTION ui_Box  *ui_spacer(ui_State *ui);

#define UI__GLUE_(a, b) a##b
#define UI__GLUE(a, b) UI__GLUE_(a, b)
#define UI__NAME_ON_LINE(name) UI__GLUE(name, __LINE__)

#define UI_DISABLED_IF(ui, condition) \
    for (int UI__NAME_ON_LINE(ui__previous_disabled) = ui_set_disabled((ui), (ui)->new_boxes_are_disabled || (condition)); \
         UI__NAME_ON_LINE(ui__previous_disabled) != -1; \
         ui_set_disabled((ui), UI__NAME_ON_LINE(ui__previous_disabled)), UI__NAME_ON_LINE(ui__previous_disabled) = -1)

#define UI_PARENT(ui, parent) \
    for (ui_Box *UI__NAME_ON_LINE(ui__previous_parent) = ui_set_parent((ui), (parent)); \
         UI__NAME_ON_LINE(ui__previous_parent) != 0; \
         ui_set_parent((ui), UI__NAME_ON_LINE(ui__previous_parent)), UI__NAME_ON_LINE(ui__previous_parent) = 0)


#endif // UI_H


#if defined(UI_IMPLEMENTATION)


#if !defined(UI_ASSERT)
    #include <assert.h>
    #define UI_ASSERT assert
#endif

#if !defined(UI_VSNPRINTF)
    #include <stdio.h>
    #define UI_VSNPRINTF vsnprintf
#endif

#if !defined(ui_size_t)
    #include <stddef.h>
    #define ui_size_t size_t
#endif

static inline void *ui__memset(void *destination_, int byte_, ui_size_t count) {
    UI_ASSERT(byte_ < 256);
    char byte = (char)byte_;
    char *destination = destination_;
    for (ui_size_t i = 0; i < count; i += 1) destination[i] = byte;
    return destination;
}

UI_FUNCTION void ui_create(ui_State *ui, ui_text_measure_function *text_measure_function) {
    ui__memset(ui, 0, sizeof *ui);
    ui->tab_start_draw_index = -1;
    ui->measure_text = text_measure_function;
}

#define UI__MAX(a, b) ((a) > (b) ? (a) : (b))
#define UI__MIN(a, b) ((a) < (b) ? (a) : (b))
#define UI__NO_LIMIT 3.4e38f

static float ui__size_clamped(ui_Size size, float value) {
    float clamped = UI__MAX(value, size.min);
    _Bool has_max = size.max > 0;
    if (has_max) clamped = UI__MIN(clamped, size.max);
    return clamped;
}

static float ui__children_total_size(ui_Box *box, int axis) {
    float total_size = 0;
    int child_count = 0;
    for (ui_Box *child = box->first_child; child != 0; child = child->next_sibling) {
        if (!(child->flags & ui_Flag_FLOATING)) {
            total_size += child->rectangle.size[axis];
            child_count += 1;
        }
    }
    if (child_count > 1) total_size += box->style.child_gap * (float)(child_count - 1);
    return total_size;
}

static float ui__children_largest_size(ui_Box *box, int axis) {
    float largest_size = 0;
    for (ui_Box *child = box->first_child; child != 0; child = child->next_sibling) {
        if (!(child->flags & ui_Flag_FLOATING)) largest_size = UI__MAX(largest_size, child->rectangle.size[axis]);
    }
    return largest_size;
}

static _Bool ui__scrolls_on_axis(ui_Box *box, int axis) {
    ui_Flags scroll_flag = axis == ui_X ? ui_Flag_SCROLL_X : ui_Flag_SCROLL_Y;
    return !!(box->flags & scroll_flag);
}

static float ui__padding_on_both_sides(ui_Style style, int axis) {
    return style.padding[axis] + style.padding[ui_Axis_COUNT + axis];
}

static const float ui__alignment_fractions[ui_Align_COUNT] = {
    [ui_Align_START]    = 0,
    [ui_Align_CENTER]   = 0.5f,
    [ui_Align_END]      = 1.f,
    [ui_Align_BASELINE] = 0,
};

static float ui__text_offset(ui_Box *box, int axis) {
    float inner_size = box->rectangle.size[axis] - ui__padding_on_both_sides(box->style, axis);
    float free_space = inner_size - box->text_measurement.size[axis];
    return box->style.padding[axis] + free_space * ui__alignment_fractions[box->style.child_alignment[axis]];
}

static float ui__baseline_from_top(ui_Box *box) {
    ui_Box *first_child = box->first_child;
    while (first_child != 0 && (first_child->flags & ui_Flag_FLOATING)) first_child = first_child->next_sibling;

    float baseline = box->rectangle.size[ui_Y];
    if (box->flags & ui_Flag_DRAW_TEXT) baseline = ui__text_offset(box, ui_Y) + box->text_measurement.ascent;
    else if (first_child != 0) {
        float inner_height = box->rectangle.size[ui_Y] - ui__padding_on_both_sides(box->style, ui_Y);
        _Bool children_stack_down = (int)(box->flags & ui_Flag_CHILD_AXIS) == ui_Y;
        float space_around_children = inner_height - (children_stack_down ? ui__children_total_size(box, ui_Y) : first_child->rectangle.size[ui_Y]);
        float first_child_top = box->style.padding[ui_Side_TOP] + UI__MAX(space_around_children, 0) * ui__alignment_fractions[box->style.child_alignment[ui_Y]];
        baseline = first_child_top + ui__baseline_from_top(first_child);
    }
    return baseline;
}

static float ui__children_largest_baseline(ui_Box *box) {
    float largest_baseline = 0;
    for (ui_Box *child = box->first_child; child != 0; child = child->next_sibling) {
        if (!(child->flags & ui_Flag_FLOATING)) largest_baseline = UI__MAX(largest_baseline, ui__baseline_from_top(child));
    }
    return largest_baseline;
}

// NOTE(felix): Axis sizes depend only on sizes on that axis, except that wrapped text needs final width to measure height. So ui_end fits and distributes X before Y.
static void ui__layout_fit(ui_State *ui, ui_Box *box, int axis) {
    for (ui_Box *child = box->first_child; child != 0; child = child->next_sibling) {
        ui__layout_fit(ui, child, axis);
    }

    float content_size = 0;
    float content_min_size = 0;
    _Bool has_text = !!(box->flags & ui_Flag_DRAW_TEXT);
    _Bool wraps_text = has_text && (box->flags & ui_Flag_WRAP_TEXT);
    if (has_text && axis == ui_X) {
        ui_Text_Measurement unwrapped = ui->measure_text(ui, box->display_string, box->style, 0);
        content_size = unwrapped.size[ui_X];
        content_min_size = unwrapped.size[ui_X];
        if (wraps_text) {
            ui_Text_Measurement one_word_per_line = ui->measure_text(ui, box->display_string, box->style, 1.f);
            content_min_size = one_word_per_line.size[ui_X];
        }
        if (box->flags & ui_Flag_TRUNCATE_TEXT) {
            ui_Text_Measurement dots = ui->measure_text(ui, "...", box->style, 0);
            content_min_size = dots.size[ui_X];
        }
    } else if (has_text) {
        box->text_wrap_width = wraps_text ? UI__MAX(box->rectangle.size[ui_X] - ui__padding_on_both_sides(box->style, ui_X), 1.f) : 0;
        box->text_measurement = ui->measure_text(ui, box->display_string, box->style, box->text_wrap_width);

        box->text_cut_length = -1;
        float available_width = box->rectangle.size[ui_X] - ui__padding_on_both_sides(box->style, ui_X);
        _Bool cuts_text = (box->flags & ui_Flag_TRUNCATE_TEXT) && !wraps_text && box->text_measurement.size[ui_X] > available_width + 0.5f;
        if (cuts_text) {
            int text_length = 0;
            while (box->display_string[text_length] != 0) text_length += 1;
            int fitting_length = 0;
            int too_long_length = text_length;
            box->text_measurement = ui->measure_text(ui, "...", box->style, 0);
            while (too_long_length - fitting_length > 1) {
                int middle = (fitting_length + too_long_length) / 2;
                UI_ASSERT(middle + 4 <= (int)(sizeof ui->format_buffer));
                for (int i = 0; i < middle; i += 1) ui->format_buffer[i] = box->display_string[i];
                ui->format_buffer[middle] = ui->format_buffer[middle + 1] = ui->format_buffer[middle + 2] = '.';
                ui->format_buffer[middle + 3] = 0;
                ui_Text_Measurement candidate = ui->measure_text(ui, ui->format_buffer, box->style, 0);
                _Bool fits = candidate.size[ui_X] <= available_width;
                if (fits) {
                    fitting_length = middle;
                    box->text_measurement = candidate;
                } else too_long_length = middle;
            }
            box->text_cut_length = fitting_length;
        }
        content_size = box->text_measurement.size[ui_Y];
        content_min_size = box->text_measurement.size[ui_Y];
    } else {
        _Bool along_child_axis = (int)(box->flags & ui_Flag_CHILD_AXIS) == axis;
        content_size = along_child_axis ? ui__children_total_size(box, axis) : ui__children_largest_size(box, axis);

        int child_count = 0;
        for (ui_Box *child = box->first_child; child != 0; child = child->next_sibling) {
            if (!(child->flags & ui_Flag_FLOATING)) {
                if (along_child_axis) content_min_size += child->min_size[axis];
                else content_min_size = UI__MAX(content_min_size, child->min_size[axis]);
                child_count += 1;
            }
        }
        if (along_child_axis && child_count > 1) content_min_size += box->style.child_gap * (float)(child_count - 1);

        _Bool aligns_baselines = axis == ui_Y && !along_child_axis && box->style.child_alignment[ui_Y] == ui_Align_BASELINE;
        if (aligns_baselines) {
            float largest_size_below_baseline = 0;
            for (ui_Box *child = box->first_child; child != 0; child = child->next_sibling) {
                if (!(child->flags & ui_Flag_FLOATING)) largest_size_below_baseline = UI__MAX(largest_size_below_baseline, child->rectangle.size[ui_Y] - ui__baseline_from_top(child));
            }
            content_size = ui__children_largest_baseline(box) + largest_size_below_baseline;
            content_min_size = UI__MAX(content_min_size, content_size);
        }

        if (ui__scrolls_on_axis(box, axis)) content_min_size = 0;
    }

    ui_Size size = box->size[axis];
    float padding = ui__padding_on_both_sides(box->style, axis);
    float fitted_size = content_size + padding;
    float min_size = ui__size_clamped(size, content_min_size + padding);
    if (size.kind == ui_Size_Kind_FIXED) fitted_size = min_size = size.value;
    if (size.kind == ui_Size_Kind_PERCENT) fitted_size = 0;
    box->rectangle.size[axis] = ui__size_clamped(size, fitted_size);
    box->min_size[axis] = min_size;

    unsigned group = box->size_group[axis];
    if (group != 0) {
        UI_ASSERT(group < UI_MAX_SIZE_GROUPS);
        float *group_size = &ui->size_group_sizes[group];
        *group_size = UI__MAX(*group_size, box->rectangle.size[axis]);
        box->rectangle.size[axis] = *group_size;
        box->min_size[axis] = UI__MAX(box->min_size[axis], *group_size);
        ui->uses_size_groups = 1;
    }
}

static float ui__room_to_change_size(ui_Box *child, int axis, float direction) {
    ui_Size size = child->size[axis];
    float current_size = child->rectangle.size[axis];
    float room = 0;
    _Bool grows = size.kind == ui_Size_Kind_GROW || size.kind == ui_Size_Kind_GROW_FROM_MINIMUM;
    if (direction > 0 && grows) room = size.max > 0 ? size.max - current_size : UI__NO_LIMIT;
    _Bool can_shrink = size.kind == ui_Size_Kind_FIT || grows;
    if (direction < 0 && can_shrink) room = current_size - child->min_size[axis];
    if (child->flags & ui_Flag_FLOATING) room = 0;
    return UI__MAX(room, 0);
}

static void ui__layout_even_out_children(ui_Box *box, int axis, float space) {
    float direction = space > 0 ? 1.f : -1.f;
    float space_left = space * direction;
    float tolerance = 0.01f;

    _Bool some_child_can_change = 1;
    while (space_left > tolerance && some_child_can_change) {
        float smallest_key = UI__NO_LIMIT;
        for (ui_Box *child = box->first_child; child != 0; child = child->next_sibling) {
            if (ui__room_to_change_size(child, axis, direction) > tolerance) smallest_key = UI__MIN(smallest_key, direction * child->rectangle.size[axis]);
        }
        some_child_can_change = smallest_key < UI__NO_LIMIT;

        float second_smallest_key = UI__NO_LIMIT;
        float size_change_per_child = space_left;
        int children_with_smallest_key_count = 0;
        for (ui_Box *child = box->first_child; child != 0; child = child->next_sibling) {
            float room = ui__room_to_change_size(child, axis, direction);
            float key = direction * child->rectangle.size[axis];
            _Bool has_smallest_key = key < smallest_key + tolerance;
            if (room > tolerance && has_smallest_key) {
                size_change_per_child = UI__MIN(size_change_per_child, room);
                children_with_smallest_key_count += 1;
            }
            if (room > tolerance && !has_smallest_key) second_smallest_key = UI__MIN(second_smallest_key, key);
        }

        if (some_child_can_change) {
            size_change_per_child = UI__MIN(size_change_per_child, space_left / (float)children_with_smallest_key_count);
            size_change_per_child = UI__MIN(size_change_per_child, second_smallest_key - smallest_key);
            for (ui_Box *child = box->first_child; child != 0; child = child->next_sibling) {
                float room = ui__room_to_change_size(child, axis, direction);
                _Bool has_smallest_key = direction * child->rectangle.size[axis] < smallest_key + tolerance;
                if (room > tolerance && has_smallest_key) child->rectangle.size[axis] += direction * size_change_per_child;
            }
            space_left -= size_change_per_child * (float)children_with_smallest_key_count;
        }
    }
}

static void ui__layout_distribute(ui_Box *box, int axis) {
    float inner_size = box->rectangle.size[axis] - ui__padding_on_both_sides(box->style, axis);
    _Bool along_child_axis = (int)(box->flags & ui_Flag_CHILD_AXIS) == axis;
    _Bool scrolls = ui__scrolls_on_axis(box, axis);

    for (ui_Box *child = box->first_child; child != 0; child = child->next_sibling) {
        ui_Size size = child->size[axis];
        float *child_size = &child->rectangle.size[axis];
        _Bool is_floating = !!(child->flags & ui_Flag_FLOATING);
        _Bool sizes_against_whole_parent = !along_child_axis || is_floating;
        _Bool grows = size.kind == ui_Size_Kind_GROW || size.kind == ui_Size_Kind_GROW_FROM_MINIMUM;
        if (size.kind == ui_Size_Kind_PERCENT) *child_size = ui__size_clamped(size, inner_size * size.value);
        if (sizes_against_whole_parent && grows) *child_size = ui__size_clamped(size, inner_size);
        if (!sizes_against_whole_parent && size.kind == ui_Size_Kind_GROW_FROM_MINIMUM && !scrolls) *child_size = child->min_size[axis];
        _Bool can_shrink = (size.kind == ui_Size_Kind_FIT || grows) && !scrolls;
        if (sizes_against_whole_parent && can_shrink && *child_size > inner_size) *child_size = UI__MAX(inner_size, child->min_size[axis]);
    }

    float space = inner_size - ui__children_total_size(box, axis);
    _Bool should_even_out = along_child_axis && (space > 0 || !scrolls);
    if (should_even_out) ui__layout_even_out_children(box, axis, space);

    for (ui_Box *child = box->first_child; child != 0; child = child->next_sibling) {
        ui__layout_distribute(child, axis);
    }
}

static ui_Rectangle ui__rectangle_intersection(ui_Rectangle a, ui_Rectangle b) {
    ui_Rectangle intersection = {0};
    for (int axis = 0; axis < ui_Axis_COUNT; axis += 1) {
        float start = UI__MAX(a.position[axis], b.position[axis]);
        float end = UI__MIN(a.position[axis] + a.size[axis], b.position[axis] + b.size[axis]);
        intersection.position[axis] = start;
        intersection.size[axis] = UI__MAX(end - start, 0);
    }
    return intersection;
}

static ui_Rectangle ui__inside_border(ui_Box *box) {
    float border_width = (box->flags & ui_Flag_DRAW_BORDER) ? box->style.border_width : 0;
    ui_Rectangle inside = box->rectangle;
    for (int axis = 0; axis < ui_Axis_COUNT; axis += 1) {
        inside.position[axis] += border_width;
        inside.size[axis] = UI__MAX(inside.size[axis] - 2.f * border_width, 0);
    }
    return inside;
}

static void ui__layout_position(ui_State *ui, ui_Box *box) {
    int child_axis = (int)(box->flags & ui_Flag_CHILD_AXIS);
    int cross_axis = !child_axis;

    float inner_position[ui_Axis_COUNT] = {0};
    float inner_size[ui_Axis_COUNT] = {0};
    float alignment_fraction[ui_Axis_COUNT] = {0};
    ui_Rectangle inside_border = ui__inside_border(box);
    ui_Rectangle scroll_clip = box->clip_rectangle;
    for (int axis = 0; axis < ui_Axis_COUNT; axis += 1) {
        inner_position[axis] = box->rectangle.position[axis] + box->style.padding[axis];
        inner_size[axis] = box->rectangle.size[axis] - ui__padding_on_both_sides(box->style, axis);
        alignment_fraction[axis] = ui__alignment_fractions[box->style.child_alignment[axis]];
        if (box->flags & ui_Flag_DRAW_TEXT) box->text_position[axis] = box->rectangle.position[axis] + ui__text_offset(box, axis);

        _Bool scrolls = ui__scrolls_on_axis(box, axis);
        float content_size = axis == child_axis ? ui__children_total_size(box, axis) : ui__children_largest_size(box, axis);
        box->content_size[axis] = content_size;
        float max_scroll_offset = scrolls ? UI__MAX(content_size - inner_size[axis], 0) : 0;
        box->scroll_offset[axis] = UI__MIN(UI__MAX(box->scroll_offset[axis], 0), max_scroll_offset);
        if (scrolls) {
            scroll_clip.position[axis] = inside_border.position[axis];
            scroll_clip.size[axis] = inside_border.size[axis];
        }
    }

    ui_Rectangle clip_for_children = ui__rectangle_intersection(box->clip_rectangle, scroll_clip);

    _Bool aligns_baselines = cross_axis == ui_Y && box->style.child_alignment[ui_Y] == ui_Align_BASELINE;
    float largest_baseline = aligns_baselines ? ui__children_largest_baseline(box) : 0;

    float free_space = UI__MAX(inner_size[child_axis] - ui__children_total_size(box, child_axis), 0);
    float cursor = inner_position[child_axis] + free_space * alignment_fraction[child_axis] - box->scroll_offset[child_axis];
    for (ui_Box *child = box->first_child; child != 0; child = child->next_sibling) {
        if (child->style.fg_color == 0) child->style.fg_color = box->style.fg_color;
        if (child->flags & ui_Flag_FLOATING) {
            ui_Rectangle root = ui->root->rectangle;
            for (int axis = 0; axis < ui_Axis_COUNT; axis += 1) {
                float position = box->rectangle.position[axis] + child->float_offset_from_parent_top_left[axis];
                if (child->flags & ui_Flag_KEEP_INSIDE_ROOT_WHEN_FLOATING) {
                    float largest_position_inside_root = root.position[axis] + root.size[axis] - child->rectangle.size[axis];
                    position = UI__MAX(UI__MIN(position, largest_position_inside_root), root.position[axis]);
                }
                child->rectangle.position[axis] = position;
            }
        } else {
            child->rectangle.position[child_axis] = cursor;
            cursor += child->rectangle.size[child_axis] + box->style.child_gap;

            float cross_free_space = UI__MAX(inner_size[cross_axis] - child->rectangle.size[cross_axis], 0);
            float cross_offset = cross_free_space * alignment_fraction[cross_axis];
            if (aligns_baselines) cross_offset = largest_baseline - ui__baseline_from_top(child);
            child->rectangle.position[cross_axis] = inner_position[cross_axis] + cross_offset - box->scroll_offset[cross_axis];
        }

        _Bool clips_only_to_root = (child->flags & ui_Flag_FLOATING) && !(child->flags & ui_Flag_CLIP_TO_PARENT);
        child->clip_rectangle = clips_only_to_root ? ui->root->clip_rectangle : clip_for_children;
        child->rounded_clip_rectangle = box->rounded_clip_rectangle;
        child->rounded_clip_radius = clips_only_to_root ? 0 : box->rounded_clip_radius;
        if (child->flags & ui_Flag_CLIP_TO_PARENT) {
            ui_Box *visible_ancestor = box;
            while (visible_ancestor->parent != 0 && !(visible_ancestor->flags & (UI_FLAG_ANY_VISIBLE | ui_Flag_FLOATING))) visible_ancestor = visible_ancestor->parent;
            ui_Rectangle inside_visible_ancestor = ui__inside_border(visible_ancestor);
            child->clip_rectangle = ui__rectangle_intersection(child->clip_rectangle, inside_visible_ancestor);

            float drawn_border_width = (visible_ancestor->flags & ui_Flag_DRAW_BORDER) ? visible_ancestor->style.border_width : 0;
            float inner_radius = visible_ancestor->style.border_radius - drawn_border_width;
            if (inner_radius > 0) {
                child->rounded_clip_rectangle = inside_visible_ancestor;
                child->rounded_clip_radius = inner_radius;
            }
        }

        ui__layout_position(ui, child);
    }
}

static void ui__append_to_draw_order(ui_State *ui, ui_Box *box) {
    UI_ASSERT(ui->draw_order_count < UI_MAX_BOX_SLOTS);
    ui->draw_order[ui->draw_order_count] = box;
    ui->draw_order_count += 1;
    for (ui_Box *child = box->first_child; child != 0; child = child->next_sibling) {
        if (!(child->flags & ui_Flag_FLOATING)) ui__append_to_draw_order(ui, child);
    }
}

static void ui__append_floating_to_draw_order(ui_State *ui, ui_Box *box) {
    for (ui_Box *child = box->first_child; child != 0; child = child->next_sibling) {
        if (child->flags & ui_Flag_FLOATING) ui__append_to_draw_order(ui, child);
        ui__append_floating_to_draw_order(ui, child);
    }
}

UI_FUNCTION void ui_begin(ui_State *ui) {
    ui_Box *top_box_under_mouse = 0;
    int top_box_draw_index = -1;
    for (int i = ui->draw_order_count - 1; i >= 0 && top_box_under_mouse == 0; i -= 1) {
        ui_Box *box = ui->draw_order[i];
        ui_Rectangle hit_area = ui__rectangle_intersection(box->rectangle, box->clip_rectangle);
        _Bool contains_mouse = 1;
        for (int axis = 0; axis < ui_Axis_COUNT; axis += 1) {
            float mouse = ui->mouse_position[axis];
            contains_mouse = contains_mouse && hit_area.position[axis] <= mouse && mouse < hit_area.position[axis] + hit_area.size[axis];
        }
        if (contains_mouse) {
            top_box_under_mouse = box;
            top_box_draw_index = i;
        }
    }

    ui->box_under_mouse = 0;
    ui_Box *scroll_box_under_mouse = 0;
    _Bool click_is_taken = 0;
    for (ui_Box *box = top_box_under_mouse; box != 0; box = box->parent) {
        _Bool is_disabled = !!(box->flags & ui_Flag_DISABLED_BUT_SWALLOWS_INPUT);
        _Bool is_interactable = (box->flags & UI_FLAG_ANY_INTERACTABLE) && !is_disabled;
        if (!click_is_taken && is_interactable) ui->box_under_mouse = box;
        click_is_taken = click_is_taken || is_interactable || is_disabled;
        if (scroll_box_under_mouse == 0 && (box->flags & (ui_Flag_SCROLL_X | ui_Flag_SCROLL_Y))) scroll_box_under_mouse = box;
    }

    if (scroll_box_under_mouse != 0) {
        for (int axis = 0; axis < ui_Axis_COUNT; axis += 1) {
            if (ui__scrolls_on_axis(scroll_box_under_mouse, axis)) scroll_box_under_mouse->scroll_offset[axis] += ui->scroll_offset[axis];
        }
    }

    ui->pressed_box = 0;
    ui->clicked_box = 0;
    ui->released_box = 0;
    ui->right_pressed_box = ui->mouse_clicked[ui_Mouse_Button_RIGHT] ? ui->box_under_mouse : 0;
    _Bool press_starts_on_clickable_box = ui->mouse_clicked[ui_Mouse_Button_LEFT] && ui->box_under_mouse != 0 && (ui->box_under_mouse->flags & ui_Flag_CLICKABLE);
    if (press_starts_on_clickable_box) {
        ui->pressed_box = ui->box_under_mouse;
        ui->held_box = ui->box_under_mouse;
    }
    if (ui->mouse_clicked[ui_Mouse_Button_LEFT]) {
        for (int axis = 0; axis < ui_Axis_COUNT; axis += 1) ui->mouse_position_at_press[axis] = ui->mouse_position[axis];
    }
    _Bool held_box_is_released = ui->held_box != 0 && !ui->mouse_down[ui_Mouse_Button_LEFT];
    if (held_box_is_released) {
        ui->released_box = ui->held_box;
        if (ui->box_under_mouse == ui->held_box) ui->clicked_box = ui->held_box;
        ui->held_box = 0;
    }
    _Bool hover_is_free = ui->held_box == 0 || ui->held_box == ui->box_under_mouse;
    ui->hovered_box = hover_is_free ? ui->box_under_mouse : 0;

    _Bool any_mouse_button_went_down = 0;
    for (int button = 0; button < ui_Mouse_Button_COUNT; button += 1) any_mouse_button_went_down = any_mouse_button_went_down || ui->mouse_clicked[button];
    if (any_mouse_button_went_down) {
        ui->focused_box = 0;
        ui->tab_start_draw_index = top_box_draw_index;
    }

    if (ui->key_pressed[ui_Key_TAB]) {
        int start_index = ui->tab_start_draw_index < ui->draw_order_count ? ui->tab_start_draw_index : -1;
        for (int i = 0; i < ui->draw_order_count; i += 1) {
            if (ui->focused_box != 0 && ui->draw_order[i] == ui->focused_box) start_index = i;
        }

        ui_Box *first_focusable = 0;
        ui_Box *last_focusable = 0;
        ui_Box *focusable_before_start = 0;
        ui_Box *focusable_after_start = 0;
        for (int i = 0; i < ui->draw_order_count; i += 1) {
            ui_Box *box = ui->draw_order[i];
            _Bool can_take_focus = (box->flags & ui_Flag_FOCUSABLE) && !(box->flags & ui_Flag_DISABLED_BUT_SWALLOWS_INPUT);
            if (can_take_focus) {
                if (first_focusable == 0) first_focusable = box;
                last_focusable = box;
                if (i < start_index) focusable_before_start = box;
                if (i > start_index && focusable_after_start == 0) focusable_after_start = box;
            }
        }

        _Bool backwards = ui->key_down[ui_Key_SHIFT];
        ui_Box *wrapped_focus = backwards ? last_focusable : first_focusable;
        ui_Box *next_focus = backwards ? focusable_before_start : focusable_after_start;
        if (start_index < 0 || next_focus == 0) next_focus = wrapped_focus;
        ui->focused_box = next_focus;
        ui->tab_start_draw_index = -1;
    }
    _Bool focused_box_is_disabled = ui->focused_box != 0 && (ui->focused_box->flags & ui_Flag_DISABLED_BUT_SWALLOWS_INPUT);
    if (focused_box_is_disabled) ui->focused_box = 0;
    if (ui->focused_box != 0 && ui->key_pressed[' ']) ui->clicked_box = ui->focused_box;
    ui_Box *previous_held_over_box = ui->held_over_box;
    ui->held_over_box = ui->held_box != 0 && ui->held_box == ui->box_under_mouse ? ui->held_box : 0;
    if (ui->focused_box != 0 && ui->key_down[' ']) ui->held_over_box = ui->focused_box;
    ui_Box *box_input_lands_on = ui->clicked_box != 0 ? ui->clicked_box : ui->pressed_box;
    _Bool keeps_previous_look = box_input_lands_on != 0 && (ui->held_over_box == box_input_lands_on || previous_held_over_box == box_input_lands_on);
    if (keeps_previous_look) ui->held_over_box = previous_held_over_box == box_input_lands_on ? box_input_lands_on : 0;

    ui->per_frame_unkeyed_boxes_index = UI_MAX_BOX_SLOTS;
    for (int i = 0; i < UI_MAX_KEYED_BOXES; i += 1) {
        ui_Box *box = &ui->boxes[i];
        _Bool stale = box->hash_string[0] != 0 && box->frame_id != ui->frame_id;
        if (stale) {
            *box = (ui_Box){0};
            ui->keyed_box_count -= 1;
        }
    }
    ui->frame_id += 1;

    for (int i = 0; i < UI_SIGNAL_COUNT; i += 1) {
        _Bool box_was_removed = ui->signal_boxes[i] != 0 && ui->signal_boxes[i]->hash_string[0] == 0;
        if (box_was_removed) ui->signal_boxes[i] = 0;
    }

    ui->root = 0;
    ui->current_parent = 0;
    ui->interacted_this_frame = 0;
    ui->display_string_used = 0;
}

UI_FUNCTION void ui_end(ui_State *ui) {
    UI_ASSERT(ui->root != 0);
    UI_ASSERT(ui->current_parent == ui->root);
    for (int axis = 0; axis < ui_Axis_COUNT; axis += 1) {
        ui__memset(ui->size_group_sizes, 0, sizeof ui->size_group_sizes);
        ui->uses_size_groups = 0;
        ui__layout_fit(ui, ui->root, axis);
        if (ui->uses_size_groups) ui__layout_fit(ui, ui->root, axis);
        ui__layout_distribute(ui->root, axis);
    }
    ui->root->clip_rectangle = ui->root->rectangle;
    ui->root->rounded_clip_radius = 0;
    ui__layout_position(ui, ui->root);

    ui->draw_order_count = 0;
    ui__append_to_draw_order(ui, ui->root);
    ui__append_floating_to_draw_order(ui, ui->root);
}

UI_FUNCTION ui_Box *ui_push(ui_State *ui, ui_Flags flags, const char *format, ...) {
    ui_va_list arguments;
    ui_va_start(arguments, format);
    ui_Box *box = ui_pushv(ui, flags, format, arguments);
    ui_va_end(arguments);
    return box;
}

UI_FUNCTION ui_Box *ui_push_styled(ui_State *ui, ui_Flags flags, const ui_Style *style, const char *format, ...) {
    ui_va_list arguments;
    ui_va_start(arguments, format);
    ui_Box *box = ui_pushv(ui, flags, format, arguments);
    ui_va_end(arguments);

    UI_ASSERT(style != 0);
    box->style = *style;

    return box;
}

UI_FUNCTION ui_Box *ui_pushv(ui_State *ui, ui_Flags flags, const char *format_c, ui_va_list arguments) {
    ui_Box *parent = ui->current_parent;
    _Bool keyed = format_c != 0;

    const char *hash_string = 0;
    const char *display_string = 0;

    ui_Box *box = 0;
    _Bool box_is_new = !keyed;
    if (keyed) {
        const char *format = 0;
        int format_length = 0;
        {
            int written = UI_VSNPRINTF(ui->format_buffer, sizeof ui->format_buffer, format_c, arguments);
            UI_ASSERT(written < (int)(sizeof ui->format_buffer));

            format = ui->format_buffer;
            format_length = written;
        }

        _Bool has_2_hash_marker = 0;
        _Bool has_3_hash_marker = 0;

        int marker_start = 0;
        for (int i = 0; format[i] != 0; i += 1) {
            if (format[i] != '#') continue;

            if (format[i + 1] == 0) break;
            if (format[i + 1] != '#') continue;

            has_3_hash_marker = format[i + 2] == '#';
            has_2_hash_marker = !has_3_hash_marker;

            marker_start = i;
            break;
        }

        int display_and_hash_left_end = marker_start;
        int hash_right_start = marker_start;

        hash_right_start += 2 * has_2_hash_marker;

        int hash_left_start = display_and_hash_left_end * has_3_hash_marker;
        hash_right_start += 3 * has_3_hash_marker;

        _Bool has_marker = has_2_hash_marker || has_3_hash_marker;
        int display_part_end = has_marker ? display_and_hash_left_end : format_length;

        int hash_string_length = 0;
        {
            int hash_cursor = hash_left_start;

            int *written = &hash_string_length;
            while (hash_cursor < display_and_hash_left_end && *written < (int)(sizeof ui->hash_buffer)) {
                ui->hash_buffer[(*written)++] = format[hash_cursor++];
            }

            hash_cursor = hash_right_start;
            while (format[hash_cursor] != 0) {
                ui->hash_buffer[(*written)++] = format[hash_cursor++];
            }

            UI_ASSERT(*written < (int)(sizeof ui->hash_buffer));
            ui->hash_buffer[*written] = 0;
            hash_string = ui->hash_buffer;
        }

        {
            UI_ASSERT(ui->display_string_used + display_part_end < (int)(sizeof ui->display_string_buffer));

            int *written = &ui->display_string_used;
            display_string = &ui->display_string_buffer[*written];

            for (int i = 0; i < display_part_end; i += 1) {
                ui->display_string_buffer[(*written)++] = format[i];
            }
            ui->display_string_buffer[(*written)++] = 0;
        }

        if (hash_string_length == 0) keyed = 0;

        if (keyed) {
            unsigned hash = 5381;
            for (int i = 0; i < hash_string_length; i += 1) {
                hash += (hash << 5) + (unsigned)(hash_string[i]);
            }

            unsigned exponent = UI_MAX_KEYED_BOXES_EXPONENT;
            unsigned mask = ((unsigned)1 << exponent) - 1;
            unsigned step = (hash >> (32 - exponent)) | 1;
            ui_Box *first_unused_box = 0;
            unsigned index = hash;
            for (int probe_count = 0; probe_count < UI_MAX_KEYED_BOXES && box == 0; probe_count += 1) {
                index = (index + step) & mask;
                index += !index; // never 0

                ui_Box *this_box = &ui->boxes[index];
                _Bool this_box_is_unused = this_box->hash_string[0] == 0;
                if (this_box_is_unused && first_unused_box == 0) first_unused_box = this_box;

                _Bool equals = 0;
                if (!this_box_is_unused && this_box->key_hash == hash) {
                    int kept_length = (int)sizeof this_box->hash_string - 1;
                    int i = 0;
                    equals = 1;
                    while (equals && i < kept_length && hash_string[i] != 0) {
                        equals = this_box->hash_string[i] == hash_string[i];
                        i += 1;
                    }
                    if (equals && i < kept_length) equals = this_box->hash_string[i] == 0;
                }

                if (equals) {
                    _Bool already_used_this_frame = this_box->frame_id == ui->frame_id;
                    UI_ASSERT(!already_used_this_frame);
                    box = this_box;
                }
            }

            if (box == 0) {
                UI_ASSERT(first_unused_box != 0);
                ui->keyed_box_count += 1;
                int hashmap_max_load = 70;
                int max_used_boxes = UI_MAX_KEYED_BOXES * hashmap_max_load / 100;
                _Bool hashmap_saturated = ui->keyed_box_count > max_used_boxes;
                UI_ASSERT(!hashmap_saturated);

                box_is_new = 1;
                box = first_unused_box;
            }

            if (box_is_new) {
                box->key_hash = hash;
                int i = 0;
                while (i < (int)(sizeof box->hash_string - 1) && hash_string[i] != 0) {
                    box->hash_string[i] = hash_string[i];
                    i += 1;
                }
                UI_ASSERT(i < (int)(sizeof box->hash_string));
                box->hash_string[i] = 0;
            }
        }
    }

    if (!keyed) {
        _Bool have_free_unkeyed_slot = ui->per_frame_unkeyed_boxes_index > UI_MAX_KEYED_BOXES;
        UI_ASSERT(have_free_unkeyed_slot);
        box = &ui->boxes[--ui->per_frame_unkeyed_boxes_index];
        box_is_new = 1;
    }

    box->flags = flags;
    if (ui->new_boxes_are_disabled) box->flags |= ui_Flag_DISABLED_BUT_SWALLOWS_INPUT;

    {
        box->display_string = 0;
        box->size[ui_X] = (ui_Size){0};
        box->size[ui_Y] = (ui_Size){0};
        box->float_offset_from_parent_top_left[ui_X] = 0;
        box->float_offset_from_parent_top_left[ui_Y] = 0;
        box->style = (ui_Style){0};
        box->user_data = 0;
        box->size_group[ui_X] = 0;
        box->size_group[ui_Y] = 0;
        box->parent = 0;
        box->previous_sibling = 0;
        box->next_sibling = 0;
        box->first_child = 0;
        box->last_child = 0;
    }

    box->parent = parent;
    if (parent == 0) UI_ASSERT(ui->root == 0);
    if (ui->root == 0) {
        UI_ASSERT(parent == 0);

        for (int axis = 0; axis < ui_Axis_COUNT; axis += 1) {
            box->size[axis] = (ui_Size){ .kind = ui_Size_Kind_FIXED, .value = ui->window_size[axis] };
        }

        ui->root = box;
        ui->current_parent = box;
    }

    box->display_string = display_string;
    if (display_string == 0 || display_string[0] == 0) box->flags &= ~(ui_Flags)ui_Flag_DRAW_TEXT;

    if (box->parent != 0) {
        if (box->parent->first_child == 0) {
            UI_ASSERT(box->parent->last_child == 0);
            box->parent->first_child = box;
            box->parent->last_child = box;
        } else {
            UI_ASSERT(box->parent->last_child != 0);
            box->parent->last_child->next_sibling = box;
            box->previous_sibling = box->parent->last_child;
            box->parent->last_child = box;
        }
    }

    if (box->flags & UI_FLAG_ANY_INTERACTABLE) UI_ASSERT(box->hash_string[0] != 0);

    if (box->flags & UI_FLAG_ANY_INTERACTABLE) {
        for (int i = 0; i < UI_SIGNAL_COUNT; i += 1) box->flags |= (ui_Flags)(box == ui->signal_boxes[i]) << (UI_SIGNAL_FIRST_BIT + i);
        if (!(box->flags & ui_Flag_HOVERABLE)) box->flags &= ~(ui_Flags)ui_Flag_HOVERED;

        ui->interacted_this_frame |= !!(box->flags & UI_FLAG_ANY_INTERACTION);
    }

    box->frame_id = ui->frame_id;
    return box;
}

UI_FUNCTION int ui_set_disabled(ui_State *ui, int disabled) {
    int previous_disabled = ui->new_boxes_are_disabled;
    ui->new_boxes_are_disabled = !!disabled;
    return previous_disabled;
}

UI_FUNCTION ui_Box *ui_set_parent(ui_State *ui, ui_Box *parent) {
    UI_ASSERT(parent != 0);
    UI_ASSERT(ui->current_parent != 0);
    ui_Box *previous_parent = ui->current_parent;
    ui->current_parent = parent;
    return previous_parent;
}

UI_FUNCTION ui_Box *ui_spacer(ui_State *ui) {
    ui_Box *box = ui_push(ui, 0, 0);
    box->size[ui_X] = box->size[ui_Y] = (ui_Size){ .kind = ui_Size_Kind_GROW };
    return box;
}

#endif // UI_IMPLEMENTATION
