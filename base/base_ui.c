#if defined(BASE_NO_IMPLEMENTATION) || defined(BASE_NO_IMPLEMENTATION_UI)

enumdef(Ui_Axis, u8) { Ui_Axis_X, Ui_Axis_Y, Ui_Axis_COUNT };

enumdef(Ui_Box_Flags, u8) {
    Ui_Box_Flag_CHILD_AXIS = 1 << 0,
    Ui_Box_Flag_DRAW_BACKGROUND = 1 << 1,
    Ui_Box_Flag_DRAW_BORDER = 1 << 2,
    Ui_Box_Flag_DRAW_TEXT = 1 << 3,
    Ui_Box_Flag_ANIMATE = 1 << 4,
    Ui_Box_Flag_HOVERABLE = 1 << 5,
    Ui_Box_Flag_CLICKABLE = 1 << 6,
    Ui_Box_Flag__FIRST_FRAME = 1 << 7,
};
#define Ui_Box_Flag_ANY_VISIBLE (Ui_Box_Flag_DRAW_BACKGROUND | Ui_Box_Flag_DRAW_BORDER | Ui_Box_Flag_DRAW_TEXT)

structdef(Ui_Box_Rectangle) {
    using(V2, top_left);
    V2 size;
};

enumdef(Ui_Size_Kind, u8) {
    Ui_Size_Kind_NIL = 0,
    Ui_Size_Kind_TEXT,
    Ui_Size_Kind_SUM_OF_CHILDREN,
    Ui_Size_Kind_LARGEST_CHILD,
};

enumdef(Ui_Color, u8) {
    Ui_Color_BACKGROUND,
    Ui_Color_FOREGROUND,
    Ui_Color_BORDER,
    Ui_Color_COUNT,
};

structdef(Ui_Box_Style) {
    f32 font_size;
    V2 inner_padding;
    V2 child_gap;
    V4 color[Ui_Color_COUNT];
    f32 border_width;
    f32 border_radius;
    f32 animation_speed;
};

enumdef(Ui_Box_Style_Kind, u8) {
    Ui_Box_Style_Kind_INACTIVE,
    Ui_Box_Style_Kind_HOVERED,
    Ui_Box_Style_Kind_CLICKED,
    Ui_Box_Style_Kind_COUNT,
};

structdef(Ui_Box_Style_Set) {
    Ui_Box_Style kinds[Ui_Box_Style_Kind_COUNT];
};

typedef u32 Ui_Box_Id;
define_container_types(Ui_Box_Id)

structdef(Ui_Box) {
    using(struct {
        Ui_Box_Flags flags;
        String display_string;
        Ui_Size_Kind size_kind[Ui_Axis_COUNT];
        using(struct {
            Ui_Box *parent;
            Ui_Box *previous_sibling;
            Ui_Box *next_sibling;
            Ui_Box *first_child;
            Ui_Box *last_child;
        }, links);
    }, frame_data);

    using(struct {
        using(struct {
            bool hovered;
            bool clicked;
        }, interaction);
        Ui_Box_Rectangle target_rectangle;
        Ui_Box_Style_Set target_style_set;
    }, frame_data_computed);

    using(struct {
        String hash_string;
    }, cached_data);

    using(struct {
        Ui_Box_Rectangle display_rectangle;
        Ui_Box_Style display_style;
        Ui_Box_Style_Kind target_style_kind;
    }, cached_data_computed);
};

typedef Ui_Box *Ui_Box_Pointer;
define_container_types(Ui_Box_Pointer)

structdef(Ui) {
    Platform *platform;

    u64 used_box_count;
    Ui_Box *boxes;

    f32 scale;

    Ui_Box *root;
    Ui_Box *current_parent;

    Array_Ui_Box_Style_Set style_stack;

    Array_Ui_Box_Pointer boxes_to_render;
};

static void ui_begin(Ui *ui);

#define ui_button(ui, format, ...) ui_button_((ui), (format), formats_from_va_args(__VA_ARGS__))
static Ui_Box *ui_button_(Ui *ui, char *format, Slice_Format arguments);

static void ui_create(Ui *ui);
static void ui_default_render_passthrough(Ui *ui);
static void ui_end(Ui *ui);
static void ui_pop_parent(Ui *ui);

#define     ui_style(ui) ui_defer_loop(ui_push_style(ui), ui_pop_style(ui))
static             void  ui_pop_style(Ui *ui);
static Ui_Box_Style_Set *ui_push_style(Ui *ui);

structdef(Ui_Push_Arguments) {
    Ui *ui;
    char *format;
    Slice_Format arguments;
    bool parent;
    Ui_Box_Flags flags;
};
#define ui_push(...) ui_push_((Ui_Push_Arguments){ __VA_ARGS__ })
static Ui_Box *ui_push_(Ui_Push_Arguments arguments);

#define ui_text(ui, format, ...) ui_text_((ui), (format), formats_from_va_args(__VA_ARGS__))
static Ui_Box *ui_text_(Ui *ui, char *format, Slice_Format arguments);

#define ui_expand_index(line) i__##line##__
#define ui_defer_loop_index(line) ui_expand_index(line)
#define ui_defer_loop(begin, end) \
    for ( \
        bool ui_defer_loop_index(__LINE__) = ((begin), false); \
        !ui_defer_loop_index(__LINE__); \
        ui_defer_loop_index(__LINE__) += 1, (end) \
    )
#define ui_parent(ui, axis) ui_defer_loop(ui_push((ui), .parent = true, .flags = axis), ui_pop_parent(ui))


#else // implementation


#define for_ui_axis(variable) for (Ui_Axis variable = 0; variable < Ui_Axis_COUNT; variable += 1)

static void ui_begin(Ui *ui) {
    ui->root = 0;
    ui->current_parent = 0;
    ui->style_stack.count = 0;
    ui->boxes_to_render.count = 0;

    ui->scale = ui->platform->dpi_scale;

    Ui_Box_Style_Set default_style = {
        .kinds = {
            [Ui_Box_Style_Kind_INACTIVE] = { .color = {
                [Ui_Color_BACKGROUND] = rgba_from_hex(0xd8d8d8ff),
                [Ui_Color_FOREGROUND] = rgba_from_hex(0x000000ff),
                [Ui_Color_BORDER] = rgba_from_hex(0x6d6d6dff),
            } },
            [Ui_Box_Style_Kind_HOVERED] = { .color = {
                [Ui_Color_BACKGROUND] = rgba_from_hex(0x9b9b9bff),
                [Ui_Color_FOREGROUND] = rgba_from_hex(0x000000ff),
                [Ui_Color_BORDER] = rgba_from_hex(0x515151ff),
            } },
            [Ui_Box_Style_Kind_CLICKED] = { .color = {
                [Ui_Color_BACKGROUND] = rgba_from_hex(0x000000ff),
                [Ui_Color_FOREGROUND] = rgba_from_hex(0x000000ff),
                [Ui_Color_BORDER] = rgba_from_hex(0xffffffff),
            } },
        }
    };
	for (Ui_Box_Style_Kind kind = 0; kind < Ui_Box_Style_Kind_COUNT; kind += 1) {
		Ui_Box_Style *style = &default_style.kinds[kind];

        style->font_size = 14.f;
		style->inner_padding = (V2){ .x = 5.f, .y = 3.f };
		style->child_gap = (V2){ .x = 3.f, .y = 3.f };
		style->border_width = 1.f;
		style->border_radius = 5.f;
		style->animation_speed = 20.f;
	}
    ui->style_stack = (Array_Ui_Box_Style_Set){ .arena = ui->platform->frame_arena };
    push(&ui->style_stack, default_style);
}

static Ui_Box *ui_button_(Ui *ui, char *format, Slice_Format arguments) {
    Ui_Box_Flags flags = Ui_Box_Flag_DRAW_TEXT | Ui_Box_Flag_DRAW_BACKGROUND | Ui_Box_Flag_DRAW_BORDER | Ui_Box_Flag_HOVERABLE | Ui_Box_Flag_CLICKABLE | Ui_Box_Flag_ANIMATE;
    Ui_Box *box = ui_push(ui, format, .arguments = arguments, .flags = flags);
    for_ui_axis (axis) box->size_kind[axis] = Ui_Size_Kind_TEXT;
    return box;
}

#define UI_MAX_BOX_COUNT 512
static void ui_create(Ui *ui) {
    ui->boxes = arena_make(ui->platform->persistent_arena, UI_MAX_BOX_COUNT * 2, sizeof(*ui->boxes));

    ui->boxes_to_render.arena = ui->platform->persistent_arena;
    reserve(&ui->boxes_to_render, UI_MAX_BOX_COUNT);
}

static void ui_default_render_passthrough(Ui *ui) {
    for_slice (Ui_Box **, box_double_pointer, ui->boxes_to_render) {
        Ui_Box *box = *box_double_pointer;

        Ui_Box_Style style = box->display_style;
        V4 background_color = style.color[Ui_Color_BACKGROUND];
        V4 border_color = style.color[Ui_Color_BORDER];
        V4 foreground_color = style.color[Ui_Color_FOREGROUND];
        f32 border_width = style.border_width * ui->scale;
        f32 border_radius = style.border_radius * ui->scale;

        bool draw_rectangle = !!(box->flags & (Ui_Box_Flag_DRAW_BORDER | Ui_Box_Flag_DRAW_BACKGROUND));
        if (draw_rectangle) {
            Draw_Command command = {
                .kind = Draw_Kind_RECTANGLE,
                .position = box->display_rectangle.top_left,
                .color[Draw_Color_SOLID] = background_color,
                .rectangle = {
                    .size = box->display_rectangle.size,
                    .border_color = border_color,
                    .border_width = border_width,
                    .border_radius = border_radius,
                },
            };
            draw_(ui->platform, command);
        }

        bool draw_text = !!(box->flags & Ui_Box_Flag_DRAW_TEXT);
        if (draw_text) {
            assert(box->display_string.count > 0);

            V2 position = box->display_rectangle.top_left;
            for_ui_axis (axis) {
                f32 add = box->display_style.inner_padding.items[axis];
                add += !!(box->flags & Ui_Box_Flag_DRAW_BORDER) * box->display_style.border_width;
                add *= ui->scale;
                position.items[axis] += add;
            }

            Draw_Command command = {
                .kind = Draw_Kind_TEXT,
                .position = position,
                .color[Draw_Color_SOLID] = foreground_color,
                .text = {
                    .string = box->display_string,
                    .font_size = box->display_style.font_size * ui->scale,
                },
            };
            draw_(ui->platform, command);
        }
    }
}

static void ui_layout_standalone(Ui *ui, Ui_Box *box) {
    Ui_Box_Style_Kind old_style_kind = box->target_style_kind;
    if ((box->flags & Ui_Box_Flag_CLICKABLE) && box->clicked) box->target_style_kind = Ui_Box_Style_Kind_CLICKED;
    else if ((box->flags & Ui_Box_Flag_HOVERABLE) && box->hovered) box->target_style_kind = Ui_Box_Style_Kind_HOVERED;
    else box->target_style_kind = Ui_Box_Style_Kind_INACTIVE;

    Ui_Box_Style *target_style = &box->target_style_set.kinds[box->target_style_kind];
    Ui_Box_Style *style = &box->display_style;

    bool lerp_style = box->target_style_kind <= old_style_kind;
    lerp_style = lerp_style && !(box->flags & Ui_Box_Flag__FIRST_FRAME);
    lerp_style = lerp_style && !!(box->flags & Ui_Box_Flag_ANIMATE);
    if (lerp_style) {
        f32 dt = ui->platform->seconds_since_last_frame;
        style->animation_speed = target_style->animation_speed;

        style->font_size = stable_lerp(style->font_size, target_style->font_size, style->animation_speed, dt);

        style->inner_padding = v2_stable_lerp(style->inner_padding, target_style->inner_padding, style->animation_speed, dt);
        style->child_gap = v2_stable_lerp(style->child_gap, target_style->child_gap, style->animation_speed, dt);

        for (Ui_Color color = 0; color < Ui_Color_COUNT; color += 1) {
            V4 *current = &style->color[color];
            V4 *target = &target_style->color[color];
            *current = v4_stable_lerp(*current, *target, style->animation_speed, dt);
        }

        style->border_width = stable_lerp(style->border_width, target_style->border_width, style->animation_speed, dt);
        style->border_radius = stable_lerp(style->border_radius, target_style->border_radius, style->animation_speed, dt);
    } else *style = *target_style;

    for_ui_axis (axis) {
		box->target_rectangle.size.items[axis] += ui->scale * 2.f * style->border_width * !!(box->flags & Ui_Box_Flag_DRAW_BORDER);
		box->target_rectangle.size.items[axis] += ui->scale * 2.f * style->inner_padding.items[axis];

        switch (box->size_kind[axis]) {
            case Ui_Size_Kind_NIL: unreachable;
            default: break;
        }
    }

    for (Ui_Box *child = box->first_child; child != 0; child = child->next_sibling) {
        ui_layout_standalone(ui, child);
    }
}

static void ui_layout_dependent_descendant(Ui *ui, Ui_Box *box) {
    for (Ui_Box *child = box->first_child; child != 0; child = child->next_sibling) {
        ui_layout_dependent_descendant(ui, child);
    }

    for_ui_axis (axis) switch (box->size_kind[axis]) {
        case Ui_Size_Kind_TEXT: break;
        case Ui_Size_Kind_SUM_OF_CHILDREN: {
            for (Ui_Box *child = box->first_child; child != 0; child = child->next_sibling) {
                box->target_rectangle.size.items[axis] += child->target_rectangle.size.items[axis];
				box->target_rectangle.size.items[axis] += ui->scale * box->display_style.child_gap.items[axis];
            }

            bool has_at_least_one_child = box->first_child != 0;
            box->target_rectangle.size.items[axis] -= has_at_least_one_child * ui->scale * box->display_style.child_gap.items[axis];
        } break;
        case Ui_Size_Kind_LARGEST_CHILD: {
            f32 largest = 0;
            for (Ui_Box *child = box->first_child; child != 0; child = child->next_sibling) {
                f32 size = child->target_rectangle.size.items[axis];
                largest = max(largest, size);
            }
            box->target_rectangle.size.items[axis] += largest;
        } break;
        default: unreachable;
    }

    assert(!v2_equal(box->target_rectangle.size, (V2){0}));
}

static void ui_layout_relative_positions_and_rectangle(Ui *ui, Ui_Box *box) {
    if (box->parent != 0) {
        Ui_Axis layout_axis = cast(Ui_Axis) box->parent->flags & Ui_Box_Flag_CHILD_AXIS;

        if (box->previous_sibling != 0) {
            box->target_rectangle.top_left.items[layout_axis] = box->previous_sibling->target_rectangle.top_left.items[layout_axis];
            box->target_rectangle.top_left.items[layout_axis] += box->previous_sibling->target_rectangle.size.items[layout_axis];
            box->target_rectangle.top_left.items[layout_axis] += ui->scale * box->parent->display_style.child_gap.items[layout_axis];
        } else {
            box->target_rectangle.top_left.items[layout_axis] = box->parent->target_rectangle.top_left.items[layout_axis];
			box->target_rectangle.top_left.items[layout_axis] += ui->scale * box->parent->display_style.inner_padding.items[layout_axis];
			box->target_rectangle.top_left.items[layout_axis] += !!(box->parent->flags & Ui_Box_Flag_DRAW_BORDER) * box->parent->display_style.border_width * ui->scale;
        }

        Ui_Axis non_layout_axis = !layout_axis;
        box->target_rectangle.top_left.items[non_layout_axis] = box->parent->target_rectangle.top_left.items[non_layout_axis];
        box->target_rectangle.top_left.items[non_layout_axis] += ui->scale * box->parent->display_style.inner_padding.items[non_layout_axis];
        box->target_rectangle.top_left.items[non_layout_axis] += !!(box->parent->flags & Ui_Box_Flag_DRAW_BORDER) * box->parent->display_style.border_width * ui->scale;
    }

    bool animate = !!(box->flags & Ui_Box_Flag_ANIMATE);
    animate = animate && !(box->flags & Ui_Box_Flag__FIRST_FRAME);
    if (animate) {
        V4 display = v4_stable_lerp(bit_cast(V4) box->display_rectangle, bit_cast(V4) box->target_rectangle, box->display_style.animation_speed, ui->platform->seconds_since_last_frame);
        box->display_rectangle = bit_cast(Ui_Box_Rectangle) display;
    } else {
        box->display_rectangle = box->target_rectangle;
    }

    if (box->flags & Ui_Box_Flag_ANY_VISIBLE) {
        Ui_Box_Rectangle display = box->display_rectangle;
        Ui_Box_Rectangle target = box->target_rectangle;
        assert(display.top_left.x >= 0);
        assert(display.top_left.y >= 0);
        assert(target.top_left.x >= 0);
        assert(target.top_left.y >= 0);
        assert(display.size.x >= 0);
        assert(display.size.y >= 0);
        assert(target.size.x > 0);
        assert(target.size.y > 0);

        push_assume_capacity(&ui->boxes_to_render, box);
    }

    for (Ui_Box *child = box->first_child; child != 0; child = child->next_sibling) {
        ui_layout_relative_positions_and_rectangle(ui, child);
    }
}

static void ui_end(Ui *ui) {
    ui_layout_standalone(ui, ui->root);
    // ui_layout_dependent_ancestor(ui, ui->root); // TODO(felix)
    ui_layout_dependent_descendant(ui, ui->root);
    // ui_layout_solve_violations(ui, ui->root); // TODO(felix)
    ui_layout_relative_positions_and_rectangle(ui, ui->root);
}

static void ui_pop_parent(Ui *ui) {
    ui->current_parent = ui->current_parent->parent;
}

static void ui_pop_style(Ui *ui) {
    pop(&ui->style_stack);
}

static Ui_Box_Style_Set *ui_push_style(Ui *ui) {
    Ui_Box_Style_Set top = *slice_get_last(ui->style_stack);
    push(&ui->style_stack, top);
    return slice_get_last(ui->style_stack);
}

static Ui_Box *ui_push_(Ui_Push_Arguments arguments) {
    Ui *ui = arguments.ui;

    bool keyed = arguments.format != 0;

    String hash_string = {0};
    String display_string = {0};

    Ui_Box *box = 0;
    bool box_is_new = !keyed;
    if (!keyed) {
        box = arena_make(ui->platform->frame_arena, 1, sizeof(Ui_Box));
    } else {
        static Arena temporary = {0};
        if (temporary.capacity == 0) temporary = arena_init(8096);
        Scratch scratch = scratch_begin(&temporary);

        String format = string_print_(scratch.arena, arguments.format, arguments.arguments);

        bool has_2_hash_marker = false;
        bool has_3_hash_marker = false;

        u64 marker_start_index = 0;
        for (u64 i = 0; i < format.count; i += 1) {
            if (format.data[i] != '#') continue;

            if (i + 1 == format.count) break;
            if (format.data[i + 1] != '#') continue;

            has_3_hash_marker = (i + 2 < format.count) && format.data[i + 2] == '#';
            has_2_hash_marker = !has_3_hash_marker;

            marker_start_index = i;
            break;
        }

        u64 display_and_first_hash_part_end_index = marker_start_index;
        u64 second_hash_part_start_index = marker_start_index;

        // before `##` displayed and hashed; after only hashed
        second_hash_part_start_index += 2 * has_2_hash_marker;

        // before `###` only displayed; after only hashed
        u64 first_hash_part_start_index = display_and_first_hash_part_end_index * has_3_hash_marker;
        second_hash_part_start_index += 3 * has_3_hash_marker;

        String first_hash_part = string_range(format, first_hash_part_start_index, display_and_first_hash_part_end_index);
        String second_hash_part = string_range(format, second_hash_part_start_index, format.count);

        bool has_marker = has_2_hash_marker || has_3_hash_marker;
        u64 display_part_end_index = has_marker * display_and_first_hash_part_end_index + !has_marker * format.count;
        String display_part = string_range(format, 0, display_part_end_index);

        hash_string = string_print(scratch.arena, "%%", fmt(String, first_hash_part), fmt(String, second_hash_part));
        display_string = arena_push(ui->platform->frame_arena, display_part);

        u64 hash = hash_djb2(hash_string);
        u64 exponent = count_trailing_zeroes(UI_MAX_BOX_COUNT * 2);
        for (u64 index = hash;;) {
            index = hash_lookup_msi(hash, exponent, index);
            index += !index; // never 0
            Ui_Box *this_box = &ui->boxes[index];

            if (this_box->hash_string.count == 0) {
                ui->used_box_count += 1;
                u64 hashmap_max_load = 70;
                bool hashmap_saturated = ui->used_box_count * hashmap_max_load / 100 > UI_MAX_BOX_COUNT * 2;
                assert(!hashmap_saturated);

                box_is_new = true;
                box = this_box;
                break;
            }

            if (string_equals(this_box->hash_string, hash_string)) {
                box = this_box;
                // NOTE(felix): debug check for same hash for different boxes in same frame can go here
                break;
            }
        }

        if (box_is_new) box->hash_string = arena_push(ui->platform->persistent_arena, hash_string);

        scratch_end(scratch);
    }

    zero(&box->frame_data);
    zero(&box->frame_data_computed);

    box->parent = ui->current_parent;
    if (ui->root == 0) {
        assert(ui->current_parent == 0);
        assert(arguments.parent);
        ui->root = box;
        ui->current_parent = box;
    }

    box->flags = arguments.flags;
    box->flags |= Ui_Box_Flag__FIRST_FRAME * box_is_new;
    {
        Ui_Axis child_layout_axis = !!(box->flags & Ui_Box_Flag_CHILD_AXIS);
        box->size_kind[child_layout_axis] = Ui_Size_Kind_SUM_OF_CHILDREN;
        box->size_kind[!child_layout_axis] = Ui_Size_Kind_LARGEST_CHILD;
    }

    box->target_style_set = *slice_get_last(ui->style_stack);
    if (box_is_new) box->display_style = box->target_style_set.kinds[Ui_Box_Style_Kind_INACTIVE];

    box->display_string = display_string;
    if (display_string.count > 0) {
        f32 font_size = box->display_style.font_size * ui->scale;
        box->target_rectangle.size = platform_measure_text(ui->platform, box->display_string, font_size);
        assert(!v2_equal(box->target_rectangle.size, (V2){0}));
    }

    if (box->parent != 0) {
        if (box->parent->first_child == 0) {
            assert(box->parent->last_child == 0);
            box->parent->first_child = box;
            box->parent->last_child = box;
        } else {
            assert(box->parent->last_child != 0);
            box->parent->last_child->next_sibling = box;
            box->previous_sibling = box->parent->last_child;
            box->parent->last_child = box;
        }
    }

    if (arguments.parent) ui->current_parent = box;

    box->target_style_set = *slice_get_last(ui->style_stack);

    V4 rectangle = {
        .top_left = box->display_rectangle.top_left,
        .bottom_right = v2_add(box->display_rectangle.top_left, box->display_rectangle.size),
    };
    box->hovered = !!(box->flags & Ui_Box_Flag_HOVERABLE) && intersect_point_in_rectangle(ui->platform->mouse_position, rectangle);
    box->clicked = box->hovered && !!(box->flags & Ui_Box_Flag_CLICKABLE) && ui->platform->mouse_clicked[App_Mouse_Button_LEFT];

    return box;
}

static Ui_Box *ui_text_(Ui *ui, char *format, Slice_Format arguments) {
    Ui_Box *box = ui_push(ui, format, .arguments = arguments, .flags = Ui_Box_Flag_DRAW_TEXT | Ui_Box_Flag_ANIMATE);
    for_ui_axis (axis) box->size_kind[axis] = Ui_Size_Kind_TEXT;
    return box;
}


#endif // defined(BASE_NO_IMPLEMENTATION) || defined(BASE_NO_IMPLEMENTATION_UI)


// #if defined(BASE_NO_IMPLEMENTATION) || defined(BASE_NO_IMPLEMENTATION_UI)

// enumdef(UI_Axis, u8) { ui_axis_x, ui_axis_y, ui_axis_count };
// #define for_ui_axis(axis_var_name) for (UI_Axis axis_var_name = 0; axis_var_name < ui_axis_count; axis_var_name += 1)

// structdef(UI_Interaction) { bool hovered, clicked; };

// enumdef(UI_Size_Kind, u8) {
//     ui_size_kind_nil,
//     ui_size_kind_text,
//     ui_size_kind_sum_of_children,
//     ui_size_kind_largest_child,
//     ui_size_kind_pixels,
// };

// typedef V4 UI_Rect;

// structdef(UI_Size) {
//     UI_Size_Kind kind;
//     f32 value, strictness;
// };

// // TODO(felix): add flags for text alignment
// enumdef(UI_Box_Flags, u8) {
//     ui_box_flag_child_axis      = 1 << 0,
//     ui_box_flag_clickable       = 1 << 1,
//     ui_box_flag_draw_text       = 1 << 2,
//     ui_box_flag_draw_border     = 1 << 3,
//     ui_box_flag_draw_background = 1 << 4,
//     ui_box_flag_draw_texture    = 1 << 5,
// };

// structdef(UI_Box_Build) {
//     String hash_string, display_string;
//     UI_Box_Flags flags;
//     UI_Size size[ui_axis_count];
// };

// structdef(UI_Box_Style) {
//     V2 pad, margin;
//     V4 clr_fg, clr_bg, clr_border;
// };

// structdef(UI_Box) {
//     UI_Box_Build build;
//     u64 key;

//     UI_Box_Style style, target_style;

//     UI_Box *parent;
//     UI_Box *prev, *next;
//     UI_Box *first, *last;

//     f32 computed_size_px[ui_axis_count];
//     f32 pos_rel_parent[ui_axis_count];
//     UI_Rect rect, target_rect;

//     UI_Interaction interaction;
// };
// typedef UI_Box *UI_Box_Pointer;
// define_container_types(UI_Box_Pointer)

// // TODO(felix): actually use properly
// structdef(UI_Style_Stack) {
//     Array_V2 pad, margin;
//     Array_V4 clr_fg, clr_bg, clr_border;
// };

// structdef(UI) {
//     Arena *persistent_arena, *frame_arena;
//     Gfx_Render_Context *gfx;
//     f32 pos[ui_axis_count];

//     UI_Box *root;
//     UI_Box *current_parent;
//     Array_UI_Box_Pointer box_hashmap;

//     UI_Style_Stack style_stack;
// };

// #define ui_animation_speed 0.4f
// #define ui_box_map_size    1024

// #define ui_expand_idx(line) i##line
// #define ui_defer_loop_idx(line) ui_expand_idx(line)
// #define ui_defer_loop(begin, end)\
//     for (\
//         int ui_defer_loop_idx(__LINE__) = ((begin), 0);\
//         ui_defer_loop_idx(__LINE__) != 1;\
//         ui_defer_loop_idx(__LINE__) += 1, (end)\
//     )
// #define ui_parent(ui, box) ui_defer_loop(ui_push_parent(ui, box), ui_pop_parent(ui))

// #define ui_render(ui_pointer) ui_render_recursive((ui_pointer), (ui_pointer)->root)

// static          UI_Box *ui_border_box(UI *ui);

// structdef(UI_Box_Arguments) { UI *ui; String string; bool only_hash; };
// #define ui_box(...) ui_box_((UI_Box_Arguments){ __VA_ARGS__ })
// static UI_Box *ui_box_(UI_Box_Arguments arguments);

// static          UI_Box *ui_box_frame_local_not_keyed(UI *ui);
// static            void  ui_begin_build(UI *ui);
// static            void  ui_end_build(UI *ui);
// static            void  ui_compute_layout_dependent_ancestor(UI *ui, UI_Box *box);
// static            void  ui_compute_layout_dependent_descendant(UI *ui, UI_Box *box);
// static            void  ui_compute_layout_relative_positions_and_rect(UI *ui, UI_Box *box);
// static            void  ui_compute_layout_solve_violations(UI *ui, UI_Box *box);
// static            void  ui_compute_layout_standalone(UI *ui, UI_Box *box);
// static              UI  ui_init(Arena *persistent_arena, Arena *frame_arena, Gfx_Render_Context *gfx);
// static inline     void  ui_pop_parent(UI *ui);
// static  UI_Interaction  ui_push(UI *ui, UI_Box *box);
// static  UI_Interaction  ui_push_parent(UI *ui, UI_Box *box);
// static inline     void  ui_rect_shift_axis(UI_Rect *rect, UI_Axis axis, f32 shift);
// static inline     void  ui_rect_shift_x(UI_Rect *rect, f32 shift);
// static inline     void  ui_rect_shift_y(UI_Rect *rect, f32 shift);
// static            void  ui_render_recursive(UI *ui, UI_Box *box);

// // Helpers for usage code
// static  UI_Interaction  ui_button(UI *ui, char *format, ...);
// static          UI_Box *ui_column(UI *ui);
// static          UI_Box *ui_row(UI *ui);
// static  UI_Interaction  ui_text(UI *ui, char *format, ...);


// #else // IMPLEMENTATION


// // NOTE(felix): How this needs to look:
// // 1. Build box hierarchy. Use last frame's data (looked up by hash) for input
// //  requires hash links { UI_Box *hash_prev, *hash_next }
// //  requires keying and frame info { UI_Key key; u64 last_frame_touched_idx }
// //      a) look up in box hash table to be able to use previous frame's layout to compute input
// //      b) iterate through hash table. if a box's last_frame_touched_idx < current_frame_idx, remove
// // 2. Layout pass (compute sizes)
// //  requires tree links { UI_Box *parent, *first, *last, *prev, *next }
// //      a) calculate standalone sizes, which do not depend on other boxs
// //      b) pre-order: calculate sizes dependent on ancestor sizes
// //      c) post-order: calculate sizes dependent on descendant sizes
// //      d) pre-order: solve violations using strictness value
// //      e) pre-order: using calculated sizes, compute relative positions of each box (lay out on axis specified by parent),
// //         compute final screen coordinate rectangle
// // 3. Render

// static UI_Box *ui_box_map_memory[ui_box_map_size];

// static Array_UI_Box_Pointer ui_box_hashmap = {
//     .data = ui_box_map_memory,
//     .capacity = ui_box_map_size,
// };

// static UI_Box *ui_border_box(UI *ui) {
//     UI_Box *panel = ui_box_frame_local_not_keyed(ui);
//     panel->build.flags = ui_box_flag_draw_border;
//     for_ui_axis (axis) panel->build.size[axis].kind = ui_size_kind_sum_of_children;
//     return panel;
// }

// static UI_Box *ui_box_(UI_Box_Arguments arguments) {
//     UI *ui = arguments.ui;
//     String string = arguments.string;

//     assert(string.count != 0 && string.data != 0);

//     String display_string = string;
//     V2 display_string_size = {0};
//     String hash_string = string;
//     if (arguments.only_hash) {
//         display_string = (String){0};
//         #if BUILD_DEBUG
//             for (u64 i = 0; i < string.count; i += 1) {
//                 if (string.data[i] != '#') continue;
//                 i += 1;
//                 if (i == string.count || string.data[i] != '[') continue;
//                 panic("you called `ui_box` with `.only_hash = true`, but the string `%` has a display/hash specifier at index %", fmt(String, string), fmt(u64, i));
//             }
//         #endif
//     } else {
//         u64 specifier_start_index = 0, specifier_end_index = 0;
//         enum { before_specifier, after_specifier, specifier_position_count };
//         bool should_hash[specifier_position_count] = {0};
//         bool should_display[specifier_position_count] = {0};

//         bool there_is_a_specifier = false;
//         for (u64 i = 0; i < string.count; i += 1) {
//             if (string.data[i] != '#') continue;

//             i += 1;
//             if (i == string.count) break;
//             if (string.data[i] != '[') continue;

//             there_is_a_specifier = true;

//             specifier_start_index = i - 1;
//             u64 position = before_specifier;

//             for (; i < string.count; i += 1) switch (string.data[i]) {
//                 case ']': specifier_end_index = i; goto compute_specifier;
//                 case 'h': should_hash[position] = true; break;
//                 case 'd': should_display[position] = true; break;
//                 case ',': {
//                     position += 1;
//                     if (position != after_specifier) panic("specifier in string '%' has too many commas", fmt(String, string));
//                 } break;
//                 default: break;
//             }
//             panic("specifier in string '%' never closed with ']'", fmt(String, string));
//         }
//         goto done_parsing;

//         compute_specifier: {
//             assert(specifier_end_index != 0 || should_hash[before_specifier] || should_hash[after_specifier]);
//             String string_before_specifier = string_range(string, 0, specifier_start_index);
//             String string_after_specifier = string_range(string, specifier_end_index + 1, string.count);

//             String_Builder hash_string_builder = { .arena = ui->frame_arena };
//             reserve(&hash_string_builder, string.count);
//             if (should_hash[before_specifier]) push_slice_assume_capacity(&hash_string_builder, string_before_specifier);
//             if (should_hash[after_specifier]) push_slice_assume_capacity(&hash_string_builder, string_after_specifier);

//             String_Builder display_string_builder = { .arena = ui->frame_arena };
//             reserve(&display_string_builder, string.count);
//             if (should_display[before_specifier]) push_slice_assume_capacity(&display_string_builder, string_before_specifier);
//             if (should_display[after_specifier]) push_slice_assume_capacity(&display_string_builder, string_after_specifier);

//             hash_string = hash_string_builder.string;
//             display_string = display_string_builder.string;
//         }

//         done_parsing: {
//             // NOTE(felix): there's probably a case to make for allowing empty display strings
//             assert(display_string.count != 0);
//             #if BUILD_DEBUG
//                 if (there_is_a_specifier) for (u64 position = 0; position < specifier_position_count; position += 1) {
//                     assert(should_hash[position] || should_display[position]);
//                 }
//             #else
//                 discard(there_is_a_specifier);
//             #endif

//             // TODO(felix): support newlines and tab characters
//             u8 tracking = 1; // TODO(felix): configurable in style
//             display_string_size = (V2){
//                 .x = (f32)(display_string.count * (ui->gfx->font.width + tracking)),
//                 .y = (f32)(ui->gfx->font.height),
//             };
//         }
//     }

//     assert(hash_string.count != 0);

//     // djb2 hash
//     // TODO(felix): add hashmap to base and use
//     u64 key = 5381;
//     for (u64 i = 0; i < hash_string.count; i += 1) {
//         key = ((key << 5) + key) + hash_string.data[i];
//     }
//     key %= ui->box_hashmap.capacity;

//     for (UI_Box *match = ui->box_hashmap.data[key]; match != 0; match = match->next) {
//         if (!string_equals(hash_string, match->build.hash_string)) continue;
//         // TODO(felix): there are fields we need to initialise here that are currently only initialised when allocating a new widget
//         // TODO(felix): also, I think, fields that need to be zeroed
//         // TODO(felix): need to do some cleanup here to unify control flow
//         match->build.display_string = arena_push(ui->frame_arena, display_string);
//         if (display_string.count != 0) {
//             match->computed_size_px[ui_axis_x] = display_string_size.x + match->style.pad.x * 2.f;
//             match->computed_size_px[ui_axis_y] = display_string_size.y + match->style.pad.y * 2.f;
//         }
//         return match;
//     }

//     // TODO(felix): check freelist in ui_state before allocating new memory. To populate said freelist, we'll need to prune the hashmap every frame.
//     UI_Box *new = arena_make(ui->persistent_arena, 1, sizeof(UI_Box));
//     ui->box_hashmap.data[key] = new;

//     // TODO(felix): I think the new style should be applied even above, in the case where the widget already exists
//     UI_Box_Style target_style = {
//         .pad = *slice_get_last(ui->style_stack.pad),
//         .margin = *slice_get_last(ui->style_stack.margin),
//         .clr_fg = *slice_get_last(ui->style_stack.clr_fg),
//         .clr_bg = *slice_get_last(ui->style_stack.clr_bg),
//         .clr_border = *slice_get_last(ui->style_stack.clr_border),
//     };
//     *new = (UI_Box){
//         .key = key,
//         .target_style = target_style,
//         .computed_size_px = { display_string_size.x + target_style.pad.x * 2.f, display_string_size.y + target_style.pad.y * 2.f },
//     };
//     new->build.hash_string = arena_push(ui->persistent_arena, hash_string);
//     new->build.display_string = arena_push(ui->frame_arena, display_string);
//     return new;
// }

// // TODO(felix): can take from freelist here also once implemented
// static UI_Box *ui_box_frame_local_not_keyed(UI *ui) {
//     UI_Box *box = arena_make(ui->frame_arena, 1, sizeof(UI_Box));
//     return box;
// }

// // NOTE(felix): Since a root node is guaranteed before user UI code, a possible optimisation/simplification could be to remove
// // all checks for nonzero root and current_parent throughout UI functions
// static void ui_begin_build(UI *ui) {
//     ui->current_parent = 0;
//     ui->root = 0;

//     f32 dpi_scale = ui->gfx->frame_info.dpi_scale;

//     // TODO(felix): this should probably push the default style from a style struct, so that usage code can also do ui_push_style(custom_style)
//     ui->style_stack.pad.count = 0;
//     V2 default_pad = v2_scale((V2){ .x = 5.f, .y = 2.5f }, dpi_scale);
//     push(&ui->style_stack.pad, default_pad);

//     ui->style_stack.margin.count = 0;
//     V2 default_margin = v2_scale((V2){ .x = 5.f, .y = 5.f }, dpi_scale);
//     push(&ui->style_stack.margin, default_margin);

//     ui->style_stack.clr_fg.count = 0;
//     push(&ui->style_stack.clr_fg, ((V4){ .r = 1.f, .g = 0, .b = 0, .a = 1.f }));

//     ui->style_stack.clr_bg.count = 0;
//     push(&ui->style_stack.clr_bg, (V4){0});

//     ui->style_stack.clr_border.count = 0;
//     push(&ui->style_stack.clr_border, (V4){ .a = 0.7f });

//     ui_push_parent(ui, ui_row(ui));
// }

// static void ui_end_build(UI *ui) {
//     ui_compute_layout_standalone(ui, ui->root);
//     ui_compute_layout_dependent_ancestor(ui, ui->root);
//     ui_compute_layout_dependent_descendant(ui, ui->root);
//     ui_compute_layout_solve_violations(ui, ui->root);
//     ui_compute_layout_relative_positions_and_rect(ui, ui->root);
// }

// static UI_Interaction ui_button(UI *ui, char *format, ...) {
//     va_list args;
//     va_start(args, format);
//     String string = string_vprint(ui->frame_arena, format, args);
//     va_end(args);

//     UI_Box *button = ui_box(ui, string);
//     button->build.flags = ui_box_flag_clickable | ui_box_flag_draw_text | ui_box_flag_draw_border | ui_box_flag_draw_background;
//     for_ui_axis (axis) button->build.size[axis].kind = ui_size_kind_text;
//     // button->target_style = (UI_Box_Style){
//     //     .clr_border = { .a = 0.7f },
//     // };
//     return ui_push(ui, button);
// }

// static UI_Box *ui_column(UI *ui) {
//     UI_Box *column = ui_box_frame_local_not_keyed(ui);
//     column->build.flags = ui_axis_y;
//     column->build.size[ui_axis_x].kind = ui_size_kind_largest_child;
//     column->build.size[ui_axis_y].kind = ui_size_kind_sum_of_children;
//     return column;
// }

// static void ui_compute_layout_dependent_ancestor(UI *ui, UI_Box *box) {
//     discard(ui);
//     discard(box);
//     // TODO(felix)
//     // NOTE(felix): preorder
// }

// static void ui_compute_layout_dependent_descendant(UI *ui, UI_Box *box) {
//     if (box == 0) return;
//     if (box->first == 0) return;

//     for (UI_Box *child = box->first; child != 0; child = child->next) ui_compute_layout_dependent_descendant(ui, child);

//     for_ui_axis (axis) switch (box->build.size[axis].kind) {
//         case ui_size_kind_sum_of_children: {
//             box->computed_size_px[axis] = 0;
//             for (UI_Box *child = box->first; child != 0; child = child->next) {
//                 box->computed_size_px[axis] += child->computed_size_px[axis];
//                 box->computed_size_px[axis] += child->style.margin.items[axis];
//             }
//             box->computed_size_px[axis] += box->last->style.margin.items[axis];
//         } break;
//         case ui_size_kind_largest_child: {
//             box->computed_size_px[axis] = 0;
//             UI_Box *biggest_child = box->first;
//             for (UI_Box *child = box->first->next; child != 0; child = child->next) {
//                 if (child->computed_size_px[axis] <= biggest_child->computed_size_px[axis]) continue;
//                 biggest_child = child;
//             }
//             box->computed_size_px[axis] = biggest_child->computed_size_px[axis];
//             box->computed_size_px[axis] += biggest_child->style.margin.items[axis] * 2.f;
//             box->computed_size_px[axis] += box->style.pad.items[axis] * 2.f;
//         } break;
//     }

//     for_ui_axis (axis) assert(box->computed_size_px[axis] != 0);
// }

// static void ui_compute_layout_relative_positions_and_rect(UI *ui, UI_Box *box) {
//     if (box == 0) return;
//     box->target_rect = (UI_Rect){
//         .right = box->computed_size_px[ui_axis_x],
//         .bottom = box->computed_size_px[ui_axis_y],
//     };
//     for_ui_axis (axis) ui_rect_shift_axis(&box->target_rect, axis, box->style.margin.items[axis]);

//     if (box->parent == 0) goto recurse;
//     ui_rect_shift_x(&box->target_rect, box->parent->target_rect.left);
//     ui_rect_shift_y(&box->target_rect, box->parent->target_rect.top);

//     if (box->prev == 0) goto recurse;
//     UI_Axis layout_axis = box->parent->build.flags & ui_box_flag_child_axis;
//     ui_rect_shift_axis(&box->target_rect, layout_axis, box->prev->pos_rel_parent[layout_axis]);
//     ui_rect_shift_axis(&box->target_rect, layout_axis, box->prev->computed_size_px[layout_axis]);
//     for_ui_axis (axis) box->pos_rel_parent[axis] = box->target_rect.items[axis] - box->parent->target_rect.items[axis];

//     recurse:
//     // TODO(felix): make lerp smoothing framerate-independent. Refer to https://www.youtube.com/watch?v=LSNQuFEDOyQ (r = remainder after one second)
//     box->rect = v4_lerp(box->rect, box->target_rect, ui_animation_speed);
//     // NOTE(felix): this is needed to avoid odd sub-pixel motion while animating
//     box->rect = v4_round(box->rect);
//     for (UI_Box *child = box->first; child != 0; child = child->next) ui_compute_layout_relative_positions_and_rect(ui, child);
// }

// static void ui_compute_layout_solve_violations(UI *ui, UI_Box *box) {
//     discard(ui);
//     discard(box);
//     // TODO(felix)
//     // NOTE(felix): preorder
// }

// static void ui_compute_layout_standalone(UI *ui, UI_Box *box) {
//     if (box == 0) return;
//     for_ui_axis (axis) switch (box->build.size[axis].kind) {
//         case ui_size_kind_nil: assert(false); break;
//         case ui_size_kind_text: /* computed_size_px is already known */ break;
//         case ui_size_kind_pixels: {
//             // TODO(felix): don't multiply
//             box->computed_size_px[axis] = box->build.size[axis].value * 5.f;
//         } break;
//     }
//     for (UI_Box *child = box->first; child != 0; child = child->next) ui_compute_layout_standalone(ui, child);
//     return;
// }

// static UI ui_init(Arena *persistent_arena, Arena *frame_arena, Gfx_Render_Context *gfx) {
//     UI ui = {
//         .persistent_arena = persistent_arena,
//         .frame_arena = frame_arena,
//         .gfx = gfx,
//         .box_hashmap = ui_box_hashmap, // TODO(felix): this should be a local-persist, not a global
//     };
//     ui.style_stack.pad.arena = ui.persistent_arena;
//     ui.style_stack.margin.arena = ui.persistent_arena;
//     ui.style_stack.clr_fg.arena = ui.persistent_arena;
//     ui.style_stack.clr_bg.arena = ui.persistent_arena;
//     ui.style_stack.clr_border.arena = ui.persistent_arena;
//     return ui;
// }

// static inline void ui_pop_parent(UI *ui) { ui->current_parent = ui->current_parent->parent; }

// static UI_Interaction ui_push(UI *ui, UI_Box *box) {
//     box->parent = 0;
//     box->prev = 0;
//     box->next = 0;
//     box->first = 0;
//     box->last = 0;

//     if (ui->root == 0) {
//         assert(ui->current_parent == 0);
//         ui->root = box;
//         ui->current_parent = box;
//         goto interaction;
//     }
//     assert(ui->current_parent != 0);

//     UI_Box *parent = ui->current_parent;
//     UI_Box *child = box;
//     child->parent = parent;
//     if (parent->first == 0) {
//         parent->first = child;
//         parent->last = child;
//     } else {
//         parent->last->next = child;
//         child->prev = parent->last;
//         parent->last = child;
//     }

//     interaction: {
//         if (!(box->build.flags & ui_box_flag_clickable)) {
//             box->interaction = (UI_Interaction){0};
//         } else {
//             // TODO(felix): this should use the virtual mouse position
//             V2 mouse_position = ui->gfx->frame_info.real_mouse_position;
//             bool hovered =
//                 (box->rect.left <= mouse_position.x && mouse_position.x <= box->rect.right) &&
//                 (box->rect.top <= mouse_position.y && mouse_position.y <= box->rect.bottom);
//             box->interaction = (UI_Interaction){
//                 .hovered = hovered,
//                 .clicked = hovered & ui->gfx->frame_info.mouse_left_clicked,
//             };
//         }
//         return box->interaction;
//     }
// }

// static UI_Interaction ui_push_parent(UI *ui, UI_Box *box) {
//     UI_Interaction interaction = ui_push(ui, box);
//     box->parent = ui->current_parent;
//     ui->current_parent = box;
//     return interaction;
// }

// static inline void ui_rect_shift_axis(UI_Rect *rect, UI_Axis axis, f32 shift) {
//     rect->items[axis] += shift;
//     rect->items[2 + axis] += shift;
// }

// static inline void ui_rect_shift_x(UI_Rect *rect, f32 shift) { rect->left += shift; rect->right += shift; }

// static inline void ui_rect_shift_y(UI_Rect *rect, f32 shift) { rect->top += shift; rect->bottom += shift; }

// static void ui_render_recursive(UI *ui, UI_Box *box) {
//     assert(!v4_equal(box->target_rect, (V4){0}));

//     u8 draw_text = box->build.flags & ui_box_flag_draw_text;
//     u8 draw_border = box->build.flags & ui_box_flag_draw_border;
//     u8 draw_background = box->build.flags & ui_box_flag_draw_background;
//     u8 draw_texture = box->build.flags & ui_box_flag_draw_texture;

//     // TODO(felix): get all the colours and values below from box style
//     UI_Box_Style *target = &box->target_style;

//     // if (!draw_border) target->clr_border.a = 0;
//     target->clr_border.a = draw_border ? 0.7f : 0;

//     // TODO(felix): framerate-independent lerping
//     box->style = (UI_Box_Style){
//         .pad = v2_lerp(box->style.pad, target->pad, ui_animation_speed),
//         .margin = v2_lerp(box->style.margin, target->margin, ui_animation_speed),
//         .clr_fg = v4_lerp(box->style.clr_fg, target->clr_fg, ui_animation_speed),
//         .clr_bg = v4_lerp(box->style.clr_bg, target->clr_bg, ui_animation_speed),
//         .clr_border = v4_lerp(box->style.clr_border, target->clr_border, ui_animation_speed),
//     };

//     if (draw_background) {
//         target->clr_bg = (V4){ .r = 0.6f, .g = 0.6f, .b = 0.6f, .a = 1.f };
//         if (box->build.flags & ui_box_flag_clickable) {
//             if (box->interaction.clicked) {
//                 box->style.clr_bg = (V4){ .r = 0, .g = 0, .b = 0, .a = 1.f };
//                 box->style.clr_border = (V4){ .r = 1.f, .g = 1.f, .b = 1.f, .a = 1.f };
//             } else if (box->interaction.hovered) {
//                 target->clr_bg = (V4){ .r = 0.4f, .g = 0.4f, .b = 0.4f, .a = 1.f };
//                 target->clr_border.a = 1.f;
//                 // looks better when instant; the *return* to unhovered colours can animate instead
//                 box->style.clr_bg = target->clr_bg;
//                 box->style.clr_border = target->clr_border;
//             } else target->clr_bg.a = 1.f;
//         }

//         // V4 background = box->style.clr_bg;

//         // TODO(felix): move these to box style
//         // TODO(felix): add corner radius
//         i32 border_thickness = 1;
//         // TODO(felix): draw border
//         discard(border_thickness);

//         u32 rgba = gfx_rgba8_from_v4(box->style.clr_bg);
//         // u32 rgba = 0xe0e0e0ff;
//         i32 top = (i32)(box->rect.top + 0.5f);
//         i32 left = (i32)(box->rect.left + 0.5f);
//         i32 width = (i32)(box->rect.right - box->rect.left + 0.5f);
//         i32 height = (i32)(box->rect.bottom - box->rect.top + 0.5f);
//         gfx_draw_rectangle(ui->gfx, top, left, width, height, rgba);
//     }

//     // // TODO(felix): remove
//     // {
//     //     V4 clr_bg = box->style.clr_bg;
//     //     if (!draw_background) clr_bg.a = 0.f;

//     //     D2D1_RECT_F d2_layout_rect = *(D2D1_RECT_F *)(&box->rect);
//     //     D2D1_ROUNDED_RECT d2_rounded_rect = {
//     //         .rect = d2_layout_rect,
//     //         .radiusX = corner_radius, .radiusY = corner_radius,
//     //     };

//     //     // TODO(felix): cache this!
//     //     ID2D1SolidColorBrush *bg_brush = 0;
//     //     win32_ensure_hr(ID2D1RenderTarget_CreateSolidColorBrush(ui->gfx->d2d_render_target, (D2D1_COLOR_F *)&clr_bg, 0, &bg_brush));
//     //     ID2D1RenderTarget_FillRoundedRectangle(ui->gfx->d2d_render_target, &d2_rounded_rect, (ID2D1Brush *)bg_brush);

//     //     // TODO(felix): cache this!
//     //     ID2D1SolidColorBrush *border_brush = 0;
//     //     win32_ensure_hr(ID2D1RenderTarget_CreateSolidColorBrush(ui->gfx->d2d_render_target, (D2D_COLOR_F *)&box->style.clr_border, 0, &border_brush));

//     //     ID2D1RenderTarget_DrawRoundedRectangle(ui->gfx->d2d_render_target,
//     //         &d2_rounded_rect, (ID2D1Brush *)border_brush, border_thickness, ui->gfx->d2d_stroke_style
//     //     );

//     //     ensure_released_and_null(&border_brush);
//     //     ensure_released_and_null(&bg_brush);
//     // }

//     if (draw_text) {
//         UI_Rect text_rect = box->rect;
//         text_rect.left += box->style.margin.x;

//         i32 x = (i32)text_rect.left;
//         i32 y = (i32)text_rect.top;
//         gfx_draw_text(ui->gfx, ui->gfx->font, box->build.display_string, x, y, 0x000000ff);
//     }

//     if (draw_texture) panic("TODO(felix)");

//     for (UI_Box *child = box->first; child != 0; child = child->next) ui_render_recursive(ui, child);
// }

// static UI_Box *ui_row(UI *ui) {
//     UI_Box *row = ui_box_frame_local_not_keyed(ui);
//     row->build.flags = ui_axis_x;
//     row->build.size[ui_axis_x].kind = ui_size_kind_sum_of_children;
//     row->build.size[ui_axis_y].kind = ui_size_kind_largest_child;
//     return row;
// }

// static UI_Interaction ui_text(UI *ui, char *format, ...) {
//     va_list args;
//     va_start(args, format);
//     String string = string_vprint(ui->frame_arena, format, args);
//     va_end(args);

//     UI_Box *text = ui_box(ui, string);
//     text->build.flags = ui_box_flag_draw_text;
//     for_ui_axis (axis) text->build.size[axis].kind = ui_size_kind_text;
//     return ui_push(ui, text);
// }

// #endif // defined(BASE_NO_IMPLEMENTATION) || defined(BASE_NO_IMPLEMENTATION_UI)
