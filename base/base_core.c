// TODO(felix): add slice_equal/slice_compare

static void array_ensure_capacity_explicit_item_size(Array_void *array, usize item_count, usize item_size, bool non_zero) {
    if (array->cap >= item_count) return;

    // TODO(felix): should this be a power of 2?
    usize new_capacity = clamp_low(1, array->cap * 2);
    while (new_capacity < item_count) new_capacity *= 2;

    u8 *new_memory = arena_make(array->arena, item_count, item_size);
    memcpy(new_memory, array->ptr, array->len * item_size);

    if (!non_zero) {
        usize old_capacity = array->cap;
        usize growth_byte_count = item_size * (new_capacity - old_capacity);
        u8 *beginning_of_new_memory = new_memory + (item_size * old_capacity);
        memset(beginning_of_new_memory, 0, growth_byte_count);
    }

    array->ptr = new_memory;
    array->cap = new_capacity;
}

static inline void array_push_explicit_item_size(Array_void *array, void *item, usize item_size) {
    Slice_void slice = { .ptr = item, .len = 1 };
    array_push_slice_explicit_item_size(array, &slice, item_size);
}

static inline void array_push_explicit_item_size_assume_capacity(Array_void *array, void *item, usize item_size) {
    usize new_len = array->len + 1;
    assert(new_len <= array->cap);
    memmove((u8 *)array->ptr + (array->len * item_size), item, item_size);
    array->len = new_len;
}

static void array_push_slice_explicit_item_size(Array_void *array, Slice_void *slice, usize item_size) {
    assert(array->arena != 0);
    usize new_len = array->len + slice->len;
    array_ensure_capacity_explicit_item_size(array, new_len, item_size, false);
    memmove((u8 *)array->ptr + (array->len * item_size), slice->ptr, slice->len * item_size);
    array->len = new_len;
}

static void array_push_slice_explicit_item_size_assume_capacity(Array_void *array, Slice_void *slice, usize item_size) {
    usize new_len = array->len + slice->len;
    assert(new_len <= array->cap);
    memmove((u8 *)array->ptr + (array->len * item_size), slice->ptr, slice->len * item_size);
    array->len = new_len;
}
