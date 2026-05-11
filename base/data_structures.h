// https://github.com/felix-u 2026-05-11
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

DS_FUNCTION ds_u64 ds_hash_fnv1a(const void *data, ds_size_t count);
DS_FUNCTION ds_u32 ds_hash_step_index(ds_u64 hash, int exponent, ds_u32 index);


#endif // DS_H


#if defined(DS_IMPLEMENTATION)


DS_FUNCTION ds_u64 ds_hash_fnv1a(const void *data, ds_size_t count) {
    ds_u64 hash = 14695981039346656037llu;
    const unsigned char *bytes = data;
    for (ds_size_t i = 0; i < count; i += 1) {
        hash ^= (ds_u64)bytes[i];
        hash *= 1099511628211;
    }
    return hash;
}

DS_FUNCTION ds_u32 ds_hash_step_index(ds_u64 hash, int exponent, ds_u32 index) {
    ds_u32 mask = ((ds_u32)1 << exponent) - 1;
    ds_u32 step = (ds_u32)(hash >> (64 - exponent)) | 1;
    return (index + step) & mask;
}


#endif // DS_IMPLEMENTATION
