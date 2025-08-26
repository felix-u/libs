#if defined(BASE_NO_IMPLEMENTATION) || defined(BASE_NO_IMPLEMENTATION_MAP)

structdef(Map_Result) { u64 index; void *pointer; };

structdef(Map_Get_Arguments) {
    u64 _item_size;
    Map_void *map;
    String key;
    void *put;
};


#else // IMPLEMENTATION


#define map_max_load_factor 70

#define map_make(arena, map, capacity) do { \
    static_assert(sizeof(*(map)) == sizeof(Map_void), "Parameter must be a Map"); \
    map_make_explicit_item_size((arena), (Map_void *)(map), (capacity), sizeof(*(map)->values.data)); \
} while (0);

static void map_make_explicit_item_size(Arena *arena, Map_void *map, u64 capacity, u64 item_size) {
    capacity *= 100;
    capacity /= map_max_load_factor;

    map->keys.arena = arena;
    reserve(&map->keys, capacity);
    map->keys.count = 1;

    map->values.arena = arena;
    reserve_explicit_item_size(&map->values, capacity, item_size, false);
    map->values.count = 1;

    Array_u64 indices = { .arena = arena };
    reserve(&indices, capacity);
    map->value_index_from_key_hash = (Slice_u64){ .data = indices.data, .count = indices.capacity };
}

#define map_get(map, key, ...) map_get_((Map_Get_Arguments){ \
    ._item_size = sizeof *(map)->values.data, \
    (Map_void *)(map), \
    slice_as_bytes(key), \
    __VA_ARGS__ \
})

#define map_put(map, key, put, ...) map_get(map, key, \
    0 ? &(map)->values.data[-1] : put, /* type checking hack */ \
)

static Map_Result map_get_(Map_Get_Arguments arguments) {
    Map_void *map = arguments.map;
    String key = arguments.key;
    void *put = arguments.put;
    u64 item_size = arguments._item_size;

    Map_Result result = {0};

    // djb2 hash
    u64 hash = 5381;
    {
        for_slice (u8 *, byte, key) hash = ((hash << 5) + hash) + *byte;
        // avoid outputting 0
        hash %= (map->values.capacity - 1);
        hash += 1;
    }

    u64 *value_index = 0;
    u64 probe_increment = 1;

    for (;;) {
        value_index = &map->value_index_from_key_hash.data[hash];
        if (*value_index == 0) break;

        String key_at_index = map->keys.data[*value_index];
        if (string_equals(key, key_at_index)) break;

        hash += probe_increment;
        probe_increment += 1;

        // wrap and avoid 0
        while (hash >= map->value_index_from_key_hash.count) hash -= map->value_index_from_key_hash.count;
        if (hash == 0) hash = 1;
    }

    if (put != 0) {
        if (*value_index == 0) {
            assert(map->keys.count == map->values.count);
            ensure(map->values.count + 1 < map->values.capacity * map_max_load_factor / 100);

            *value_index = map->values.count;
            map->values.count += 1;
            map->keys.count += 1;
        }

        map->keys.data[*value_index] = key;

        void *pointer = (u8 *)map->values.data + *value_index * item_size;
        memcpy(pointer, put, item_size);
    }

    result.index = *value_index;
    if (result.index != 0) {
        result.pointer = (u8 *)map->values.data + result.index * item_size;
    }
    return result;
}


#endif // defined(BASE_NO_IMPLEMENTATION) || defined(BASE_NO_IMPLEMENTATION_MAP)
