// https://github.com/felix-u 2026-11-03
// Public domain. NO WARRANTY - use at your own risk.

#if !defined(SMALL_SNPRINTF_H)
#define SMALL_SNPRINTF_H


#if !defined(SMALL_SNPRINTF_NO_STD_INCLUDE)
    #include <stddef.h> // size_t
    #include <stdarg.h>
#endif

#if !defined(SMALL_SNPRINTF_FUNCTION)
    #define SMALL_SNPRINTF_FUNCTION
#endif

SMALL_SNPRINTF_FUNCTION int small_snprintf(char *buffer, size_t buffer_size, const char *format, ...);
SMALL_SNPRINTF_FUNCTION int small_vsnprintf(char *buffer, size_t buffer_size, const char *format, va_list arguments);


#endif // SMALL_SNPRINTF_H


#if defined(SMALL_SNPRINTF_IMPLEMENTATION)


#if !defined(SMALL_SNPRINTF_UNKNOWN_FORMAT_HANDLER)
    #define SMALL_SNPRINTF_UNKNOWN_FORMAT_HANDLER(buffer, buffer_size, format, index, arguments) ((void)(buffer), (void)(buffer_size), -1)
#endif

#if !defined(SMALL_SNPRINTF_ASSERT)
    #if defined(SMALL_SNPRINTF_NO_STD_INCLUDE)
        #error "provide a custom definition for SMALL_SNPRINTF_ASSERT, since you defined SMALL_SNPRINTF_NO_STD_INCLUDE"
    #else
        #include <assert.h>
        #define SMALL_SNPRINTF_ASSERT assert
    #endif
#endif

#define SMALL_SNPRINTF__U64_DIGIT_CAPACITY 64

SMALL_SNPRINTF_FUNCTION int small_snprintf(char *buffer, size_t buffer_size, const char *format, ...) {
    va_list arguments;
    va_start(arguments, format);
    int result = small_vsnprintf(buffer, buffer_size, format, arguments);
    va_end(arguments);
    return result;
}

static void small_snprintf__write(char *buffer, size_t buffer_size, int *written, const void *data, size_t size) {
    for (size_t i = 0; (size_t)(*written) + i + 1 < buffer_size && i < size; i += 1) {
        buffer[*written + (int)i] = ((const char *)data)[i];
    }
    *written += (int)size;
}

static void small_snprintf__write_repeated(char *buffer, size_t buffer_size, int *written, char character, int count) {
    for (int i = 0; (size_t)(*written) + (size_t)i + 1 < buffer_size && i < count; i += 1) {
        buffer[*written + i] = character;
    }
    if (count > 0) *written += count;
}

static void small_snprintf__write_field_start(char *buffer, size_t buffer_size, int *written, _Bool is_negative, int body_length, int width, char pad_char) {
    int sign_length = (int)is_negative;
    int pad_count = width - sign_length - body_length;
    if (pad_char == '0') {
        small_snprintf__write(buffer, buffer_size, written, "-", (size_t)sign_length);
        small_snprintf__write_repeated(buffer, buffer_size, written, '0', pad_count);
    } else {
        small_snprintf__write_repeated(buffer, buffer_size, written, ' ', pad_count);
        small_snprintf__write(buffer, buffer_size, written, "-", (size_t)sign_length);
    }
}

static int small_snprintf__digits_from_u64(char digits[SMALL_SNPRINTF__U64_DIGIT_CAPACITY], unsigned long long value, unsigned long long base, _Bool uppercase) {
    const char *character_from_digit = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
    int first_digit_index = SMALL_SNPRINTF__U64_DIGIT_CAPACITY;
    do {
        first_digit_index -= 1;
        digits[first_digit_index] = character_from_digit[value % base];
        value /= base;
    } while (value > 0);
    return first_digit_index;
}

SMALL_SNPRINTF_FUNCTION int small_vsnprintf(char *buffer, size_t buffer_size, const char *format, va_list arguments_parameter) {
    int written = 0;

    va_list arguments;
    va_copy(arguments, arguments_parameter);

    for (size_t i = 0; format[i] != 0; i += 1) {
        size_t start_index = i;
        while (format[i] != 0 && format[i] != '%') i += 1;

        _Bool non_format_chars = i > start_index;
        if (non_format_chars) {
            small_snprintf__write(buffer, buffer_size, &written, &format[start_index], i - start_index);
        }
        if (format[i] == 0) break;

        size_t percent_index = i;
        i += 1;
        int byte_length = 4;
        char type = 0;

        int width = 0;
        char pad_char = ' ';

        int precision = 0;
        _Bool is_custom_precision = 0;

        int *parsing = &width;

        parse_type: {
            type = format[i];

            _Bool is_zero_pad_flag = type == '0' && parsing == &width && width == 0;
            if ('0' <= type && type <= '9' && !is_zero_pad_flag) {
                *parsing *= 10;
                *parsing += (int)(type - '0');
                i += 1;
                goto parse_type;
            }

            switch (type) {
                case 'l': {
                    if (format[i + 1] == 'l') {
                        byte_length = sizeof(long long);
                        i += 1;
                    } else byte_length = sizeof(long);

                    i += 1;
                    goto parse_type;
                } break;
                case 'z': {
                    byte_length = sizeof(size_t);
                    i += 1;
                    goto parse_type;
                } break;
                case '0': {
                    pad_char = '0';
                    i += 1;
                    goto parse_type;
                } break;
                case '.': {
                    parsing = &precision;
                    is_custom_precision = 1;
                    i += 1;
                    goto parse_type;
                } break;
                case 'u': case 'd': case 'i': case 'x': case 'X': case 'o': case 'b':
                case 'c':
                case 's':
                case 'f': case 'F':
                case '%':
                    break; // everything is handled below - these just need to not error
                default: {
                    _Bool buffer_has_room = (size_t)written < buffer_size;
                    char *handler_buffer = buffer_has_room ? &buffer[written] : 0;
                    size_t handler_buffer_size = buffer_has_room ? buffer_size - (size_t)written : 0;
                    int written_by_handler = SMALL_SNPRINTF_UNKNOWN_FORMAT_HANDLER(handler_buffer, handler_buffer_size, format, &i, &arguments);

                    if (written_by_handler < 0) {
                        small_snprintf__write(buffer, buffer_size, &written, &format[percent_index], i - percent_index + 1);
                    } else written += written_by_handler;
                    continue;
                } break;
            }
        }

        precision += 6 * (int)!is_custom_precision;

        unsigned long long value = 0;
        _Bool unsigned_int_value_already_populated = 0;
        _Bool is_negative = 0;
        unsigned long long base = 10;
        _Bool is_uppercase = 0;

        switch (type) {
            case 'X': is_uppercase = 1; // fallthrough
            case 'x': base = 16; goto unsigned_int;
            case 'o': base = 8; goto unsigned_int;
            case 'b': base = 2; goto unsigned_int;
            case 'd': case 'i': {
                long long signed_value = 0;
                if (byte_length == 4) signed_value = va_arg(arguments, int);
                else {
                    SMALL_SNPRINTF_ASSERT(byte_length == 8);
                    signed_value = va_arg(arguments, long long);
                }

                is_negative = signed_value < 0;
                value = is_negative ? 0ull - (unsigned long long)signed_value : (unsigned long long)signed_value;
                unsigned_int_value_already_populated = 1;
            } // fallthrough
            case 'u': { unsigned_int:
                if (!unsigned_int_value_already_populated) {
                    if (byte_length == 4) value = va_arg(arguments, unsigned);
                    else {
                        SMALL_SNPRINTF_ASSERT(byte_length == 8);
                        value = va_arg(arguments, unsigned long long);
                    }
                }

                char digits[SMALL_SNPRINTF__U64_DIGIT_CAPACITY] = {0};
                int first_digit_index = small_snprintf__digits_from_u64(digits, value, base, is_uppercase);
                int digit_count = SMALL_SNPRINTF__U64_DIGIT_CAPACITY - first_digit_index;
                small_snprintf__write_field_start(buffer, buffer_size, &written, is_negative, digit_count, width, pad_char);
                small_snprintf__write(buffer, buffer_size, &written, &digits[first_digit_index], (size_t)digit_count);
            } break;
            case 'c': {
                unsigned c = va_arg(arguments, unsigned);
                char as_char = (char)c;
                small_snprintf__write_field_start(buffer, buffer_size, &written, 0, 1, width, ' ');
                small_snprintf__write(buffer, buffer_size, &written, &as_char, 1);
            } break;
            case 's': {
                const char *cstring = va_arg(arguments, const char *);
                if (cstring == 0) cstring = "(null)";

                size_t length = 0;
                while ((!is_custom_precision || length < (size_t)precision) && cstring[length] != 0) length += 1;

                small_snprintf__write_field_start(buffer, buffer_size, &written, 0, (int)length, width, ' ');
                small_snprintf__write(buffer, buffer_size, &written, cstring, length);
            } break;
            case 'F': is_uppercase = 1; // fallthrough
            case 'f': {
                union { double floating; unsigned long long bits; } representation = {0};
                representation.floating = va_arg(arguments, double);
                unsigned long long bits = representation.bits;

                long long biased_exponent = (long long)((bits >> 52) & 0x7ff);
                unsigned long long mantissa_mask = (unsigned long long)(-1) >> 12;
                unsigned long long mantissa = bits & mantissa_mask;

                is_negative = (bits >> 63) != 0;

                if (biased_exponent == 0x7ff) {
                    const char *inf = is_uppercase ? "INF" : "inf";
                    const char *nan = is_uppercase ? "NAN" : "nan";
                    const char *string = (mantissa == 0) ? inf : nan;
                    small_snprintf__write_field_start(buffer, buffer_size, &written, is_negative, 3, width, ' ');
                    small_snprintf__write(buffer, buffer_size, &written, string, 3);
                    break;
                }

                double absolute_value = is_negative ? -representation.floating : representation.floating;

                // NOTE(felix): this is the part responsible for only being able to represent integer parts up to UINT64T_MAX
                if (absolute_value >= (double)(unsigned long long)(-1)) {
                    small_snprintf__write_field_start(buffer, buffer_size, &written, is_negative, 5, width, ' ');
                    small_snprintf__write(buffer, buffer_size, &written, "(...)", 5);
                    break;
                }

                unsigned long long integer_part = (unsigned long long)absolute_value;
                double fraction = absolute_value - (double)integer_part;

                double half_of_last_digit = 0.5;
                for (int j = 0; j < precision; j += 1) half_of_last_digit /= 10.0;
                fraction += half_of_last_digit;
                if (fraction >= 1.0) {
                    integer_part += 1;
                    fraction -= 1.0;
                }

                char digits[SMALL_SNPRINTF__U64_DIGIT_CAPACITY] = {0};
                int first_digit_index = small_snprintf__digits_from_u64(digits, integer_part, 10, 0);
                int digit_count = SMALL_SNPRINTF__U64_DIGIT_CAPACITY - first_digit_index;
                int fraction_length = precision > 0 ? 1 + precision : 0;
                small_snprintf__write_field_start(buffer, buffer_size, &written, is_negative, digit_count + fraction_length, width, pad_char);
                small_snprintf__write(buffer, buffer_size, &written, &digits[first_digit_index], (size_t)digit_count);

                if (precision > 0) {
                    small_snprintf__write(buffer, buffer_size, &written, ".", 1);
                    for (int j = 0; j < precision; j += 1) {
                        fraction *= 10.0;
                        int digit = (int)fraction;
                        fraction -= (double)digit;
                        char digit_as_char = (char)('0' + digit);
                        small_snprintf__write(buffer, buffer_size, &written, &digit_as_char, 1);
                    }
                }
            } break;
            case '%': {
                small_snprintf__write(buffer, buffer_size, &written, "%", 1);
            } break;
            default: SMALL_SNPRINTF_ASSERT(0 && "unreachable"); break;
        }
    }

    va_end(arguments);

    if ((size_t)written < buffer_size) buffer[written] = 0;
    else if (buffer_size > 0) buffer[buffer_size - 1] = 0;

    return written;
}


#endif // SMALL_SNPRINTF_IMPLEMENTATION
