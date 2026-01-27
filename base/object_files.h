// https://github.com/felix-u 2026-01-26
// Public domain. NO WARRANTY - use at your own risk


// NOTE(felix): these are possible COFF references
// https://learn.microsoft.com/en-us/windows/win32/debug/pe-format
// https://wiki.osdev.org/COFF
// https://0xrick.github.io/win-internals/pe2/
// https://www.delorie.com/djgpp/doc/coff/


#if !defined(OBJECT_H)
#define OBJECT_H


#if !defined(OBJECT_FUNCTION)
    #define OBJECT_FUNCTION static
#endif

typedef enum {
    object_Bundle_COFF = 1 << 0,

    object_Bundle_READ = 1 << 1,
    object_Bundle_WRITE = 1 << 2,
} object_Bundle_Flags;

OBJECT_FUNCTION unsigned char *object_bundle(
    const char *array_name, unsigned array_name_length,
    const char *length_name, unsigned length_name_length,
    void *data, unsigned data_size,
    unsigned char *buffer, unsigned long long *buffer_size,
    object_Bundle_Flags flags
);

typedef unsigned short object_COFF_File_Header_Flags;
enum {
    object_COFF_File_Header_RELOCS_STRIPPED         = 0x0001,
    object_COFF_File_Header_EXECUTABLE_IMAGE        = 0x0002,
    object_COFF_File_Header_LINE_NUMS_STRIPPED      = 0x0004, // deprecated, should be 0
    object_COFF_File_Header_LOCAL_SYMS_STRIPPED     = 0x0008, // deprecated, should be 0
    object_COFF_File_Header_AGGRESSIVE_WS_TRIM      = 0x0010, // deprecated, should be 0
    object_COFF_File_Header_LARGE_ADDRESS_AWARE     = 0x0020,
    object_COFF_File_Header_BYTES_REVERSED_LO       = 0x0080, // deprecated, should be 0
    object_COFF_File_Header_32BIT_MACHINE           = 0x0100,
    object_COFF_File_Header_DEBUG_STRIPPED          = 0x0200,
    object_COFF_File_Header_REMOVABLE_RUN_FROM_SWAP = 0x0400,
    object_COFF_File_Header_NET_RUN_FROM_SWAP       = 0x0800,
    object_COFF_File_Header_SYSTEM                  = 0x1000,
    object_COFF_File_Header_DLL                     = 0x2000,
    object_COFF_File_Header_UP_SYSTEM_ONLY          = 0x4000,
    object_COFF_File_Header_BYTES_REVERSED_HI       = 0x8000, // deprecated, should be 0
};

typedef struct {
    unsigned short machine; // also given as "magic"
    unsigned short number_of_sections;
    unsigned time_date_stamp;
    unsigned pointer_to_symbol_table;
    unsigned number_of_symbols;
    unsigned short size_of_optional_header;
    object_COFF_File_Header_Flags flags;
} object_COFF_File_Header;

typedef unsigned object_COFF_Section_Header_Flags;
enum {
    object_COFF_Section_Header_TYPE_NO_PAD            = 0x00000008, // deprecated, use ALIGN_1BYTES
    object_COFF_Section_Header_CNT_CODE               = 0x00000020,
    object_COFF_Section_Header_CNT_INITIALIZED_DATA   = 0x00000040,
    object_COFF_Section_Header_CNT_UNINITIALIZED_DATA = 0x00000080,
    object_COFF_Section_Header_LNK_OTHER              = 0x00000100, // reserved for future use
    object_COFF_Section_Header_LNK_INFO               = 0x00000200,
    object_COFF_Section_Header_LNK_REMOVE             = 0x00000800,
    object_COFF_Section_Header_LNK_COMDAT             = 0x00001000,
    object_COFF_Section_Header_GPREL                  = 0x00008000,
    object_COFF_Section_Header_MEM_PURGEABLE          = 0x00020000, // reserved for future use
    object_COFF_Section_Header_MEM_16BIT              = 0x00020000, // reserved for future use
    object_COFF_Section_Header_MEM_LOCKED             = 0x00040000, // reserved for future use
    object_COFF_Section_Header_MEM_PRELOAD            = 0x00080000, // reserved for future use
    object_COFF_Section_Header_ALIGN_1BYTES           = 0x00100000,
    object_COFF_Section_Header_ALIGN_2BYTES           = 0x00200000,
    object_COFF_Section_Header_ALIGN_4BYTES           = 0x00300000,
    object_COFF_Section_Header_ALIGN_8BYTES           = 0x00400000,
    object_COFF_Section_Header_ALIGN_16BYTES          = 0x00500000,
    object_COFF_Section_Header_ALIGN_32BYTES          = 0x00600000,
    object_COFF_Section_Header_ALIGN_64BYTES          = 0x00700000,
    object_COFF_Section_Header_ALIGN_128BYTES         = 0x00800000,
    object_COFF_Section_Header_ALIGN_256BYTES         = 0x00900000,
    object_COFF_Section_Header_ALIGN_512BYTES         = 0x00A00000,
    object_COFF_Section_Header_ALIGN_1024BYTES        = 0x00B00000,
    object_COFF_Section_Header_ALIGN_2048BYTES        = 0x00C00000,
    object_COFF_Section_Header_ALIGN_4096BYTES        = 0x00D00000,
    object_COFF_Section_Header_ALIGN_8192BYTES        = 0x00E00000,
    object_COFF_Section_Header_LNK_NRELOC_OVFL        = 0x01000000,
    object_COFF_Section_Header_MEM_DISCARDABLE        = 0x02000000,
    object_COFF_Section_Header_MEM_NOT_CACHED         = 0x04000000,
    object_COFF_Section_Header_MEM_NOT_PAGED          = 0x08000000,
    object_COFF_Section_Header_MEM_SHARED             = 0x10000000,
    object_COFF_Section_Header_MEM_EXECUTE            = 0x20000000,
    object_COFF_Section_Header_MEM_READ               = 0x40000000,
};
// NOTE(felix): this bypasses error "enumerator value is not representable in the underlying type". I suppose the anonymous enum is treated as a signed int while parsing
#define object_COFF_Section_Header_MEM_WRITE 0x80000000

typedef struct {
    char name[8];
    unsigned virtual_size;
    unsigned virtual_address;
    unsigned size_of_raw_data;
    unsigned pointer_to_raw_data;
    unsigned pointer_to_relocations;
    unsigned pointer_to_line_numbers;
    unsigned short number_of_relocations;
    unsigned short number_of_line_numbers;
    object_COFF_Section_Header_Flags flags;
} object_COFF_Section_Header;

typedef union {
    char short_name[8];
    struct { unsigned zeroes, offset; } string_table;
} object_COFF_Symbol_Name;

// docs: "Microsoft tools rely on Visual C++ debug format for most symbolic information and generally use only four storage-class values: EXTERNAL (2), STATIC (3), FUNCTION (101), and STATIC (103)"
typedef unsigned char object_COFF_Symbol_Storage_Class;
enum {
    object_COFF_Symbol_Storage_Class_END_OF_FUNCTION     = 255,
    object_COFF_Symbol_Storage_Class_NULL                =   0,
    object_COFF_Symbol_Storage_Class_AUTOMATIC           =   1,
    object_COFF_Symbol_Storage_Class_EXTERNAL            =   2, // symbol.value = (symbol.section_number == 0) ? size : offset within section
    object_COFF_Symbol_Storage_Class_STATIC              =   3, // symbol.value = symbol offset within section
    object_COFF_Symbol_Storage_Class_REGISTER            =   4,
    object_COFF_Symbol_Storage_Class_EXTERNAL_DEF        =   5,
    object_COFF_Symbol_Storage_Class_LABEL               =   6,
    object_COFF_Symbol_Storage_Class_UNDEFINED_LABEL     =   7,
    object_COFF_Symbol_Storage_Class_MEMBER_OF_STRUCT    =   8,
    object_COFF_Symbol_Storage_Class_ARGUMENT            =   9,
    object_COFF_Symbol_Storage_Class_STRUCT_TAG          =  10,
    object_COFF_Symbol_Storage_Class_MEMBER_OF_UNION     =  11,
    object_COFF_Symbol_Storage_Class_UNION_TAG           =  12,
    object_COFF_Symbol_Storage_Class_TYPE_DEFINITION     =  13,
    object_COFF_Symbol_Storage_Class_UNDEFINED_STATIC    =  14,
    object_COFF_Symbol_Storage_Class_ENUM_TAG            =  15,
    object_COFF_Symbol_Storage_Class_MEMBER_OF_ENUM      =  16,
    object_COFF_Symbol_Storage_Class_REGISTER_PARAM      =  17,
    object_COFF_Symbol_Storage_Class_BIT_FIELD           =  18,
    object_COFF_Symbol_Storage_Class_BLOCK               = 100,
    object_COFF_Symbol_Storage_Class_FUNCTION            = 101,
    object_COFF_Symbol_Storage_Class_END_OF_STRUCT       = 102,
    object_COFF_Symbol_Storage_Class_CLASS_FILE          = 103,
    object_COFF_Symbol_Storage_Class_CLASS_SECTION       = 104,
    object_COFF_Symbol_Storage_Class_CLASS_WEAK_EXTERNAL = 105,
    object_COFF_Symbol_Storage_Class_CLASS_CLR_TOKEN     = 107,
};

typedef short object_COFF_Symbol_Section_Number;
enum {
    object_COFF_Symbol_Section_Number_UNDEFINED = 0,
    object_COFF_Symbol_Section_Number_ABSOLUTE = -1,
    object_COFF_Symbol_Section_Number_DEBUG = -2,
};

typedef unsigned char object_COFF_Symbol_Component_Type;
enum {
    // docs: "Microsoft tools generally do not use this field and set the LSB to 0"
    object_COFF_Symbol_Base_Type_NULL   =  0, // none or unknown
    object_COFF_Symbol_Base_Type_VOID   =  1, // no valid type; void pointers or functions
    object_COFF_Symbol_Base_Type_CHAR   =  2,
    object_COFF_Symbol_Base_Type_SHORT  =  3,
    object_COFF_Symbol_Base_Type_INT    =  4,
    object_COFF_Symbol_Base_Type_LONG   =  5,
    object_COFF_Symbol_Base_Type_FLOAT  =  6,
    object_COFF_Symbol_Base_Type_DOUBLE =  7,
    object_COFF_Symbol_Base_Type_STRUCT =  8,
    object_COFF_Symbol_Base_Type_UNION  =  9,
    object_COFF_Symbol_Base_Type_ENUM   = 10,
    object_COFF_Symbol_Base_Type_MOE    = 11, // member of enum
    object_COFF_Symbol_Base_Type_BYTE   = 12,
    object_COFF_Symbol_Base_Type_WORD   = 13,
    object_COFF_Symbol_Base_Type_UINT   = 14,
    object_COFF_Symbol_Base_Type_DWORD  = 15,

    // docs: "Microsoft tools use this field only to indicate whether the symbol is a function, so that the only two resulting values are 0x0 and 0x20 for the Type field. However, other tools can use this field to communicate more information."
    object_COFF_Symbol_Complex_Type_NULL     = 0, // no derived type; simple scalar variable
    object_COFF_Symbol_Complex_Type_POINTER  = 1,
    object_COFF_Symbol_Complex_Type_FUNCTION = 2,
    object_COFF_Symbol_Complex_Type_ARRAY    = 3,
};
typedef struct {
    object_COFF_Symbol_Component_Type complex;
    object_COFF_Symbol_Component_Type base;
} object_COFF_Symbol_Type;

typedef struct {
    object_COFF_Symbol_Name name;
    unsigned value;
    object_COFF_Symbol_Section_Number section_number;
    object_COFF_Symbol_Type type;
    object_COFF_Symbol_Storage_Class storage_class;
    unsigned char number_of_aux_symbols;
} object_COFF_Symbol;
#define OBJECT_COFF_SYMBOL_SIZE 18

#endif // OBJECT_H


#if defined(OBJECT_IMPLEMENTATION)


#if !defined(OBJECT_ASSERT)
    #include <assert.h>
    #define OBJECT_ASSERT assert
#endif

#if !defined(OBJECT_MEMCPY)
    // TODO(felix): is it ok to do possibly unaligned writes via assignment to a pointer, bypassing the need for memcpy?
    #define OBJECT_MEMCPY object__memcpy
    static inline void *object__memcpy(void *destination, const void *source, unsigned long long count) {
        for (unsigned long long i = 0; i < count; i += 1) ((char *)destination)[i] = ((char *)source)[i];
        return destination;
    }
#endif

static inline void object__write(unsigned char *buffer, unsigned long long *written, const void *data, unsigned long long size) {
    OBJECT_MEMCPY(buffer + *written, data, size);
    *written += size;
}

OBJECT_FUNCTION unsigned char *object_bundle(
    const char *array_name, unsigned array_name_length,
    const char *length_name, unsigned length_name_length,
    void *data, unsigned data_size,
    unsigned char *buffer, unsigned long long *buffer_size,
    object_Bundle_Flags flags
) {
    if (flags & object_Bundle_COFF) {
        unsigned long long data_size_without_length = (unsigned long long)data_size;
        data_size += sizeof(unsigned long long); // for length variable
        unsigned long long size_of_headers = sizeof(object_COFF_File_Header) + sizeof(object_COFF_Section_Header);

        // docs: "For object files, the value should be aligned on a 4-byte boundary for best performance."
        unsigned long long alignment_padding = (4 - (size_of_headers & 3)) & 3;

        _Bool array_name_is_in_string_table = array_name_length > 8;
        _Bool length_name_is_in_string_table = length_name_length > 8;
        unsigned string_table_size = sizeof string_table_size;
        string_table_size += (unsigned)array_name_is_in_string_table * (array_name_length + 1);
        string_table_size += (unsigned)length_name_is_in_string_table * (length_name_length + 1);

        unsigned long long bytes_needed = size_of_headers + alignment_padding + (unsigned long long)data_size;
        bytes_needed += 2 * sizeof(object_COFF_Symbol);
        bytes_needed += (unsigned long long)string_table_size;

        OBJECT_ASSERT(bytes_needed <= 0xffffffff && "COFF file offsets are 32-bit");

        if (buffer == 0 || *buffer_size < bytes_needed) {
            *buffer_size = bytes_needed;
            return 0;
        }

        unsigned long long written = 0;

        object_COFF_File_Header file_header = {
            .machine = 0, // unknown / applicable to any
            .number_of_sections = 1,
            .time_date_stamp = 0, // none
            .pointer_to_symbol_table = (unsigned)(size_of_headers + alignment_padding) + data_size,
            .number_of_symbols = 2,
            .size_of_optional_header = 0, // should be zero for object file
            .flags = object_COFF_File_Header_LARGE_ADDRESS_AWARE | object_COFF_File_Header_DEBUG_STRIPPED,
        };
        object__write(buffer, &written, &file_header, sizeof file_header);

        object_COFF_Section_Header section_header = {
            .virtual_size = 0, // should be 0 for object files
            .virtual_address = 0, // "for simplicity, compilers should set this to zero"
            .size_of_raw_data = data_size,
            .pointer_to_raw_data = (unsigned)(written + sizeof section_header + alignment_padding),
            .pointer_to_relocations = 0, // none
            .pointer_to_line_numbers = 0, // none
            .number_of_relocations = 0,
            .number_of_line_numbers = 0,
        };
        {
            _Bool read = !!(flags & object_Bundle_READ);
            _Bool write = !!(flags & object_Bundle_WRITE);

            if (write) OBJECT_MEMCPY(section_header.name, ".data", sizeof ".data" - 1);
            else {
                OBJECT_ASSERT(read);
                OBJECT_MEMCPY(section_header.name, ".rdata", sizeof ".rdata" - 1);
            }

            section_header.flags = object_COFF_Section_Header_CNT_INITIALIZED_DATA;
            section_header.flags |= read * object_COFF_Section_Header_MEM_READ;
            section_header.flags |= write * object_COFF_Section_Header_MEM_WRITE;

            object__write(buffer, &written, &section_header, sizeof section_header);
        }

        object__write(buffer, &written, "\0\0\0", alignment_padding);
        OBJECT_ASSERT((unsigned)written == section_header.pointer_to_raw_data);
        {
            unsigned long long data_size_u64 = (unsigned long long)data_size;
            object__write(buffer, &written, &data_size_u64, sizeof data_size_u64);

            object__write(buffer, &written, data, data_size_without_length);
        }
        OBJECT_ASSERT((unsigned)written == file_header.pointer_to_symbol_table);

        { // size of data
            object_COFF_Symbol symbol = {
                .value = 0, // offset in section header data
                .section_number = 1, // one-based index
                .type = {
                    .complex = object_COFF_Symbol_Complex_Type_NULL,
                    .base = object_COFF_Symbol_Base_Type_NULL, // no such thing as u64?
                },
                .storage_class = object_COFF_Symbol_Storage_Class_EXTERNAL,
                .number_of_aux_symbols = 0,
            };

            if (!length_name_is_in_string_table) OBJECT_MEMCPY(symbol.name.short_name, length_name, length_name_length);
            else symbol.name.string_table.offset = 4;

            object__write(buffer, &written, &symbol, OBJECT_COFF_SYMBOL_SIZE);
        }

        { // data array
            object_COFF_Symbol symbol = {
                .value = sizeof(unsigned long long), // offset in section header data
                .section_number = 1, // one-based index
                .type = {
                    .complex = object_COFF_Symbol_Complex_Type_ARRAY,
                    .base = object_COFF_Symbol_Base_Type_CHAR,
                },
                .storage_class = object_COFF_Symbol_Storage_Class_EXTERNAL,
                .number_of_aux_symbols = 0,
            };

            if (!array_name_is_in_string_table) OBJECT_MEMCPY(symbol.name.short_name, array_name, array_name_length);
            else if (length_name_is_in_string_table) symbol.name.string_table.offset = 4 + length_name_length + 1;
            else symbol.name.string_table.offset = 4;

            object__write(buffer, &written, &symbol, OBJECT_COFF_SYMBOL_SIZE);
        }

        unsigned string_table_begin = (unsigned)written;
        object__write(buffer, &written, &string_table_size, sizeof string_table_size);
        if (length_name_is_in_string_table) {
            object__write(buffer, &written, length_name, length_name_length);
            buffer[written++] = 0;
        }
        if (array_name_is_in_string_table) {
            object__write(buffer, &written, array_name, array_name_length);
            buffer[written++] = 0;
        }
        OBJECT_ASSERT(string_table_size == (unsigned)written - string_table_begin);

        *buffer_size = written;
        return buffer;
    }

    OBJECT_ASSERT(0 && "only COFF is supported for now");
    *buffer_size = 0;
    return 0;
}


#endif // OBJECT_IMPLEMENTATION
