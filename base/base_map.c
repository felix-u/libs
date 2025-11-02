#if defined(BASE_NO_IMPLEMENTATION) || defined(BASE_NO_IMPLEMENTATION_MAP)


structdef(Intern_Pool) {
    Array_String keys;
    u64 *key_index_from_hash;
};

structdef(Map_Result) { u64 index; void *pointer; bool is_new; };

structdef(Map_Get_Arguments) {
    u64 _item_size;
    Map_void *map;
    u64 key;
    void *put;
};

static u64 hash_djb2(String bytes);
static u64 hash_lookup_msi(u64 hash, u64 exponent, u64 index);

static  Intern_Pool intern_pool_make(Arena *arena, u64 capacity);
static          u64 intern_id(Intern_Pool *pool, String key, bool add);


#else // IMPLEMENTATION


#define MAP_MAX_LOAD_FACTOR 70

static u64 hash_djb2(String bytes) {
    u64 hash = 5381;
    for (u64 i = 0; i < bytes.count; i += 1) {
        hash += (hash << 5) + bytes.data[i];
    }
    return hash;
}

static u64 hash_lookup_msi(u64 hash, u64 exponent, u64 index) {
    u64 mask = ((u64)1 << exponent) - 1;
    u64 step = (hash >> (64 - exponent)) | 1;
    u64 new = (index + step) & mask;
    return new;
}

static Intern_Pool intern_pool_make(Arena *arena, u64 capacity) {
    Intern_Pool pool = { .keys.arena = arena };
    capacity *= 100 / MAP_MAX_LOAD_FACTOR;

    reserve(&pool.keys, capacity);
    pool.keys.count = 1;

    pool.key_index_from_hash = arena_make(pool.keys.arena, pool.keys.capacity, sizeof *pool.key_index_from_hash);

    return pool;
}

static u64 intern_id(Intern_Pool *pool, String key, bool add) {
    assert(is_power_of_2(pool->keys.capacity));
    u64 exponent = count_trailing_zeroes(pool->keys.capacity);
    u64 hash = hash_djb2(key);
    u64 *id = 0;
    for (u64 index = hash;;) {
        index = hash_lookup_msi(hash, exponent, index);
        index += !index; // never 0

        id = &pool->key_index_from_hash[index];

        if (*id == 0) break;

        String key_at_index = pool->keys.data[*id];
        if (string_equals(key_at_index, key)) break;
    }

    bool new = add && *id == 0;
    if (new) {
        ensure(pool->keys.count + 1 < pool->keys.capacity * MAP_MAX_LOAD_FACTOR / 100);
        *id = pool->keys.count;
        pool->keys.count += 1;
        pool->keys.data[*id] = key;
    }

    return *id;
}

#define map_make(arena, map, capacity) statement_macro( \
    static_assert(sizeof(*(map)) == sizeof(Map_void), "Parameter must be a Map"); \
    map_make_explicit_item_size((arena), (Map_void *)(map), (capacity), sizeof(*(map)->values.data)); \
)

static void map_make_explicit_item_size(Arena *arena, Map_void *map, u64 capacity, u64 item_size) {
    capacity *= 100;
    capacity /= MAP_MAX_LOAD_FACTOR;

    map->arena = arena;
    reserve_explicit_item_size(&map->values, capacity, item_size, false);

    map->count = 1;
    map->keys = arena_make(map->arena, capacity, sizeof *map->keys);
    map->value_index_from_key_hash = arena_make(map->arena, capacity, sizeof *map->value_index_from_key_hash);
}

#define map_get(map, key, ...) map_get_((Map_Get_Arguments){ \
    ._item_size = sizeof *(map)->values.data, \
    (Map_void *)(map), \
    key, \
    __VA_ARGS__ \
})

#define map_put(map_pointer, key, put, ...) map_get(map_pointer, key, \
    0 ? &(map_pointer)->data[-1] : put, /* type checking hack */ \
)

static Map_Result map_get_(Map_Get_Arguments arguments) {
    Map_void *map = arguments.map;
    u64 key = arguments.key;
    void *put = arguments.put;
    u64 item_size = arguments._item_size;

    Map_Result result = {0};

    assert(is_power_of_2(map->capacity));
    u64 exponent = count_trailing_zeroes(map->capacity);
    u64 hash = hash_djb2(as_bytes(&key));
    u64 *value_index = 0;
    for (u64 index = hash;;) {
        index = hash_lookup_msi(hash, exponent, index);
        index += !index; // never 0

        value_index = &map->value_index_from_key_hash[index];

        if (*value_index == 0) break;

        u64 key_at_index = map->keys[*value_index];
        if (key == key_at_index) break;
    }

    if (put != 0) {
        result.is_new = *value_index == 0;
        if (result.is_new) {
            ensure(map->count + 1 < map->capacity * MAP_MAX_LOAD_FACTOR / 100);

            *value_index = map->values.count;
            map->count += 1;
        }

        map->keys[*value_index] = key;

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
