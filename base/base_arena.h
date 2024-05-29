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
static void  arena_deinit(Arena *arena); 
static void *arena_alloc(Arena *arena, usize cap, usize size); 
static void *arena_realloc(Arena *arena, void *mem, usize cap, usize size); 

#define arena_alloc_array(arena_ptr, array_ptr, cap) \
    _arena_alloc_array(arena_ptr, (Array_void *)(array_ptr), cap, sizeof(*((array_ptr)->ptr)))
static void _arena_alloc_array(Arena *arena, Array_void *array, usize cap, usize size); 

#define arena_realloc_array(arena_ptr, array_ptr, cap) \
    _arena_realloc_array(arena_ptr, (Array_void *)(array_ptr), cap, sizeof(*((array_ptr)->ptr)))
static void _arena_realloc_array(Arena *arena, Array_void *array, usize cap, usize size); 

#define arena_default_alignment (2 * sizeof(void *))

static Arena_Temp arena_temp_begin(Arena *arena);
static void       arena_temp_end(Arena_Temp arena_temp);

// TODO: why no vork on clang?
#if BUILD_ASAN && !COMPILER_CLANG
    #define asan_poison_memory_region(addr, size)   __asan_poison_memory_region(addr, size)
    #define asan_unpoison_memory_region(addr, size) __asan_unpoison_memory_region(addr, size)
    #include <sanitizer/asan_interface.h>
#else
    #define asan_poison_memory_region(addr, size)   { discard(addr); discard(size); }
    #define asan_unpoison_memory_region(addr, size) { discard(addr); discard(size); }
#endif // BUILD_ASAN
