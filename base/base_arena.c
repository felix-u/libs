static Arena arena_init(usize size) {
    Arena arena = { .mem = calloc(1, size) };
    if (arena.mem == 0) err("allocation failure");
    else arena.cap = size;
    return arena;
}

static void arena_align(Arena *arena, usize align) {
    usize modulo = arena->offset & (align - 1);
    if (modulo != 0) arena->offset += align - modulo;
}

static void *arena_alloc(Arena *arena, usize cap, usize size) {
    usize num_bytes = cap * size;
    arena_align(arena, arena_default_alignment);
    if (arena->offset + num_bytes >= arena->cap) {
        err("allocation failure");
        return 0;
    }

    void *mem = (u8 *)arena->mem + arena->offset;
    arena->last_offset = arena->offset;
    arena->offset += num_bytes;
    return mem;
}

static void *arena_realloc(Arena *arena, void *mem, usize cap, usize size) {
    usize num_bytes = cap * size;
    void *last_allocation = (u8 *)arena->mem + arena->last_offset;
    if (mem == last_allocation && 
        arena->last_offset + num_bytes <= arena->cap
    ) {
        arena->offset = arena->last_offset + num_bytes;
        return mem;
    }
    return arena_alloc(arena, cap, size);
}

static void 
_arena_alloc_array(Arena *arena, Array_void *array, usize cap, usize size) {
    array->ptr = arena_alloc(arena, cap, size);
    array->len = 0;
    array->cap = (array->ptr == 0) ? 0 : cap;
}

static void 
_arena_realloc_array(Arena *arena, Array_void *array, usize cap, usize size) {
    void *old_ptr = array->ptr;
    array->ptr = arena_realloc(arena, array->ptr, cap, size);
    if (array->ptr == 0) {
        *array = (Array_void){0};
        return;
    }
    // memmove instead of memcpy because there is overlap (the ranges are
    // identical) if arena_realloc() didn't need to really realloc
    memmove(array->ptr, old_ptr, array->len * size);
    array->cap = cap;
}

static void arena_deinit(Arena *arena) {
    free(arena->mem);
    arena->offset = 0;
    arena->cap = 0;
}

static Arena_Temp arena_temp_begin(Arena *arena) {
    return (Arena_Temp){
        .arena = arena,
        .offset = arena->offset,
        .last_offset = arena->last_offset,
    };
}

static void arena_temp_end(Arena_Temp arena_temp) {
    arena_temp.arena->offset = arena_temp.offset;
    arena_temp.arena->last_offset = arena_temp.last_offset;
}
