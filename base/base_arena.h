structdef(Arena) {
    void *mem;
    usize offset;
    usize cap;
    usize last_offset;
};

structdef(Arena_Temp) {
    Arena *arena;
    usize offset;
    usize last_offset;
};

static Arena arena_init(usize size);
static void  arena_align(Arena *arena, usize align);
static void *arena_alloc(Arena *arena, usize cap, usize size); 
static void *arena_realloc(Arena *arena, void *mem, usize cap, usize size); 
static void  arena_deinit(Arena *arena); 

static void 
_arena_alloc_array(Arena *arena, Array_void *array, usize cap, usize size); 

static void 
_arena_realloc_array(Arena *arena, Array_void *array, usize cap, usize size); 

#define arena_default_alignment (2 * sizeof(void *))

#define arena_alloc_array(arena_ptr, array_ptr, cap) \
    _arena_alloc_array(\
        arena_ptr,\
        (Array_void *)(array_ptr),\
        cap,\
        sizeof(*((array_ptr)->ptr))\
    )

#define arena_realloc_array(arena_ptr, array_ptr, cap) \
    _arena_realloc_array(\
        arena_ptr,\
        (Array_void *)(array_ptr),\
        cap,\
        sizeof(*((array_ptr)->ptr))\
    )

static Arena_Temp arena_temp_begin(Arena *arena);
static void       arena_temp_end(Arena_Temp arena_temp);
