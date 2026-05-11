// https://github.com/felix-u 2026-05-10
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

#if !defined(UI_DISPLAY_STRING_BUFFER_SIZE)
    #define UI_DISPLAY_STRING_BUFFER_SIZE 4096
#endif

#if !defined(UI_HASH_STRING_BUFFER_SIZE)
    #define UI_HASH_STRING_BUFFER_SIZE 2048
#endif

#if !defined(UI_MAX_STRING_LENGTH)
    #define UI_MAX_STRING_LENGTH 256
#endif

typedef unsigned short ui_Flags;
enum {
    ui_X = 0,
    ui_Y = 1,
    ui_Axis_COUNT = 2,

    ui_Flag_CHILD_AXIS      = 1 <<  0,
    ui_Flag_DRAW_BACKGROUND = 1 <<  1,
    ui_Flag_DRAW_BORDER     = 1 <<  2,
    ui_Flag_DRAW_TEXT       = 1 <<  3,
    ui_Flag_DRAW_COMMAND    = 1 <<  4,
    ui_Flag_DRAW_SHADOW     = 1 <<  5,
    ui_Flag_HOVERABLE       = 1 <<  7,
    ui_Flag_CLICKABLE       = 1 <<  8,
    ui_Flag_DRAGGABLE       = 1 <<  9,
    ui_Flag_CLIP_TO_PARENT  = 1 << 10,

    // computed by builder code
    ui_Flag_HOVERED         = 1 << 11,
    ui_Flag_CLICKED         = 1 << 12,
    ui_Flag_DRAGGING        = 1 << 13, // persisted across frames
};
#define UI_FLAG_ANY_VISIBLE (ui_Flag_DRAW_BACKGROUND | ui_Flag_DRAW_BORDER | ui_Flag_DRAW_TEXT | ui_Flag_DRAW_COMMAND | ui_Flag_DRAW_SHADOW)
#define UI_FLAG_ANY_INTERACTABLE (ui_Flag_HOVERABLE | ui_Flag_CLICKABLE | ui_Flag_DRAGGABLE)
#define UI_FLAG_ANY_INTERACTION (ui_Flag_HOVERED | ui_Flag_CLICKED | ui_Flag_DRAGGING)

typedef struct {
    float position[ui_Axis_COUNT];
    float size[ui_Axis_COUNT];
} ui_Rectangle;

typedef enum {
    ui_Size_NIL = 0,
    ui_Size_PIXELS,
    ui_Size_TEXT,
    ui_Size_SUM_OF_CHILDREN,
    ui_Size_LARGEST_CHILD,
    ui_Size_OF_PARENT, // TODO(felix): replace with GROW (simpler)

    ui_Size_Kind_COUNT,
} ui_Size_Kind;

typedef struct {
    float font_size;
    float padding[ui_Axis_COUNT];
    float child_gap;
    unsigned bg_color;
    unsigned fg_color;
    unsigned border_color;
    unsigned shadow_color;
    float border_width;
    float border_radius;
    float shadow_offset[ui_Axis_COUNT];
} ui_Style;

typedef struct {
    ui_Size_Kind kind;
    float value;
} ui_Size;

typedef struct ui_Box ui_Box;
struct ui_Box {
    // === frame data ===
    ui_Flags flags; // some bits persisted across frames
    const char *display_string;
    ui_Size size[ui_Axis_COUNT];
    ui_Style style;
    // links
    ui_Box *parent;
    ui_Box *previous_sibling;
    ui_Box *next_sibling;
    ui_Box *first_child;
    ui_Box *last_child;

    // === cached data ===
    void *user_data;
    char hash_string[UI_MAX_HASH_STRING_SIZE];
    // computed
    float drag_start_value;
    float drag_start_mouse[ui_Axis_COUNT];
    ui_Rectangle rectangle;

    unsigned long long frame_id;
};

#define UI_MAX_BOX_SLOTS (1 << UI_MAX_BOX_SLOTS_EXPONENT)
#define UI_MAX_KEYED_BOXES_EXPONENT (UI_MAX_BOX_SLOTS_EXPONENT - 1)
#define UI_MAX_KEYED_BOXES (1 << UI_MAX_KEYED_BOXES_EXPONENT)

typedef void (ui_text_measure_function)(void *user_data, const char *string, ui_Style style, float size[2]);

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

typedef struct {
    float window_size[ui_Axis_COUNT];
    float mouse_position[ui_Axis_COUNT];
    _Bool mouse_clicked[ui_Mouse_Button_COUNT];
    _Bool mouse_down[ui_Mouse_Button_COUNT];
    _Bool key_pressed[ui_Key_MAX_VALUE];

    void *user_data;
    ui_text_measure_function *measure_text;
    _Bool input_blocked;

    _Bool interacted_this_frame;

    int keyed_box_count; // for asserting we don't go over the 70% usage threshold. hashmap uses first half of boxes
    int per_frame_unkeyed_boxes_index; // grows down from last box to midpoint
    ui_Box boxes[UI_MAX_BOX_SLOTS];

    ui_Box *root;

    int display_string_used;
    char display_string_buffer[UI_DISPLAY_STRING_BUFFER_SIZE];
    int hash_string_used;
    char hash_string_buffer[UI_HASH_STRING_BUFFER_SIZE];
    char format_buffer[UI_MAX_STRING_LENGTH];
    char hash_buffer[UI_MAX_STRING_LENGTH];

    unsigned long long frame_id;
} ui_State;

typedef struct {
    ui_Style bar;
    ui_Style dragging;

    float length;
    float thickness;
    float radius;

    float handle_length;
    float handle_thickness;
    float handle_radius;
} ui_Slider_Style;

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
UI_FUNCTION ui_Box  *ui_push(ui_State *ui, ui_Box *parent, ui_Flags flags, const char *format, ...);
UI_FUNCTION ui_Box  *ui_pushv(ui_State *ui, ui_Box *parent, ui_Flags flags, const char *format, ui_va_list arguments);
UI_FUNCTION ui_Box  *ui_push_styled(ui_State *ui, ui_Box *parent, ui_Flags flags, const ui_Style *, const char *format, ...);
UI_FUNCTION ui_Box  *ui_spacer(ui_State *ui, ui_Box *parent);
UI_FUNCTION ui_Box  *ui_slider(ui_State *ui, ui_Box *parent, ui_Flags slider_axis, float least, float most, float *value, ui_Slider_Style style, const char *format, ...);


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

UI_FUNCTION void ui_begin(ui_State *ui) {
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

    ui->root = 0;
    ui->interacted_this_frame = 0;
    ui->display_string_used = 0;
}

static inline void *ui__memset(void *destination_, int byte_, ui_size_t count) {
    UI_ASSERT(byte_ < 256);
    char byte = (char)byte_;
    char *destination = destination_;
    for (ui_size_t i = 0; i < count; i += 1) destination[i] = byte;
    return destination;
}

UI_FUNCTION void ui_create(ui_State *ui, ui_text_measure_function *text_measure_function) {
    ui__memset(ui, 0, sizeof *ui);
    ui->measure_text = text_measure_function;
}

static void ui__layout_standalone(ui_State *ui, ui_Box *box) {
    ui_Style *style = &box->style;

    box->rectangle = (ui_Rectangle){0};

    float text_size[2] = {0};
    _Bool measure = box->size[ui_X].kind == ui_Size_TEXT || box->size[ui_Y].kind == ui_Size_TEXT;
    if (measure) {
        UI_ASSERT(box->display_string != 0);
        UI_ASSERT(box->display_string[0] != 0);
        ui->measure_text(ui->user_data, box->display_string, box->style, text_size);
        UI_ASSERT(text_size[0] > 0);
        UI_ASSERT(text_size[1] > 0);
    }

    for (int axis = 0; axis < ui_Axis_COUNT; axis += 1) {
        switch (box->size[axis].kind) {
            case ui_Size_PIXELS: {
                box->rectangle.size[axis] = box->size[axis].value;
            } break;
            case ui_Size_OF_PARENT: /* not solved here */ break;
            case ui_Size_LARGEST_CHILD: /* not solved here */ break;
            case ui_Size_TEXT: {
                box->rectangle.size[axis] = text_size[axis];
            } break;
            case ui_Size_SUM_OF_CHILDREN: /* not solved here */ break;
            default: UI_ASSERT(0 && "unreachable"); break;
        }

		box->rectangle.size[axis] += 2.f * style->padding[axis];
    }

    for (ui_Box *child = box->first_child; child != 0; child = child->next_sibling) {
        ui__layout_standalone(ui, child);
    }
}

static void ui__layout_dependent_ancestor(ui_State *ui, ui_Box *box) {
    for (int axis = 0; axis < ui_Axis_COUNT; axis += 1) switch (box->size[axis].kind) {
        case ui_Size_LARGEST_CHILD: /* not solved here */ break;
        case ui_Size_SUM_OF_CHILDREN: /* not solved here */ break;
        case ui_Size_OF_PARENT: /* not solved here */ break;
        case ui_Size_TEXT: /* not solved here */ break;
        case ui_Size_PIXELS: /* not solved here */ break;
        default: UI_ASSERT(0 && "unreachable"); break;
    }

    for (ui_Box *child = box->first_child; child != 0; child = child->next_sibling) {
        ui__layout_dependent_ancestor(ui, child);
    }
}

static void ui__layout_dependent_descendant(ui_State *ui, ui_Box *box) {
    for (ui_Box *child = box->first_child; child != 0; child = child->next_sibling) {
        ui__layout_dependent_descendant(ui, child);
    }

    for (int axis = 0; axis < ui_Axis_COUNT; axis += 1) switch (box->size[axis].kind) {
        case ui_Size_TEXT: break;
        case ui_Size_SUM_OF_CHILDREN: {
            for (ui_Box *child = box->first_child; child != 0; child = child->next_sibling) {
                box->rectangle.size[axis] += child->rectangle.size[axis];
				box->rectangle.size[axis] += box->style.child_gap;
            }

            _Bool has_at_least_one_child = box->first_child != 0;
            box->rectangle.size[axis] -= has_at_least_one_child * box->style.child_gap;
        } break;
        case ui_Size_LARGEST_CHILD: {
            float largest = 0;
            for (ui_Box *child = box->first_child; child != 0; child = child->next_sibling) {
                float size = child->rectangle.size[axis];
                largest = size > largest ? size : largest;
            }
            box->rectangle.size[axis] += largest;
        } break;
        case ui_Size_OF_PARENT: /* not solved here */ break;
        case ui_Size_PIXELS: /* not solved here */ break;
        default: UI_ASSERT(0 && "unreachable"); break;
    }
}

static void ui__layout_relative_positions_and_rectangle(ui_State *ui, ui_Box *box) {
    if (box->parent != 0) {
        ui_Flags layout_axis = box->parent->flags & ui_Flag_CHILD_AXIS;

        if (box->previous_sibling != 0) {
            box->rectangle.position[layout_axis] = box->previous_sibling->rectangle.position[layout_axis];
            box->rectangle.position[layout_axis] += box->previous_sibling->rectangle.size[layout_axis];
            box->rectangle.position[layout_axis] += box->parent->style.child_gap;
        } else {
            box->rectangle.position[layout_axis] = box->parent->rectangle.position[layout_axis];
			box->rectangle.position[layout_axis] += box->parent->style.padding[layout_axis];
        }

        ui_Flags non_layout_axis = !layout_axis;
        box->rectangle.position[non_layout_axis] = box->parent->rectangle.position[non_layout_axis];
        box->rectangle.position[non_layout_axis] += box->parent->style.padding[non_layout_axis];
    }

    if (box->flags & UI_FLAG_ANY_VISIBLE) {
        ui_Rectangle display = box->rectangle;
        for (int axis = 0; axis < ui_Axis_COUNT; axis += 1) {
            UI_ASSERT(display.position[axis] >= 0);
            UI_ASSERT(display.size[axis] >= 0);
        }
    }

    for (ui_Box *child = box->first_child; child != 0; child = child->next_sibling) {
        ui__layout_relative_positions_and_rectangle(ui, child);
    }
}

#define UI__MIN(a, b) ((a) < (b) ? (a) : (b))
#define UI__MAX(a, b) ((a) > (b) ? (a) : (b))
#define UI__CLAMP(v, low, high) UI__MIN(UI__MAX((v), (low)), (high))

static void ui__layout_solve_violations(ui_State *ui, ui_Box *box) {
    ui_Flags layout_axis = box->flags & ui_Flag_CHILD_AXIS;

    for (int axis = 0; axis < ui_Axis_COUNT; axis += 1) {
        float applies_to_this_axis = (float)(layout_axis == axis);

        float parent_size = box->rectangle.size[axis];
        parent_size -= 2.f * box->style.padding[axis];
        float available_size = parent_size;

        float wanted_size = 0;
        float child_count = 0;

        for (ui_Box *child = box->first_child; child != 0; child = child->next_sibling) {
            child_count += applies_to_this_axis;

            switch (child->size[axis].kind) {
                case ui_Size_OF_PARENT: {
                    child->rectangle.size[axis] = parent_size * child->size[axis].value;
                    wanted_size += applies_to_this_axis * child->rectangle.size[axis];
                } break;
                case ui_Size_SUM_OF_CHILDREN: case ui_Size_LARGEST_CHILD: case ui_Size_TEXT: case ui_Size_PIXELS: {
                    available_size -= applies_to_this_axis * child->rectangle.size[axis];
                } break;
                default: UI_ASSERT(0 && "unreachable"); break;
            }
        }

        float total_child_gap = box->style.child_gap * (child_count - 1.f);
        available_size -= total_child_gap;

        if (wanted_size > available_size) {
            for (ui_Box *child = box->first_child; child != 0; child = child->next_sibling) if (child->size[axis].kind == ui_Size_OF_PARENT) {
                float average_ratio_available = available_size / wanted_size;

                float ratio_left = child->size[axis].value;
                if (child->size[axis].value > average_ratio_available) {
                    float min_ratio_left = 0;
                    ratio_left = UI__CLAMP(min_ratio_left, average_ratio_available, child->size[axis].value);
                }

                float wanted = child->rectangle.size[axis];
                float got = parent_size * ratio_left;

                wanted_size -= wanted;
                available_size -= got;
                child->rectangle.size[axis] = got;
            }
        }
    }

    for (ui_Box *child = box->first_child; child != 0; child = child->next_sibling) {
        ui__layout_solve_violations(ui, child);
    }
}

UI_FUNCTION void ui_end(ui_State *ui) {
    ui__layout_standalone(ui, ui->root);
    ui__layout_dependent_ancestor(ui, ui->root);
    ui__layout_dependent_descendant(ui, ui->root);
    ui__layout_solve_violations(ui, ui->root);
    ui__layout_relative_positions_and_rectangle(ui, ui->root);
}

UI_FUNCTION ui_Box *ui_push(ui_State *ui, ui_Box *parent, ui_Flags flags, const char *format, ...) {
    ui_va_list arguments;
    ui_va_start(arguments, format);
    ui_Box *box = ui_pushv(ui, parent, flags, format, arguments);
    ui_va_end(arguments);
    return box;
}

UI_FUNCTION ui_Box *ui_push_styled(ui_State *ui, ui_Box *parent, ui_Flags flags, const ui_Style *style, const char *format, ...) {
    ui_va_list arguments;
    ui_va_start(arguments, format);
    ui_Box *box = ui_pushv(ui, parent, flags, format, arguments);
    ui_va_end(arguments);

    UI_ASSERT(style != 0);
    box->style = *style;

    return box;
}

UI_FUNCTION ui_Box *ui_pushv(ui_State *ui, ui_Box *parent, ui_Flags flags, const char *format_c, ui_va_list arguments) {
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

        // before `##` displayed and hashed; after only hashed
        hash_right_start += 2 * has_2_hash_marker;

        // before `###` only displayed; after only hashed
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
            // djb2
            unsigned hash = 5381;
            for (int i = 0; i < hash_string_length; i += 1) {
                hash += (hash << 5) + (unsigned)(hash_string[i]);
            }

            unsigned exponent = UI_MAX_KEYED_BOXES_EXPONENT;
            unsigned mask = ((unsigned)1 << exponent) - 1;
            unsigned step = (hash >> (32 - exponent)) | 1;
            for (unsigned index = hash;;) {
                index = (index + step) & mask;
                index += !index; // never 0

                ui_Box *this_box = &ui->boxes[index];

                _Bool this_box_is_unused = this_box->hash_string[0] == 0;
                if (this_box_is_unused) {
                    ui->keyed_box_count += 1;
                    int hashmap_max_load = 70;
                    int max_used_boxes = UI_MAX_KEYED_BOXES * hashmap_max_load / 100;
                    _Bool hashmap_saturated = ui->keyed_box_count > max_used_boxes;
                    UI_ASSERT(!hashmap_saturated);

                    box_is_new = 1;
                    box = this_box;
                    break;
                }

                _Bool equals = 1;
                {
                    const char *a = this_box->hash_string;
                    const char *b = hash_string;

                    if (a[0] == 0 || b[0] == 0) equals = 0;
                    else while (*a != 0 && *a == *b) {
                        a += 1;
                        b += 1;
                    }
                    equals = (*a == *b);
                }

                if (equals) {
                    _Bool already_used_this_frame = this_box->frame_id == ui->frame_id;
                    UI_ASSERT(!already_used_this_frame);
                    box = this_box;
                    break;
                }
            }

            if (box_is_new) {
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

    _Bool was_dragging = !box_is_new && !!(box->flags & ui_Flag_DRAGGING);

    box->flags = flags;
    box->flags |= was_dragging * ui_Flag_DRAGGING;

    { // zero frame data, keep cached data
        box->display_string = 0;
        box->size[ui_X] = (ui_Size){0};
        box->size[ui_Y] = (ui_Size){0};
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
            box->size[axis].kind = ui_Size_PIXELS;
            box->size[axis].value = ui->window_size[axis];
        }

        ui->root = box;
    }

    if (box != ui->root) {
        ui_Flags child_layout_axis = box->flags & ui_Flag_CHILD_AXIS;
        box->size[child_layout_axis].kind = ui_Size_SUM_OF_CHILDREN;
        box->size[!child_layout_axis].kind = ui_Size_LARGEST_CHILD;
    }

    box->display_string = display_string;

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

    if (!ui->input_blocked && (box->flags & UI_FLAG_ANY_INTERACTABLE)) {
        _Bool mouse_over = 1;
        {
            float left = box->rectangle.position[ui_X];
            float top = box->rectangle.position[ui_Y];
            float right = left + box->rectangle.size[ui_X];
            float bottom = top + box->rectangle.size[ui_Y];

            float x = ui->mouse_position[ui_X];
            float y = ui->mouse_position[ui_Y];

            mouse_over = mouse_over && left <= x && x < right;
            mouse_over = mouse_over && top <= y && y < bottom;
        }

        box->flags |= ui_Flag_HOVERED * (mouse_over && (box->flags & ui_Flag_HOVERABLE));
        box->flags |= ui_Flag_CLICKED * (mouse_over && (box->flags & ui_Flag_CLICKABLE) && ui->mouse_clicked[ui_Mouse_Button_LEFT]);

        if (box->flags & ui_Flag_DRAGGABLE) {
            _Bool start_drag = !was_dragging && mouse_over && ui->mouse_down[ui_Mouse_Button_LEFT] && !ui->mouse_clicked[ui_Mouse_Button_LEFT];
            if (start_drag) {
                box->flags |= ui_Flag_DRAGGING;
                box->drag_start_mouse[ui_X] = ui->mouse_position[ui_X];
                box->drag_start_mouse[ui_Y] = ui->mouse_position[ui_Y];
            }

            _Bool end_drag = was_dragging && !ui->mouse_down[ui_Mouse_Button_LEFT];
            if (end_drag) {
                box->flags &= ~ui_Flag_DRAGGING;
                box->drag_start_mouse[ui_X] = 0;
                box->drag_start_mouse[ui_Y] = 0;
            }
        }

        ui->interacted_this_frame |= !!(box->flags & UI_FLAG_ANY_INTERACTION);
    }

    box->frame_id = ui->frame_id;
    return box;
}

UI_FUNCTION ui_Box *ui_spacer(ui_State *ui, ui_Box *parent) {
    ui_Box *box = ui_push(ui, parent, 0, 0, 0);
    box->size[ui_X] = box->size[ui_Y] = (ui_Size){ .kind = ui_Size_OF_PARENT, .value = 1.f };
    return box;
}

// TODO(felix): Simplify! Maybe remove.
UI_FUNCTION ui_Box *ui_slider(ui_State *ui, ui_Box *parent, ui_Flags slider_axis, float least, float most, float *value, ui_Slider_Style style, const char *format, ...) {
    ui_Box *slider_box = 0;

    float range = most - least;
    UI_ASSERT(range > 0);

    ui_Style slider_style = style.bar;
    slider_style.padding[ui_X] = 0;
    slider_style.padding[ui_Y] = 0;
    slider_style.child_gap = 0;
    slider_style.border_radius = style.radius;

    const char *formatted = 0;

    ui_va_list arguments;
    ui_va_start(arguments, format);
    {
        char format_buffer[UI_MAX_STRING_LENGTH];

        int written = UI_VSNPRINTF(format_buffer, sizeof format_buffer, format, arguments);
        UI_ASSERT(written < (int)(sizeof format_buffer));

        formatted = format_buffer;
    }
    ui_va_end(arguments);

    slider_box = ui_push_styled(ui, parent, slider_axis | ui_Flag_CLICKABLE | ui_Flag_DRAGGABLE, &slider_style, "%s##slider box", formatted);
    {
        if (slider_box->flags & ui_Flag_CLICKED) slider_box->drag_start_value = *value;

        if (slider_box->flags & ui_Flag_DRAGGING) {
            float mouse = ui->mouse_position[slider_axis];

            float delta = mouse - slider_box->drag_start_mouse[slider_axis];
            float difference = delta * range / style.length;

            *value = slider_box->drag_start_value + difference;
            if (*value < least) *value = least;
            if (*value > most) *value = most;
        }

        float travel = style.length - style.handle_length;
        float first_part_length = (*value - least) * travel / range;
        float last_part_length  = travel - first_part_length;

        slider_box->size[slider_axis] = (ui_Size){ .kind = ui_Size_PIXELS, .value = style.length };
        slider_box->size[!slider_axis] = (ui_Size){ .kind = ui_Size_OF_PARENT, .value = 1.f };

        ui_Flags slider_flags = ui_Flag_DRAW_BORDER | ui_Flag_DRAW_BACKGROUND;

        if (first_part_length >= 1.f) {
            ui_Box *first_part = ui_push_styled(ui, slider_box, !slider_axis, &slider_style, 0);
            first_part->size[slider_axis] = (ui_Size){ .kind = ui_Size_PIXELS, .value = first_part_length };
            first_part->size[!slider_axis] = (ui_Size){ .kind = ui_Size_OF_PARENT, .value = 1.f };

            ui_spacer(ui, first_part);

            ui_Box *first = ui_push_styled(ui, first_part, slider_flags, &slider_style, "%s##slider first", formatted);
            first->size[slider_axis] = (ui_Size){ .kind = ui_Size_OF_PARENT, .value = 1.f };
            first->size[!slider_axis] = (ui_Size){ .kind = ui_Size_PIXELS, .value = style.thickness };

            ui_spacer(ui, first_part);
        }

        {
            ui_Box *handle_part = ui_push_styled(ui, slider_box, !slider_axis, &slider_style, 0);
            handle_part->size[slider_axis] = (ui_Size){ .kind = ui_Size_PIXELS, .value = style.handle_length };
            handle_part->size[!slider_axis] = (ui_Size){ .kind = ui_Size_OF_PARENT, .value = 1.f };

            ui_spacer(ui, handle_part);

            ui_Style handle_style = slider_style;
            handle_style.border_radius = style.handle_radius;
            ui_Flags handle_flags = ui_Flag_DRAW_BORDER | ui_Flag_DRAW_BACKGROUND | ui_Flag_DRAW_SHADOW;
            ui_Box *handle = ui_push_styled(ui, handle_part, handle_flags, &handle_style, "%s##slider handle", formatted);
            if (slider_box->flags & ui_Flag_DRAGGING) {
                handle->style.bg_color = style.dragging.bg_color;
                handle->style.border_color = style.dragging.border_color;
            }
            handle->size[slider_axis] = (ui_Size){ .kind = ui_Size_OF_PARENT, .value = 1.f };
            handle->size[!slider_axis] = (ui_Size){ .kind = ui_Size_PIXELS, .value = style.handle_thickness };

            ui_spacer(ui, handle_part);
        }

        if (last_part_length >= 1.f) {
            ui_Box *last_part = ui_push_styled(ui, slider_box, !slider_axis, &slider_style, 0);
            last_part->size[slider_axis] = (ui_Size){ .kind = ui_Size_PIXELS, .value = last_part_length };
            last_part->size[!slider_axis] = (ui_Size){ .kind = ui_Size_OF_PARENT, .value = 1.f };

            ui_spacer(ui, last_part);

            ui_Box *last = ui_push_styled(ui, last_part, slider_flags, &slider_style, "%s##slider last", formatted);
            last->size[slider_axis] = (ui_Size){ .kind = ui_Size_OF_PARENT, .value = 1.f };
            last->size[!slider_axis] = (ui_Size){ .kind = ui_Size_PIXELS, .value = style.thickness };

            ui_spacer(ui, last_part);
        }
    }

    return slider_box;
}


#endif // UI_IMPLEMENTATION
