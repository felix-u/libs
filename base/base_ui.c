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

static inline UI_Box *ui_box(Str8 str) {
    UI_Str_Parsed parsed = ui_parse_str(str);
    return ui_hash_find_or_alloc_new(ui_hash_key_from_str(parsed.to_hash), parsed);
}

static void ui_box_add_child(UI_Box *parent, UI_Box *child) {
    if (parent->first == 0) {
        parent->first = child;
        parent->last = child;
        return;
    }
    child->parent = parent;
    parent->last->next = child;
    child->prev = parent->last;
    parent->last = child;
}

// TODO(felix): can take from freelist here also once implemented
static UI_Box *ui_box_frame_local_not_keyed(void) { return arena_alloc(ui_state.arena_frame, 1, sizeof(UI_Box)); }

static UI_Box_Style ui_box_style_lerp(UI_Box_Style a, UI_Box_Style b, f32 amount) {
    return (UI_Box_Style){
        .pad = v2_lerp(a.pad, b.pad, amount),
        .margin = v2_lerp(a.margin, b.margin, amount),
        .clr_fg = v4_lerp(a.clr_fg, b.clr_fg, amount),
        .clr_bg = v4_lerp(a.clr_bg, b.clr_bg, amount),
        .clr_border = v4_lerp(a.clr_border, b.clr_border, amount),
    };
}

static inline void ui_box_zero_tree_links(UI_Box *b) { b->parent = 0; b->prev = 0; b->next = 0; b->first = 0; b->last = 0; }

// NOTE(felix): Since a root node is guaranteed before user UI code, a possible optimisation/simplification could be to remove
// all checks for nonzero root and current_parent throughout UI functions
static void ui_begin_build(void) {
    ui_state.current_parent = 0;
    ui_state.root = 0;
    ui_push_parent(ui_row());
}

static UI_Interaction ui_buttonS(Str8 str) {
    UI_Box *button = ui_box(str);
    button->build.flags = ui_box_flag_clickable | ui_box_flag_draw_text | ui_box_flag_draw_border | ui_box_flag_draw_background;
    for_ui_axis (axis) button->build.size[axis].kind = ui_size_kind_text;
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
            }
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
        case ui_size_kind_nil: assume(false); break;
        case ui_size_kind_text: /* computed_size_px is already known */ break;
    }
    for (UI_Box *child = box->first; child != 0; child = child->next) ui_compute_layout_standalone(child);
    return;
}

static void ui_end_build_and_compute_layout(void) {
    ui_compute_layout_standalone(ui_state.root);
    ui_compute_layout_dependent_ancestor(ui_state.root);
    ui_compute_layout_dependent_descendant(ui_state.root);
    ui_compute_layout_solve_violations(ui_state.root);
    ui_compute_layout_relative_positions_and_rect(ui_state.root);
}

static UI_Box *ui_hash_find_or_alloc_new(usize key, UI_Str_Parsed str) {
    V2 str_dimensions = gfx_str_dimensions(ui_state.arena_frame, ui_state.dw_ctx, str.to_display);

    for (UI_Box *match = ui_state.box_hashmap.ptr[key]; match != 0; match = match->next) {
        if (!str8_eql(str.to_hash, match->build.str.to_hash)) continue;
        // TODO(felix): there are fields we need to initialise here that are currently only initialised when allocating a new widget
        // TODO(felix): also, I think, fields that need to be zeroed
        // TODO(felix): need to do some cleanup here to unify control flow
        slice_copy(ui_state.arena_frame, &match->build.str.to_display, &str.to_display);
        match->computed_size_px[ui_axis_x] = str_dimensions.x + match->style.pad.x * 2.f;
        match->computed_size_px[ui_axis_y] = str_dimensions.y + match->style.pad.y * 2.f;
        return match;
    }

    // TODO(felix): check freelist in ui_state before allocating new memory. To populate said freelist, we'll need to prune the hashmap every frame.
    UI_Box *new = arena_alloc(ui_state.arena_persistent, 1, sizeof(UI_Box));
    ui_state.box_hashmap.ptr[key] = new;

    UI_Box_Style target_style = { .pad = v2(15.f, 7.5f), .margin = v2(10.f, 10.f), .clr_fg = v4(0, 0, 0, 1.f) };
    *new = (UI_Box){
        .key = key,
        .target_style = target_style, // NOTE(felix): for testing
        .computed_size_px = { str_dimensions.x + target_style.pad.x * 2.f, str_dimensions.y + target_style.pad.y * 2.f },
    };
    slice_copy(ui_state.arena_persistent, &new->build.str.to_hash, &str.to_hash);
    slice_copy(ui_state.arena_frame, &new->build.str.to_display, &str.to_display);
    return new;
}

static usize ui_hash_key_from_str(Str8 str) {
    // djb2 hash
    usize hash = 5381;
    for (usize i = 0; i < str.len; i += 1) {
        hash = ((hash << 5) + hash) + str.ptr[i];
    }
    return hash % ui_state.box_hashmap.cap;
}

static UI_Interaction ui_interaction_compute(UI_Box *box) {
    if (!(box->build.flags & ui_box_flag_clickable)) return (UI_Interaction){0};

    // TODO(felix): can this be naively optimised by returning a nil interaction if the mouse doesn't fall within the parent rect?
    bool hovered = ui_mouse_in_rect(box->rect);
    return (UI_Interaction){
        .hovered = hovered,
        .clicked = hovered & mouse_left_clicked,
    };
}

static inline bool ui_mouse_in_rect(UI_Rect rect) {
    return (rect.left <= mouse_x && mouse_x <= rect.right) && (rect.top <= mouse_y && mouse_y <= rect.bottom);
}

static UI_Str_Parsed ui_parse_str(Str8 s) {
    UI_Str_Parsed result = { .to_hash = s, .to_display = s };

    usize specifier_beg_i = 0, specifier_end_i = 0;
    enum { before, after, pos_count };
    bool hash[pos_count] = {0};
    bool display[pos_count] = {0};

    for (usize i = 0; i < s.len; i += 1) {
        if (s.ptr[i] != '#') continue;

        i += 1;
        if (i == s.len) break;
        if (s.ptr[i] != '[') continue;

        specifier_beg_i = i - 1;
        usize pos = before;

        for (; i < s.len; i += 1) switch (s.ptr[i]) {
            case ']': specifier_end_i = i; goto compute_specifier;
            case 'h': hash[pos] = true; break;
            case 'd': display[pos] = true; break;
            case ',': {
                pos += 1;
                if (pos != after) panicf("specifier in string '%.*s' has too many commas", str_fmt(s));
            } break;
            default: break;
        }
        panicf("specifier in string '%.*s' never closed with ']'", str_fmt(s));
    }
    goto no_specifier;

    compute_specifier: {
        assume(specifier_end_i != 0 || hash[before] || hash[after]);
        Str8 before_specifier = str8_range(s, 0, specifier_beg_i);
        Str8 after_specifier = str8_range(s, specifier_end_i + 1, s.len);

        Array_u8 to_hash = {0};
        arena_alloc_array(ui_state.arena_frame, &to_hash, s.len);
        if (hash[before]) array_push_slice_assume_capacity(&to_hash, &before_specifier);
        if (hash[after]) array_push_slice_assume_capacity(&to_hash, &after_specifier);

        Array_u8 to_display = {0};
        arena_alloc_array(ui_state.arena_frame, &to_display, s.len);
        if (display[before]) array_push_slice_assume_capacity(&to_display, &before_specifier);
        if (display[after]) array_push_slice_assume_capacity(&to_display, &after_specifier);

        result = (UI_Str_Parsed){ .to_hash = slice_from_array(to_hash), .to_display = slice_from_array(to_display) };
    }

    no_specifier:
    return result;
}

static inline void ui_pop_parent(void) { ui_state.current_parent = ui_state.current_parent->parent; }

static UI_Interaction ui_push(UI_Box *box) {
    ui_box_zero_tree_links(box);

    if (ui_state.root == 0) {
        assume(ui_state.current_parent == 0);
        ui_state.root = box;
        ui_state.current_parent = box;
        goto interaction;
    }
    assume(ui_state.current_parent != 0);

    ui_box_add_child(ui_state.current_parent, box);

    interaction: {
        box->interaction = ui_interaction_compute(box);
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

static void ui_render_recurse(UI_Box *box) {
    box->style = ui_box_style_lerp(box->style, box->target_style, ui_animation_speed);

    if (!(box->build.flags & ui_box_flag_draw_text)) {
        // TODO(felix): support border, background, etc. even without text
        assume(box->build.flags == 0 || box->build.flags == ui_axis_y);
        goto recurse;
    }

    b8 draw_border = box->build.flags & ui_box_flag_draw_border;
    b8 draw_background = box->build.flags & ui_box_flag_draw_background;

    // TODO(felix): get all the colours and values below from somewhere else - make a style stack system? - but don't hardcode here

    if (draw_border) box->target_style.clr_border = v4(0, 0, 0, 0.7f);
    if (draw_background) {
        box->target_style.clr_bg = v4(0.6f, 0.6f, 0.6f, 1.f);
        if (box->build.flags & ui_box_flag_clickable) {
            if (box->interaction.clicked) {
                box->style.clr_bg = v4(0, 0, 0, 1.f);
                box->style.clr_border = v4(1.f, 1.f, 1.f, 1.f);
            } else if (box->interaction.hovered) {
                box->target_style.clr_bg.a = 0.4f;
                box->target_style.clr_border.a = 1.f;
                // looks better when instant; the *return* to unhovered colours can animate instead
                box->style.clr_bg.a = 0.4f;
                box->style.clr_border.a = 1.f;
            } else box->target_style.clr_bg.a = 1.f;
        }
    }

    gfx_draw_rounded_rect(ui_state.dw_ctx, (Gfx_Rounded_Rect){
        .colour = draw_background ? box->style.clr_bg : (V4){0},
        .corner_radius = 15.f,
        .border_thickness = draw_border ? 3.f : 0,
        .border_colour = box->style.clr_border,
        .rect = box->rect,
    });

    UI_Rect text_rect = box->rect;
    text_rect.left += box->style.margin.x;
    // TODO(felix): add parameter for text colour
    gfx_draw_text(ui_state.arena_frame, ui_state.dw_ctx, box->build.str.to_display, text_rect);

    recurse:
    for (UI_Box *child = box->first; child != 0; child = child->next) ui_render_recurse(child);
}

static UI_Box *ui_row(void) {
    UI_Box *row = ui_box_frame_local_not_keyed();
    row->build.flags = ui_axis_x;
    row->build.size[ui_axis_x].kind = ui_size_kind_sum_of_children;
    row->build.size[ui_axis_y].kind = ui_size_kind_largest_child;
    return row;
}

static UI_Interaction ui_textS(Str8 str) {
    UI_Box *text = ui_box(str);
    text->build.flags = ui_box_flag_draw_text;
    for_ui_axis (axis) text->build.size[axis].kind = ui_size_kind_text;
    return ui_push(text);
}
ui_define_fmt_fns(ui_text)
