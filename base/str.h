// https://github.com/felix-u 2025-12-24
// Public domain. NO WARRANTY - use at your own risk.

#if !defined(STR_H)
#define STR_H


typedef struct {
    unsigned char *data;
    unsigned long long count;
} String;

#define STRING_LITERAL_CONSTANT(s)  { .data = (unsigned char *)(s), .count = sizeof(s) - 1 }
#define STRING_LITERAL(s) (String)STRING_LITERAL_CONSTANT(s)

#if !defined(STR_FUNCTION)
    #define STR_FUNCTION
#endif

STR_FUNCTION  _Bool string_equals(String a, String b);
STR_FUNCTION String string_from_cstring(const char *s);
STR_FUNCTION String string_range(String s, unsigned long long start, unsigned long long end);
STR_FUNCTION  _Bool string_starts_with(String s, String start);


#endif // STR_H


#if defined(STR_IMPLEMENTATION)


#if !defined(STR_ASSERT)
    #include <assert.h>
    #define STR_ASSERT assert
#endif

static _Bool string_equals(String a, String b) {
    if (a.count != b.count) return 0;
    for (unsigned long long i = 0; i < a.count; i += 1) {
        if (a.data[i] != b.data[i]) return 0;
    }
    return 1;
}

static String string_from_cstring(const char *s) {
    STR_ASSERT(s != 0);
    String result = { .data = (unsigned char *)s };
    while (s[result.count] != 0) result.count += 1;
    return result;
}

static String string_range(String s, unsigned long long start, unsigned long long end) {
    STR_ASSERT(end <= s.count);
    STR_ASSERT(start <= end);
    String result = { .data = s.data + start, .count = end - start };
    return result;
}

static _Bool string_starts_with(String s, String start) {
    if (s.count < start.count) return 0;
    for (u64 i = 0; i < start.count; i += 1) if (s.data[i] != start.data[i]) return 0;
    return 1;
}


#endif // STR_IMPLEMENTATION
