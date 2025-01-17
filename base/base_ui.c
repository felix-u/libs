// NOTE(felix): How this needs to look:
// 1. Build box hierarchy. Use last frame's data (looked up by hash) for input
//  requires hash links { UI_Box *hash_prev, *hash_next }
//  requires keying and frame info { UI_Key key; usize last_frame_touched_idx }
//      a) look up in box hash table to be able to use previous frame's layout to compute input
//      b) iterate through hash table. if a box's last_frame_touched_idx < current_frame_idx, remove
// 2. Layout pass (compute sizes)
//  requires tree links { UI_Box *parent, *first, *last, *prev, *next }
//      a) calculate standalone sizes, which do not depend on other boxs
//      b) pre-order: calculate sizes dependent on ancestor sizes
//      c) post-order: calculate sizes dependent on descendant sizes
//      d) pre-order: solve violations using strictness value
//      e) pre-order: using calculated sizes, compute relative positions of each box (lay out on axis specified by parent),
//         compute final screen coordinate rectangle
// 3. Render

static UI_Box *ui_box_map_mem[ui_box_map_size];

static Array_UI_Box_Ptr ui_box_hashmap = {
    .ptr = ui_box_map_mem,
    .cap = ui_box_map_size,
};

static UI_State ui_state;

static UI_Box *ui_border_box(void) {
    UI_Box *panel = ui_box_frame_local_not_keyed();
    panel->build.flags = ui_box_flag_draw_border;
    for_ui_axis (axis) panel->build.size[axis].kind = ui_size_kind_sum_of_children;
    return panel;
}

static inline UI_Box *ui_box(String str) {
    assert(str.len != 0 && str.ptr != 0);

    String display_str = str;
    String hash_str = str;
    {
        usize specifier_beg_i = 0, specifier_end_i = 0;
        enum { before, after, pos_count };
        bool hash[pos_count] = {0};
        bool display[pos_count] = {0};

        for (usize i = 0; i < str.len; i += 1) {
            if (str.ptr[i] != '#') continue;

            i += 1;
            if (i == str.len) break;
            if (str.ptr[i] != '[') continue;

            specifier_beg_i = i - 1;
            usize pos = before;

            for (; i < str.len; i += 1) switch (str.ptr[i]) {
                case ']': specifier_end_i = i; goto compute_specifier;
                case 'h': hash[pos] = true; break;
                case 'd': display[pos] = true; break;
                case ',': {
                    pos += 1;
                    if (pos != after) panicf("specifier in string '%' has too many commas", fmt(String, str));
                } break;
                default: break;
            }
            panicf("specifier in string '%' never closed with ']'", fmt(String, str));
        }
        goto done_parsing;

        compute_specifier: {
            assert(specifier_end_i != 0 || hash[before] || hash[after]);
            String before_specifier = string_range(str, 0, specifier_beg_i);
            String after_specifier = string_range(str, specifier_end_i + 1, str.len);

            Array_u8 hash_str_builder = {0};
            arena_alloc_array(ui_state.arena_frame, &hash_str_builder, str.len);
            if (hash[before]) array_push_slice_assume_capacity(&hash_str_builder, &before_specifier);
            if (hash[after]) array_push_slice_assume_capacity(&hash_str_builder, &after_specifier);

            Array_u8 display_str_builder = {0};
            arena_alloc_array(ui_state.arena_frame, &display_str_builder, str.len);
            if (display[before]) array_push_slice_assume_capacity(&display_str_builder, &before_specifier);
            if (display[after]) array_push_slice_assume_capacity(&display_str_builder, &after_specifier);

            hash_str = (String)slice_from_array(hash_str_builder);
            display_str = (String)slice_from_array(display_str_builder);
        }

        done_parsing:;
    }

    assert(display_str.len != 0 && display_str.ptr != 0);
    assert(hash_str.len != 0 && hash_str.ptr != 0);

    // djb2 hash
    usize key = 5381;
    for (usize i = 0; i < hash_str.len; i += 1) {
        key = ((key << 5) + key) + hash_str.ptr[i];
    }
    key %= ui_state.box_hashmap.cap;

    V2 str_dimensions = {0};
    {
        String16 wstr = string16_from_string(ui_state.arena_frame, display_str);
        // TODO(felix): cache this! we want to call these functions as little as possible
        IDWriteFactory_CreateTextLayout(ui_state.render_ctx->dw_factory,
            wstr.ptr, (u32)wstr.len, ui_state.render_ctx->dw_text_fmt, window_size.x, window_size.y, &ui_state.render_ctx->dw_text_layout
        );
        DWRITE_TEXT_METRICS metrics = {0};
        win32_assert_hr(IDWriteTextLayout_GetMetrics(ui_state.render_ctx->dw_text_layout, &metrics));
        ensure_released_and_null(&ui_state.render_ctx->dw_text_layout);
        str_dimensions = (V2){ .x = metrics.width, .y = metrics.height };
    }

    for (UI_Box *match = ui_state.box_hashmap.ptr[key]; match != 0; match = match->next) {
        if (!string_equal(hash_str, match->build.str_to_hash)) continue;
        // TODO(felix): there are fields we need to initialise here that are currently only initialised when allocating a new widget
        // TODO(felix): also, I think, fields that need to be zeroed
        // TODO(felix): need to do some cleanup here to unify control flow
        slice_copy(ui_state.arena_frame, &match->build.str_to_display, &display_str);
        match->computed_size_px[ui_axis_x] = str_dimensions.x + match->style.pad.x * 2.f;
        match->computed_size_px[ui_axis_y] = str_dimensions.y + match->style.pad.y * 2.f;
        return match;
    }

    // TODO(felix): check freelist in ui_state before allocating new memory. To populate said freelist, we'll need to prune the hashmap every frame.
    UI_Box *new = arena_alloc(ui_state.arena_persistent, 1, sizeof(UI_Box));
    ui_state.box_hashmap.ptr[key] = new;

    // TODO(felix): I think the new style should be applied even above, in the case where the widget already exists
    UI_Box_Style target_style = {
        .pad = slice_get_last_assume_not_empty(ui_state.style_stack.pad),
        .margin = slice_get_last_assume_not_empty(ui_state.style_stack.margin),
        .clr_fg = slice_get_last_assume_not_empty(ui_state.style_stack.clr_fg),
        .clr_bg = slice_get_last_assume_not_empty(ui_state.style_stack.clr_bg),
        .clr_border = slice_get_last_assume_not_empty(ui_state.style_stack.clr_border),
    };
    *new = (UI_Box){
        .key = key,
        .target_style = target_style,
        .computed_size_px = { str_dimensions.x + target_style.pad.x * 2.f, str_dimensions.y + target_style.pad.y * 2.f },
    };
    slice_copy(ui_state.arena_persistent, &new->build.str_to_hash, &hash_str);
    slice_copy(ui_state.arena_frame, &new->build.str_to_display, &display_str);
    return new;
}

// TODO(felix): can take from freelist here also once implemented
static UI_Box *ui_box_frame_local_not_keyed(void) {
    UI_Box *box = arena_alloc(ui_state.arena_frame, 1, sizeof(UI_Box));
    return box;
}

// NOTE(felix): Since a root node is guaranteed before user UI code, a possible optimisation/simplification could be to remove
// all checks for nonzero root and current_parent throughout UI functions
static void ui_begin_build(void) {
    ui_state.current_parent = 0;
    ui_state.root = 0;

    ui_state.style_stack.pad.len = 0;
    V2 default_pad = v2_scale((V2){ .x = 5.f, .y = 2.5f }, dpi_scale);
    array_push(ui_state.arena_persistent, &ui_state.style_stack.pad, &default_pad);

    ui_state.style_stack.margin.len = 0;
    V2 default_margin = v2_scale((V2){ .x = 5.f, .y = 5.f }, dpi_scale);
    array_push(ui_state.arena_persistent, &ui_state.style_stack.margin, &default_margin);

    ui_state.style_stack.clr_fg.len = 0;
    array_push(ui_state.arena_persistent, &ui_state.style_stack.clr_fg, (&(V4){ .r = 1.f, .g = 0, .b = 0, .a = 1.f }));

    ui_state.style_stack.clr_bg.len = 0;
    array_push(ui_state.arena_persistent, &ui_state.style_stack.clr_bg, &(V4){0});

    ui_state.style_stack.clr_border.len = 0;
    array_push(ui_state.arena_persistent, &ui_state.style_stack.clr_border, &(V4){ .a = 0.7f });

    ui_push_parent(ui_row());
}

static UI_Interaction ui_buttonS(String str) {
    UI_Box *button = ui_box(str);
    button->build.flags = ui_box_flag_clickable | ui_box_flag_draw_text | ui_box_flag_draw_border | ui_box_flag_draw_background;
    for_ui_axis (axis) button->build.size[axis].kind = ui_size_kind_text;
    // button->target_style = (UI_Box_Style){
    //     .clr_border = { .a = 0.7f },
    // };
    return ui_push(button);
}
ui_define_fmt_fns(ui_button)

static UI_Box *ui_column(void) {
    UI_Box *column = ui_box_frame_local_not_keyed();
    column->build.flags = ui_axis_y;
    column->build.size[ui_axis_x].kind = ui_size_kind_largest_child;
    column->build.size[ui_axis_y].kind = ui_size_kind_sum_of_children;
    return column;
}

static void ui_compute_layout_dependent_ancestor(UI_Box *box) {
    discard(box);
    // TODO(felix)
    // NOTE(felix): preorder
}

static void ui_compute_layout_dependent_descendant(UI_Box *box) {
    if (box == 0) return;
    for (UI_Box *child = box->first; child != 0; child = child->next) ui_compute_layout_dependent_descendant(child);
    for_ui_axis (axis) switch (box->build.size[axis].kind) {
        case ui_size_kind_sum_of_children: {
            box->computed_size_px[axis] = 0;
            for (UI_Box *child = box->first; child != 0; child = child->next) {
                box->computed_size_px[axis] += child->computed_size_px[axis];
                box->computed_size_px[axis] += child->style.margin.elements[axis];
            }
            box->computed_size_px[axis] += box->last->style.margin.elements[axis];
        } break;
        case ui_size_kind_largest_child: {
            box->computed_size_px[axis] = 0;
            for (UI_Box *child = box->first; child != 0; child = child->next) {
                f32 size_px = child->computed_size_px[axis];
                if (size_px <= box->computed_size_px[axis]) continue;
                box->computed_size_px[axis] = size_px;
            }
        } break;
    }
    for_ui_axis (axis) assert(box->computed_size_px[axis] != 0);
}

static void ui_compute_layout_relative_positions_and_rect(UI_Box *box) {
    if (box == 0) return;
    box->target_rect = (UI_Rect){
        .right = box->computed_size_px[ui_axis_x],
        .bottom = box->computed_size_px[ui_axis_y],
    };
    for_ui_axis (axis) ui_rect_shift_axis(&box->target_rect, axis, box->style.margin.elements[axis]);

    if (box->parent == 0) goto recurse;
    ui_rect_shift_x(&box->target_rect, box->parent->target_rect.left);
    ui_rect_shift_y(&box->target_rect, box->parent->target_rect.top);

    if (box->prev == 0) goto recurse;
    UI_Axis layout_axis = box->parent->build.flags & ui_box_flag_child_axis;
    ui_rect_shift_axis(&box->target_rect, layout_axis, box->prev->pos_rel_parent[layout_axis]);
    ui_rect_shift_axis(&box->target_rect, layout_axis, box->prev->computed_size_px[layout_axis]);
    for_ui_axis (axis) box->pos_rel_parent[axis] = box->target_rect.elements[axis] - box->parent->target_rect.elements[axis];

    recurse:
    // TODO(felix): make lerp smoothing framerate-independent. Refer to https://www.youtube.com/watch?v=LSNQuFEDOyQ (r = remainder after one second)
    box->rect = v4_lerp(box->rect, box->target_rect, ui_animation_speed);
    // NOTE(felix): this is needed to avoid odd sub-pixel motion while animating
    box->rect = v4_round(box->rect);
    for (UI_Box *child = box->first; child != 0; child = child->next) ui_compute_layout_relative_positions_and_rect(child);
}

static void ui_compute_layout_solve_violations(UI_Box *box) {
    discard(box);
    // TODO(felix)
    // NOTE(felix): preorder
}

static void ui_compute_layout_standalone(UI_Box *box) {
    if (box == 0) return;
    for_ui_axis (axis) switch (box->build.size[axis].kind) {
        case ui_size_kind_nil: assert(false); break;
        case ui_size_kind_text: /* computed_size_px is already known */ break;
    }
    for (UI_Box *child = box->first; child != 0; child = child->next) ui_compute_layout_standalone(child);
    return;
}

static inline void ui_pop_parent(void) { ui_state.current_parent = ui_state.current_parent->parent; }

static UI_Interaction ui_push(UI_Box *box) {
    box->parent = 0;
    box->prev = 0;
    box->next = 0;
    box->first = 0;
    box->last = 0;

    if (ui_state.root == 0) {
        assert(ui_state.current_parent == 0);
        ui_state.root = box;
        ui_state.current_parent = box;
        goto interaction;
    }
    assert(ui_state.current_parent != 0);

    UI_Box *parent = ui_state.current_parent;
    UI_Box *child = box;
    child->parent = parent;
    if (parent->first == 0) {
        parent->first = child;
        parent->last = child;
    } else {
        parent->last->next = child;
        child->prev = parent->last;
        parent->last = child;
    }

    interaction: {
        if (!(box->build.flags & ui_box_flag_clickable)) {
            box->interaction = (UI_Interaction){0};
        } else {
            bool hovered =
                (box->rect.left <= mouse_pos.x && mouse_pos.x <= box->rect.right) &&
                (box->rect.top <= mouse_pos.y && mouse_pos.y <= box->rect.bottom);
            box->interaction = (UI_Interaction){
                .hovered = hovered,
                .clicked = hovered & mouse_left_clicked,
            };
        }
        return box->interaction;
    }
}

static UI_Interaction ui_push_parent(UI_Box *box) {
    UI_Interaction interaction = ui_push(box);
    box->parent = ui_state.current_parent;
    ui_state.current_parent = box;
    return interaction;
}

static inline void ui_rect_shift_axis(UI_Rect *rect, UI_Axis axis, f32 shift) {
    rect->elements[axis] += shift;
    rect->elements[2 + axis] += shift;
}

static inline void ui_rect_shift_x(UI_Rect *rect, f32 shift) { rect->left += shift; rect->right += shift; }

static inline void ui_rect_shift_y(UI_Rect *rect, f32 shift) { rect->top += shift; rect->bottom += shift; }

static void ui_render_recursive(UI_Box *box) {
    assert(!v4_equal(box->target_rect, (V4){0}));

    b8 draw_text = box->build.flags & ui_box_flag_draw_text;
    b8 draw_border = box->build.flags & ui_box_flag_draw_border;
    b8 draw_background = box->build.flags & ui_box_flag_draw_background;

    // TODO(felix): get all the colours and values below from box style
    UI_Box_Style *target = &box->target_style;

    // if (!draw_border) target->clr_border.a = 0;
    target->clr_border.a = draw_border ? 0.7f : 0;

    // TODO(felix): framerate-independent lerping
    box->style = (UI_Box_Style){
        .pad = v2_lerp(box->style.pad, target->pad, ui_animation_speed),
        .margin = v2_lerp(box->style.margin, target->margin, ui_animation_speed),
        .clr_fg = v4_lerp(box->style.clr_fg, target->clr_fg, ui_animation_speed),
        .clr_bg = v4_lerp(box->style.clr_bg, target->clr_bg, ui_animation_speed),
        .clr_border = v4_lerp(box->style.clr_border, target->clr_border, ui_animation_speed),
    };

    if (draw_background) {
        target->clr_bg = (V4){ .r = 0.6f, .g = 0.6f, .b = 0.6f, .a = 1.f };
        if (box->build.flags & ui_box_flag_clickable) {
            if (box->interaction.clicked) {
                box->style.clr_bg = (V4){ .r = 0, .g = 0, .b = 0, .a = 1.f };
                box->style.clr_border = (V4){ .r = 1.f, .g = 1.f, .b = 1.f, .a = 1.f };
            } else if (box->interaction.hovered) {
                target->clr_bg.a = 0.4f;
                target->clr_border.a = 1.f;
                // looks better when instant; the *return* to unhovered colours can animate instead
                box->style.clr_bg.a = 0.4f;
                box->style.clr_border.a = 1.f;
            } else target->clr_bg.a = 1.f;
        }
    }

    {
        V4 clr_bg = box->style.clr_bg;
        if (!draw_background) clr_bg.a = 0.f;

        // TODO(felix): move these to box style
        f32 corner_radius = 5.f * dpi_scale;
        f32 border_thickness = draw_border ? (dpi_scale * 1.f) : 0;

        D2D1_RECT_F d2_layout_rect = *(D2D1_RECT_F *)(&box->rect);
        D2D1_ROUNDED_RECT d2_rounded_rect = {
            .rect = d2_layout_rect,
            .radiusX = corner_radius, .radiusY = corner_radius,
        };

        // TODO(felix): cache this!
        ID2D1SolidColorBrush *bg_brush = 0;
        win32_assert_hr(ID2D1RenderTarget_CreateSolidColorBrush(ui_state.render_ctx->d2_render_target, (D2D1_COLOR_F *)&clr_bg, 0, &bg_brush));
        ID2D1RenderTarget_FillRoundedRectangle(ui_state.render_ctx->d2_render_target, &d2_rounded_rect, (ID2D1Brush *)bg_brush);

        // TODO(felix): cache this!
        ID2D1SolidColorBrush *border_brush = 0;
        win32_assert_hr(ID2D1RenderTarget_CreateSolidColorBrush(ui_state.render_ctx->d2_render_target, (D2D_COLOR_F *)&box->style.clr_border, 0, &border_brush));

        ID2D1RenderTarget_DrawRoundedRectangle(ui_state.render_ctx->d2_render_target,
            &d2_rounded_rect, (ID2D1Brush *)border_brush, border_thickness, ui_state.render_ctx->d2_stroke_style
        );

        ensure_released_and_null(&border_brush);
        ensure_released_and_null(&bg_brush);
    }

    if (draw_text) {
        UI_Rect text_rect = box->rect;
        text_rect.left += box->style.margin.x;

        // TODO(felix): add parameters for text colour, font size
        {
            D2D1_RECT_F layout_rect = *(D2D1_RECT_F *)(&text_rect);
            String16 wstr = string16_from_string(ui_state.arena_frame, box->build.str_to_display);
            ID2D1RenderTarget_DrawText(ui_state.render_ctx->d2_render_target,
                wstr.ptr, (u32)wstr.len, ui_state.render_ctx->dw_text_fmt, &layout_rect,
                (ID2D1Brush *)ui_state.render_ctx->d2_brush, D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT, DWRITE_MEASURING_MODE_NATURAL
            );
        }
    }

    for (UI_Box *child = box->first; child != 0; child = child->next) ui_render_recursive(child);
}

static UI_Box *ui_row(void) {
    UI_Box *row = ui_box_frame_local_not_keyed();
    row->build.flags = ui_axis_x;
    row->build.size[ui_axis_x].kind = ui_size_kind_sum_of_children;
    row->build.size[ui_axis_y].kind = ui_size_kind_largest_child;
    return row;
}

static UI_Interaction ui_textS(String str) {
    UI_Box *text = ui_box(str);
    text->build.flags = ui_box_flag_draw_text;
    for_ui_axis (axis) text->build.size[axis].kind = ui_size_kind_text;
    return ui_push(text);
}
ui_define_fmt_fns(ui_text)
