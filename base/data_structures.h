// https://github.com/felix-u 2026-11-08
// Public domain. NO WARRANTY - use at your own risk

#if !defined(DS_H)
#define DS_H


#if !defined(ds_u64)
    #include <stdint.h>
    #include <stddef.h>
    #define ds_u32 uint32_t
    #define ds_u64 uint64_t
    #define ds_size_t size_t
#endif

#if !defined(DS_FUNCTION)
    #define DS_FUNCTION
#endif

typedef struct {
    ds_u32 hash;
    ds_u32 index; // The caller's array should reserve index 0 as an empty slot.
} ds_Hash_Slot;

typedef struct {
    unsigned char *memory;
    ds_size_t capacity;
    ds_size_t used;
} ds_Static_Buffer;

DS_FUNCTION ds_u64 ds_array_grow_capacity(ds_u64 capacity, ds_u64 minimum);
DS_FUNCTION void  *ds_array_resize(void *allocate_context, void *data, ds_size_t item_size, ds_u64 capacity, ds_u64 new_capacity);
DS_FUNCTION ds_u32 ds_hash_fnv1a(const void *data, ds_size_t count);
DS_FUNCTION ds_u64 ds_hash_fnv1a_64(const void *data, ds_u64 count);
DS_FUNCTION ds_u64 ds_hash_fnv1a_64_update(ds_u64 hash, const void *data, ds_u64 count);
DS_FUNCTION ds_u32 ds_hash_fnv1a_update(ds_u32 hash, const void *data, ds_size_t count);
DS_FUNCTION ds_u32 ds_hash_step_index(ds_u32 hash, int exponent, ds_u32 index);
DS_FUNCTION void  *ds_static_buffer_allocate(void *context, void *pointer, ds_size_t old_size, ds_size_t new_size);


#endif // DS_H


#if defined(DS_IMPLEMENTATION)


#if !defined(DS_ALLOCATE)
    #define DS_ALLOCATE(context, pointer, old_size, new_size) ds_static_buffer_allocate((context), (pointer), (old_size), (new_size))
#endif

DS_FUNCTION ds_u64 ds_array_grow_capacity(ds_u64 capacity, ds_u64 minimum) {
    ds_u64 result = capacity;
    if (result < minimum) {
        _Bool can_double_to_minimum = minimum <= ((ds_u64)1 << 62);
        if (can_double_to_minimum) {
            result += result == 0;
            while (result < minimum) result *= 2;
        } else result = minimum;
    }
    return result;
}

DS_FUNCTION void *ds_array_resize(void *allocate_context, void *data, ds_size_t item_size, ds_u64 capacity, ds_u64 new_capacity) {
    void *result = 0;

    _Bool size_fits = item_size != 0 && new_capacity <= (ds_size_t)(-1) / item_size;
    if (size_fits) {
        ds_size_t old_size = (ds_size_t)capacity * item_size;
        ds_size_t new_size = (ds_size_t)new_capacity * item_size;
        result = DS_ALLOCATE(allocate_context, data, old_size, new_size);
    }

    return result;
}

#define DS_HASH_FNV1A_OFFSET_BASIS 2166136261u

DS_FUNCTION ds_u32 ds_hash_fnv1a(const void *data, ds_size_t count) {
    return ds_hash_fnv1a_update(DS_HASH_FNV1A_OFFSET_BASIS, data, count);
}

#define DS_HASH_FNV1A_64_OFFSET_BASIS 14695981039346656037ull

DS_FUNCTION ds_u64 ds_hash_fnv1a_64(const void *data, ds_u64 count) {
    return ds_hash_fnv1a_64_update(DS_HASH_FNV1A_64_OFFSET_BASIS, data, count);
}

DS_FUNCTION ds_u64 ds_hash_fnv1a_64_update(ds_u64 hash, const void *data, ds_u64 count) {
    const unsigned char *bytes = data;
    for (ds_u64 i = 0; i < count; i += 1) {
        hash ^= bytes[i];
        hash *= 1099511628211ull;
    }
    return hash;
}

DS_FUNCTION ds_u32 ds_hash_fnv1a_update(ds_u32 hash, const void *data, ds_size_t count) {
    const unsigned char *bytes = data;
    for (ds_size_t i = 0; i < count; i += 1) {
        hash ^= (ds_u32)bytes[i];
        hash *= 16777619u;
    }
    return hash;
}

DS_FUNCTION ds_u32 ds_hash_step_index(ds_u32 hash, int exponent, ds_u32 index) {
    ds_u32 mask = ((ds_u32)1 << exponent) - 1;
    ds_u32 step = (hash >> (32 - exponent)) | 1;
    return (index + step) & mask;
}

#define DS__STATIC_BUFFER_ALIGNMENT 16

DS_FUNCTION void *ds_static_buffer_allocate(void *context, void *pointer, ds_size_t old_size, ds_size_t new_size) {
    ds_Static_Buffer *buffer = context;
    unsigned char *old_bytes = pointer;
    void *result = 0;

    _Bool is_last_allocation = old_bytes != 0 && (old_bytes + old_size) == (buffer->memory + buffer->used);
    if (is_last_allocation) {
        ds_size_t start = buffer->used - old_size;
        _Bool fits_in_place = new_size <= buffer->capacity - start;
        if (fits_in_place) {
            buffer->used = start + new_size;
            if (new_size != 0) result = pointer;
        }
    } else if (old_bytes != 0 && new_size != 0 && new_size <= old_size) {
        result = pointer;
    } else if (new_size != 0) {
        ds_size_t alignment_mask = DS__STATIC_BUFFER_ALIGNMENT - 1;
        ds_size_t start = (buffer->used + alignment_mask) & ~alignment_mask;
        _Bool fits = start <= buffer->capacity && new_size <= buffer->capacity - start;
        if (fits) {
            unsigned char *new_bytes = buffer->memory + start;
            ds_size_t copy_size = old_size < new_size ? old_size : new_size;
            for (ds_size_t i = 0; old_bytes != 0 && i < copy_size; i += 1) new_bytes[i] = old_bytes[i];

            buffer->used = start + new_size;
            result = new_bytes;
        }
    }

    return result;
}


#endif // DS_IMPLEMENTATION
