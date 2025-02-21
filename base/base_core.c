// TODO(felix): add slice_equal/slice_compare

static void array_ensure_capacity_explicit_item_size(Array_void *array, usize item_count, usize item_size, bool non_zero) {
    if (array->capacity >= item_count) return;

    // TODO(felix): should this be a power of 2?
    usize new_capacity = clamp_low(1, array->capacity * 2);
    while (new_capacity < item_count) new_capacity *= 2;

    u8 *new_memory = arena_make(array->arena, new_capacity, item_size);
    if (array->count > 0) memcpy(new_memory, array->data, array->count * item_size);

    if (!non_zero) {
        usize old_capacity = array->capacity;
        usize growth_byte_count = item_size * (new_capacity - old_capacity);
        u8 *beginning_of_new_memory = new_memory + (item_size * old_capacity);
        memset(beginning_of_new_memory, 0, growth_byte_count);
    }

    array->data = new_memory;
    array->capacity = new_capacity;
}

static inline void array_push_explicit_item_size(Array_void *array, void *item, usize item_size) {
    Slice_void slice = { .data = item, .count = 1 };
    array_push_slice_explicit_item_size(array, &slice, item_size);
}

static inline void array_push_explicit_item_size_assume_capacity(Array_void *array, void *item, usize item_size) {
    usize new_len = array->count + 1;
    assert(new_len <= array->capacity);
    memmove((u8 *)array->data + (array->count * item_size), item, item_size);
    array->count = new_len;
}

static void array_push_slice_explicit_item_size(Array_void *array, Slice_void *slice, usize item_size) {
    assert(array->arena != 0);
    usize new_len = array->count + slice->count;
    array_ensure_capacity_explicit_item_size(array, new_len, item_size, false);
    memmove((u8 *)array->data + (array->count * item_size), slice->data, slice->count * item_size);
    array->count = new_len;
}

static void array_push_slice_explicit_item_size_assume_capacity(Array_void *array, Slice_void *slice, usize item_size) {
    usize new_len = array->count + slice->count;
    assert(new_len <= array->capacity);
    memmove((u8 *)array->data + (array->count * item_size), slice->data, slice->count * item_size);
    array->count = new_len;
}
