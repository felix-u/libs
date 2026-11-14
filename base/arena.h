// https://github.com/felix-u 2026-11-04
// Public domain. NO WARRANTY - use at your own risk.

#if !defined(ARENA_H)
#define ARENA_H


#if !defined(ARENA_NO_SYSTEM_INCLUDE)
    #include <stddef.h>
#endif

typedef struct {
    unsigned char *memory;
    size_t capacity;
    size_t committed;
    size_t offset;
    size_t last_offset;
} Arena;

typedef struct {
    Arena *arena;
    size_t offset;
} arena_Scratch;

#if !defined(ARENA_FUNCTION)
    #define ARENA_FUNCTION
#endif

#define arena_make(arena, item_count, T) (T *)arena_make_((arena), (item_count), sizeof(T), 1)

ARENA_FUNCTION void          arena_free(Arena *arena);
ARENA_FUNCTION void         *arena_make_(Arena *arena, size_t item_count, size_t item_size, _Bool should_zero);
ARENA_FUNCTION void         *arena_push(Arena *arena, const void *data, size_t byte_count);
ARENA_FUNCTION Arena         arena_reserve(size_t byte_count);
ARENA_FUNCTION arena_Scratch scratch_begin(Arena *arena);
ARENA_FUNCTION void          scratch_end(arena_Scratch scratch);


#endif // ARENA_H


#if defined(ARENA_IMPLEMENTATION)


#if defined(_WIN32)
    #define ARENA_OS_WINDOWS
#elif defined(__wasm__)
    #define ARENA_OS_WASM
#else
    #error "UNIMPLEMENTED"
#endif

#if !defined(ARENA_NO_SYSTEM_INCLUDE)
    #if defined(_MSC_VER)
        #pragma comment(lib, "Ntdll.lib")
    #endif
#endif

#if defined(ARENA_OS_WINDOWS)
    long NtAllocateVirtualMemory(void *ProcessHandle, void **BaseAddress, unsigned long long ZeroBits, unsigned long long *RegionSize, unsigned long AllocationType, unsigned long Protect);
    long NtFreeVirtualMemory(void *ProcessHandle, void **BaseAddress, unsigned long long *RegionSize, unsigned long FreeType);
#endif

#if !defined(ARENA_ASSERT)
    #include <assert.h>
    #define ARENA_ASSERT assert
#endif

#if !defined(ARENA_STATIC_ASSERT)
    #if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
        #define ARENA_STATIC_ASSERT(expression, message) _Static_assert(expression, message)
    #else
        #define ARENA__GLUE(a, b) a##b
        #define ARENA__GLUE_GLUE(a, b) ARENA__GLUE(a, b)
        #define ARENA_STATIC_ASSERT(expression, message) typedef char ARENA__GLUE_GLUE(arena__static_assert_line_, __LINE__)[(expression) ? 1 : -1]
    #endif
#endif

#if !defined(ARENA_PANIC)
    #define ARENA_PANIC(message) ARENA_ASSERT(0 && (message))
#endif

#if !defined(ARENA_ASAN_POISON)
    #define ARENA_ASAN_POISON(address, byte_count) ((void)(address), (void)(byte_count))
#endif

#if !defined(ARENA_ASAN_UNPOISON)
    #define ARENA_ASAN_UNPOISON(address, byte_count) ((void)(address), (void)(byte_count))
#endif

#if !defined(ARENA_ASAN)
    #define ARENA_ASAN 0
#endif

#if !defined(ARENA_DEFAULT_RESERVE_SIZE)
    #define ARENA_DEFAULT_RESERVE_SIZE ((size_t)1 << (sizeof(size_t) == 4 ? 26 : 32))
#endif

#if !defined(ARENA_COMMIT_SIZE)
    #define ARENA_COMMIT_SIZE ((size_t)256 * 1024)
#endif
ARENA_STATIC_ASSERT((ARENA_COMMIT_SIZE & (ARENA_COMMIT_SIZE - 1)) == 0, "ARENA_COMMIT_SIZE must be a power of 2");
ARENA_STATIC_ASSERT(ARENA_COMMIT_SIZE >= 4096, "ARENA_COMMIT_SIZE must be a multiple of the page size");

#if !defined(ARENA_MEMCPY)
    #define ARENA_MEMCPY arena__memcpy
    static void *arena__memcpy(void *destination, const void *source, size_t byte_count) {
        for (size_t i = 0; i < byte_count; i += 1) ((unsigned char *)destination)[i] = ((const unsigned char *)source)[i];
        return destination;
    }
#endif

#if !defined(ARENA_MEMSET)
    #define ARENA_MEMSET arena__memset
    static void *arena__memset(void *destination, int byte, size_t byte_count) {
        for (size_t i = 0; i < byte_count; i += 1) ((unsigned char *)destination)[i] = (unsigned char)byte;
        return destination;
    }
#endif

enum {
    Arena_NT_Memory_COMMIT  = 0x1000,
    Arena_NT_Memory_RESERVE = 0x2000,
    Arena_NT_Memory_RELEASE = 0x8000,

    Arena_NT_Protection_READ_WRITE = 0x04,
};

#define ARENA__CURRENT_PROCESS ((void *)-1)

#if defined(ARENA_OS_WASM)
    #if defined(__clang__)
        #if !defined(ARENA_WASM_HEAP_BASE)
            extern unsigned char __heap_base;
            #define ARENA_WASM_HEAP_BASE (&__heap_base)
        #endif
        #if !defined(ARENA_WASM_MEMORY_PAGE_COUNT)
            #define ARENA_WASM_MEMORY_PAGE_COUNT() __builtin_wasm_memory_size(0)
        #endif
        #if !defined(ARENA_WASM_MEMORY_GROW)
            #define ARENA_WASM_MEMORY_GROW(page_count) __builtin_wasm_memory_grow(0, (page_count))
        #endif
    #endif

    #if !defined(ARENA_WASM_HEAP_BASE) || !defined(ARENA_WASM_MEMORY_PAGE_COUNT) || !defined(ARENA_WASM_MEMORY_GROW)
        #error "define ARENA_WASM_HEAP_BASE, ARENA_WASM_MEMORY_PAGE_COUNT() and ARENA_WASM_MEMORY_GROW(page_count), with the semantics of clang's __heap_base, __builtin_wasm_memory_size and __builtin_wasm_memory_grow"
    #endif

    static unsigned char *arena__wasm_highest_reservation_end;

    #define ARENA__WASM_PAGE_SIZE 65536ull
    #define ARENA__WASM_ADDRESS_LIMIT (65536ull * ARENA__WASM_PAGE_SIZE)
#endif

static _Bool arena__commit(unsigned char *address, size_t byte_count) {
    _Bool ok = 0;

    #if defined(ARENA_OS_WINDOWS)
    {
        void *commit_address = address;
        unsigned long long commit_size = byte_count;
        ok = NtAllocateVirtualMemory(ARENA__CURRENT_PROCESS, &commit_address, 0, &commit_size, Arena_NT_Memory_COMMIT, Arena_NT_Protection_READ_WRITE) >= 0;
    }
    #elif defined(ARENA_OS_WASM)
    {
        unsigned long long required_size = (unsigned long long)(size_t)address + byte_count;
        unsigned long long current_size = (unsigned long long)ARENA_WASM_MEMORY_PAGE_COUNT() * ARENA__WASM_PAGE_SIZE;
        ok = required_size <= current_size;
        if (!ok) {
            unsigned long long page_count = (required_size - current_size + ARENA__WASM_PAGE_SIZE - 1) / ARENA__WASM_PAGE_SIZE;
            ok = ARENA_WASM_MEMORY_GROW((size_t)page_count) != (size_t)-1;
        }
    }
    #endif

    return ok;
}

ARENA_FUNCTION void arena_free(Arena *arena) {
    if (arena->memory != 0) {
        ARENA_ASAN_UNPOISON(arena->memory, arena->committed);

        #if defined(ARENA_OS_WINDOWS)
        {
            void *base_address = arena->memory;
            unsigned long long whole_reservation = 0;
            NtFreeVirtualMemory(ARENA__CURRENT_PROCESS, &base_address, &whole_reservation, Arena_NT_Memory_RELEASE);
        }
        #elif defined(ARENA_OS_WASM)
        {
            _Bool is_highest_reservation = arena->memory + arena->capacity == arena__wasm_highest_reservation_end;
            if (is_highest_reservation) arena__wasm_highest_reservation_end = arena->memory;
        }
        #endif
    }
    *arena = (Arena){0};
}

#if defined(ARENA_POISON_BYTE) && !ARENA_ASAN
    #define ARENA__FILL_INACCESSIBLE 1
#else
    #define ARENA__FILL_INACCESSIBLE 0
#endif

#define ARENA__ALIGNMENT (2 * sizeof(void *))

ARENA_FUNCTION void *arena_make_(Arena *arena, size_t item_count, size_t item_size, _Bool should_zero) {
    ARENA_ASSERT(arena != 0);
    ARENA_ASSERT(item_size > 0);
    if (item_count == 0) return 0;
    unsigned char *result = 0;

    _Bool size_fits = item_count <= (size_t)-1 / item_size;
    if (!size_fits) ARENA_PANIC("arena_make_: size overflow");
    size_t byte_count = item_count * item_size;

    if (size_fits && arena->memory == 0) *arena = arena_reserve(ARENA_DEFAULT_RESERVE_SIZE);
    _Bool have_memory = size_fits && arena->memory != 0;

    size_t pad = (ARENA__ALIGNMENT - (arena->offset & (ARENA__ALIGNMENT - 1))) & (ARENA__ALIGNMENT - 1);

    #if defined(ARENA_OS_WASM)
    {
        _Bool is_highest_reservation = arena->memory + arena->capacity == arena__wasm_highest_reservation_end;
        unsigned long long commit_mask = ARENA_COMMIT_SIZE - 1;
        unsigned long long required_capacity = ((unsigned long long)arena->offset + pad + byte_count + commit_mask) & ~commit_mask;
        _Bool can_grow = have_memory && is_highest_reservation && required_capacity > arena->capacity;
        can_grow = can_grow && (unsigned long long)(size_t)arena->memory + required_capacity <= ARENA__WASM_ADDRESS_LIMIT;
        if (can_grow) {
            arena->capacity = (size_t)required_capacity;
            arena__wasm_highest_reservation_end = arena->memory + arena->capacity;
        }
    }
    #endif

    size_t available = arena->capacity - arena->offset;
    _Bool fits_reserve = have_memory && pad <= available && byte_count <= available - pad;
    if (have_memory && !fits_reserve) ARENA_PANIC("arena_make_: out of reserved memory");

    _Bool fits_commit = fits_reserve;
    size_t end = arena->offset + pad + byte_count;
    if (fits_reserve && end > arena->committed) {
        size_t new_committed = (end + ARENA_COMMIT_SIZE - 1) & ~(ARENA_COMMIT_SIZE - 1);
        ARENA_ASSERT(new_committed <= arena->capacity);

        fits_commit = arena__commit(arena->memory + arena->committed, new_committed - arena->committed);

        if (fits_commit) {
            ARENA_ASAN_POISON(arena->memory + arena->committed, new_committed - arena->committed);
            arena->committed = new_committed;
        } else ARENA_PANIC("arena_make_: commit failure");
    }

    if (fits_commit) {
        #if ARENA__FILL_INACCESSIBLE
            ARENA_ASAN_UNPOISON(arena->memory + arena->offset, pad);
            ARENA_MEMSET(arena->memory + arena->offset, ARENA_POISON_BYTE, pad);
        #endif
        ARENA_ASAN_POISON(arena->memory + arena->offset, pad);
        arena->offset += pad;

        result = arena->memory + arena->offset;
        arena->last_offset = arena->offset;
        arena->offset += byte_count;

        ARENA_ASAN_UNPOISON(result, byte_count);
        ARENA_ASAN_POISON(arena->memory + arena->offset, arena->committed - arena->offset);

        if (should_zero) ARENA_MEMSET(result, 0, byte_count);
        #if defined(ARENA_POISON_BYTE)
            else ARENA_MEMSET(result, ARENA_POISON_BYTE, byte_count);
        #endif
    }

    return result;
}

ARENA_FUNCTION void *arena_push(Arena *arena, const void *data, size_t byte_count) {
    void *result = arena_make_(arena, byte_count, 1, 0);
    if (result != 0) ARENA_MEMCPY(result, data, byte_count);
    return result;
}

ARENA_FUNCTION Arena arena_reserve(size_t byte_count) {
    Arena arena = {0};

    size_t commit_mask = ARENA_COMMIT_SIZE - 1;
    _Bool size_can_round_up_to_commit_size = byte_count <= (size_t)-1 - commit_mask;
    _Bool size_valid = byte_count != 0 && size_can_round_up_to_commit_size;

    if (size_valid) {
        size_t capacity = (byte_count + commit_mask) & ~commit_mask;
        void *memory = 0;
        _Bool reserved = 0;

        #if defined(ARENA_OS_WINDOWS)
        {
            unsigned long long reserved_size = capacity;
            reserved = NtAllocateVirtualMemory(ARENA__CURRENT_PROCESS, &memory, 0, &reserved_size, Arena_NT_Memory_RESERVE, Arena_NT_Protection_READ_WRITE) >= 0;
        }
        #elif defined(ARENA_OS_WASM)
        {
            if (arena__wasm_highest_reservation_end == 0) arena__wasm_highest_reservation_end = ARENA_WASM_HEAP_BASE;
            reserved = (unsigned long long)(size_t)arena__wasm_highest_reservation_end + capacity <= ARENA__WASM_ADDRESS_LIMIT;
            if (reserved) {
                memory = arena__wasm_highest_reservation_end;
                arena__wasm_highest_reservation_end += capacity;
            }
        }
        #endif

        if (reserved) {
            arena.memory = memory;
            arena.capacity = capacity;
        } else ARENA_PANIC("arena_reserve: reservation failure");
    } else ARENA_PANIC("arena_reserve: invalid size");

    return arena;
}

ARENA_FUNCTION arena_Scratch scratch_begin(Arena *arena) {
    arena_Scratch scratch = { .arena = arena, .offset = arena->offset };
    return scratch;
}

ARENA_FUNCTION void scratch_end(arena_Scratch scratch) {
    Arena *arena = scratch.arena;
    _Bool is_reverse_order = scratch.offset <= arena->offset;
    ARENA_ASSERT(is_reverse_order);

    if (scratch.offset < arena->offset) {
        #if ARENA__FILL_INACCESSIBLE
        {
            ARENA_ASAN_UNPOISON(arena->memory + scratch.offset, arena->offset - scratch.offset);
            ARENA_MEMSET(arena->memory + scratch.offset, ARENA_POISON_BYTE, arena->offset - scratch.offset);
        }
        #endif
        ARENA_ASAN_POISON(arena->memory + scratch.offset, arena->offset - scratch.offset);

        arena->offset = scratch.offset;
        if (arena->last_offset > scratch.offset) arena->last_offset = scratch.offset;
    }
}


#endif // ARENA_IMPLEMENTATION
