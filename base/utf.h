// https://github.com/felix-u 2026-11-03
// Public domain. NO WARRANTY - use at your own risk.

#if !defined(UTF_H)
#define UTF_H


#if !defined(UTF_NO_SYSTEM_INCLUDE)
    #include <stddef.h> // size_t
#endif

#if !defined(UTF_FUNCTION)
    #define UTF_FUNCTION
#endif

typedef struct {
    size_t source_units_converted;
    size_t destination_units_written;
} utf_Conversion;

UTF_FUNCTION utf_Conversion utf16_from_utf8(unsigned short *destination, size_t destination_capacity, const char *source, size_t source_count);
UTF_FUNCTION utf_Conversion utf8_from_utf16(char *destination, size_t destination_capacity, const unsigned short *source, size_t source_count);


#endif // UTF_H


#if defined(UTF_IMPLEMENTATION)


#define UTF__REPLACEMENT_CHARACTER 0xfffd

UTF_FUNCTION utf_Conversion utf16_from_utf8(unsigned short *destination, size_t destination_capacity, const char *source, size_t source_count) {
    const unsigned char *bytes = (const unsigned char *)source;
    utf_Conversion conversion = {0};

    _Bool destination_has_room = 1;
    while (destination_has_room && conversion.source_units_converted < source_count) {
        size_t start = conversion.source_units_converted;
        unsigned char leading = bytes[start];

        size_t sequence_length = 1;
        unsigned codepoint = leading;
        unsigned char second_minimum = 0x80, second_maximum = 0xbf;
        if (0xc2 <= leading && leading <= 0xdf) {
            sequence_length = 2;
            codepoint = leading & 0x1f;
        } else if (0xe0 <= leading && leading <= 0xef) {
            sequence_length = 3;
            codepoint = leading & 0x0f;
            if (leading == 0xe0) second_minimum = 0xa0;
        } else if (0xf0 <= leading && leading <= 0xf4) {
            sequence_length = 4;
            codepoint = leading & 0x07;
            if (leading == 0xf0) second_minimum = 0x90;
            if (leading == 0xf4) second_maximum = 0x8f;
        }

        _Bool is_valid = leading < 0x80 || sequence_length > 1;
        size_t valid_length = 1;
        while (is_valid && valid_length < sequence_length) {
            size_t index = start + valid_length;
            unsigned char minimum = valid_length == 1 ? second_minimum : 0x80;
            unsigned char maximum = valid_length == 1 ? second_maximum : 0xbf;
            is_valid = index < source_count && minimum <= bytes[index] && bytes[index] <= maximum;
            if (is_valid) {
                codepoint = (codepoint << 6) | (bytes[index] & 0x3f);
                valid_length += 1;
            }
        }
        if (!is_valid) codepoint = UTF__REPLACEMENT_CHARACTER;

        size_t unit_count = codepoint >= 0x10000 ? 2 : 1;
        destination_has_room = destination == 0 || conversion.destination_units_written + unit_count <= destination_capacity;
        if (destination_has_room) {
            if (destination != 0) {
                if (unit_count == 2) {
                    unsigned offset = codepoint - 0x10000;
                    destination[conversion.destination_units_written] = (unsigned short)(0xd800 | (offset >> 10));
                    destination[conversion.destination_units_written + 1] = (unsigned short)(0xdc00 | (offset & 0x3ff));
                } else destination[conversion.destination_units_written] = (unsigned short)codepoint;
            }
            conversion.destination_units_written += unit_count;
            conversion.source_units_converted += valid_length;
        }
    }

    return conversion;
}

UTF_FUNCTION utf_Conversion utf8_from_utf16(char *destination, size_t destination_capacity, const unsigned short *source, size_t source_count) {
    utf_Conversion conversion = {0};

    _Bool destination_has_room = 1;
    while (destination_has_room && conversion.source_units_converted < source_count) {
        size_t start = conversion.source_units_converted;
        unsigned codepoint = source[start];
        size_t unit_count = 1;

        _Bool is_high_surrogate = 0xd800 <= codepoint && codepoint <= 0xdbff;
        _Bool has_low_surrogate = start + 1 < source_count && 0xdc00 <= source[start + 1] && source[start + 1] <= 0xdfff;
        if (is_high_surrogate && has_low_surrogate) {
            codepoint = 0x10000 + ((codepoint - 0xd800) << 10) + (source[start + 1] - 0xdc00u);
            unit_count = 2;
        }

        size_t byte_count = codepoint < 0x80 ? 1 : codepoint < 0x800 ? 2 : codepoint < 0x10000 ? 3 : 4;
        destination_has_room = destination == 0 || conversion.destination_units_written + byte_count <= destination_capacity;
        if (destination_has_room) {
            if (destination != 0) {
                unsigned char *bytes = (unsigned char *)destination + conversion.destination_units_written;
                switch (byte_count) {
                    case 1: {
                        bytes[0] = (unsigned char)codepoint;
                    } break;
                    case 2: {
                        bytes[0] = (unsigned char)(0xc0 | (codepoint >> 6));
                        bytes[1] = (unsigned char)(0x80 | (codepoint & 0x3f));
                    } break;
                    case 3: {
                        bytes[0] = (unsigned char)(0xe0 | (codepoint >> 12));
                        bytes[1] = (unsigned char)(0x80 | ((codepoint >> 6) & 0x3f));
                        bytes[2] = (unsigned char)(0x80 | (codepoint & 0x3f));
                    } break;
                    default: {
                        bytes[0] = (unsigned char)(0xf0 | (codepoint >> 18));
                        bytes[1] = (unsigned char)(0x80 | ((codepoint >> 12) & 0x3f));
                        bytes[2] = (unsigned char)(0x80 | ((codepoint >> 6) & 0x3f));
                        bytes[3] = (unsigned char)(0x80 | (codepoint & 0x3f));
                    } break;
                }
            }
            conversion.destination_units_written += byte_count;
            conversion.source_units_converted += unit_count;
        }
    }

    return conversion;
}


#endif // UTF_IMPLEMENTATION
