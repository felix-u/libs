#if defined(BASE_NO_IMPLEMENTATION) || defined(BASE_NO_IMPLEMENTATION_ARENA)

structdef(Arena) {
    void *mem;
    u64 offset;
    u64 capacity;
    u64 last_offset;
};

structdef(Scratch) {
    Arena *arena;
    u64 offset;
    u64 last_offset;
};

static void *arena_make(Arena *arena, u64 item_count, u64 item_size);
static void  arena_deinit(Arena *arena);

// TODO(felix): remove this function. A zeroed reserve+commit arena will be valid and will grow on demand
static Arena arena_init(u64 initial_size_bytes);

static String arena_push(Arena *arena, String bytes);

static Scratch scratch_begin(Arena *arena);
static void    scratch_end(Scratch scratch);

#if BUILD_ASAN
    #define asan_poison_memory_region(address, byte_count)   __asan_poison_memory_region(address, byte_count)
    #define asan_unpoison_memory_region(address, byte_count) __asan_unpoison_memory_region(address, byte_count)
    #include <sanitizer/asan_interface.h>
#else
    #define asan_poison_memory_region(address, byte_count)   statement_macro( discard(address); discard(byte_count); )
    #define asan_unpoison_memory_region(address, byte_count) statement_macro( discard(address); discard(byte_count); )
#endif // BUILD_ASAN


#else // IMPLEMENTATION


static Arena arena_init(u64 initial_size_bytes) {
    // TODO(felix): switch to reserve+commit with (virtually) no limit: reserve something like 64gb and commit pages as needed
    #if OS_WINDOWS
        Arena arena = { .mem = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, initial_size_bytes) };
    #elif OS_LINUX || OS_MACOS || OS_EMSCRIPTEN
        Arena arena = { .mem = calloc(initial_size_bytes, 1) };
    #else
        #error "unsupported OS"
    #endif

    if (arena.mem == 0) panic("allocation failure");
    arena.capacity = initial_size_bytes;
    asan_poison_memory_region(arena.mem, arena.capacity);
    return arena;
}

static void *arena_make(Arena *arena, u64 item_count, u64 item_size) {
    assert(item_size > 0);
    u64 byte_count = item_count * item_size;
    if (byte_count == 0) return 0;

    // TODO(felix): asan_poison alignment bytes
    u64 alignment = 2 * sizeof(void *);
    u64 modulo = arena->offset & (alignment - 1);
    if (modulo != 0) arena->offset += alignment - modulo;

    if (arena->capacity == 0) *arena = arena_init(8 * 1024 * 1024);

    if (arena->offset + byte_count > arena->capacity) panic("allocation failure");

    void *mem = (u8 *)arena->mem + arena->offset;
    arena->last_offset = arena->offset;
    arena->offset += byte_count;
    asan_unpoison_memory_region(mem, byte_count);
    return mem;
}

static void arena_deinit(Arena *arena) {
    asan_poison_memory_region(arena->mem, arena->capacity);

    #if OS_WINDOWS
        HeapFree(GetProcessHeap(), 0, arena->mem);
    #elif OS_LINUX || OS_MACOS || OS_EMSCRIPTEN
        free(arena->mem);
    #else
        #error "unsupported OS"
    #endif

    arena->mem = 0;
    arena->offset = 0;
    arena->capacity = 0;
}

static String arena_push(Arena *arena, String bytes) {
    u8 *bytes_on_arena = arena_make(arena, bytes.count, sizeof(*bytes.data));
    memcpy(bytes_on_arena, bytes.data, bytes.count);
    String result = { .data = bytes_on_arena, .count = bytes.count };
    return result;
}

static Scratch scratch_begin(Arena *arena) {
    return (Scratch){
        .arena = arena,
        .offset = arena->offset,
        .last_offset = arena->last_offset,
    };
}

static void scratch_end(Scratch scratch) {
    asan_poison_memory_region((u8 *)scratch.arena->mem + scratch.offset, scratch.arena->capacity - scratch.offset);
    scratch.arena->offset = scratch.offset;
    scratch.arena->last_offset = scratch.last_offset;
}

#endif // defined(BASE_NO_IMPLEMENTATION) || defined(BASE_NO_IMPLEMENTATION_ARENA)
