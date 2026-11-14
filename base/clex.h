// https://github.com/felix-u 2026-11-08
// Public domain. NO WARRANTY - use at your own risk

#if !defined(CLEX_H)
#define CLEX_H


typedef struct { const char *start, *end; } clex_Data;

typedef struct {
    clex_Data path;
    _Bool is_system;
} clex_Include;

typedef enum {
    clex_Token_ERROR = 0,

    // Preprocessor
    clex_Token_DEFINE,
    clex_Token_ELIF,
    clex_Token_ELSE,
    clex_Token_ENDIF,
    clex_Token_ERROR_DIRECTIVE,
    clex_Token_IF,
    clex_Token_IFDEF,
    clex_Token_IFNDEF,
    clex_Token_INCLUDE,
    clex_Token_PRAGMA,
    clex_Token_UNDEF,

    clex_Token_COMMENT,
    clex_Token_IDENTIFIER,
    clex_Token_INTEGER,
    clex_Token_FLOAT,
    clex_Token_STRING,
    clex_Token_CHARACTER,

    clex_Token__ASCII_BEGIN = ' ' - 1,
    clex_Token_NOT                  = '!',
    clex_Token_HASHTAG              = '#',
    clex_Token_MODULO               = '%',
    clex_Token_AMPERSAND            = '&',
    clex_Token_OPEN_PAREN           = '(',
    clex_Token_CLOSE_PAREN          = ')',
    clex_Token_ASTERISK             = '*',
    clex_Token_PLUS                 = '+',
    clex_Token_COMMA                = ',',
    clex_Token_MINUS                = '-',
    clex_Token_DOT                  = '.',
    clex_Token_DIVIDE               = '/',
    clex_Token_COLON                = ':',
    clex_Token_SEMICOLON            = ';',
    clex_Token_LESS_THAN            = '<',
    clex_Token_EQUALS               = '=',
    clex_Token_GREATER_THAN         = '>',
    clex_Token_QUESTION             = '?',
    clex_Token_OPEN_SQUARE_BRACKET  = '[',
    clex_Token_CLOSE_SQUARE_BRACKET = ']',
    clex_Token_CARET                = '^',
    clex_Token_OPEN_CURLY           = '{',
    clex_Token_PIPE                 = '|',
    clex_Token_CLOSE_CURLY          = '}',
    clex_Token_BIT_NOT              = '~',
    clex_Token__ASCII_END = 128,

    clex_Token__DOUBLE_OPERATOR,
    clex_Token_AND            = '&' + clex_Token__DOUBLE_OPERATOR,
    clex_Token_PLUS_PLUS      = '+' + clex_Token__DOUBLE_OPERATOR,
    clex_Token_MINUS_MINUS    = '-' + clex_Token__DOUBLE_OPERATOR,
    clex_Token_LEFT_SHIFT     = '<' + clex_Token__DOUBLE_OPERATOR,
    clex_Token_EQUALITY_CHECK = '=' + clex_Token__DOUBLE_OPERATOR,
    clex_Token_RIGHT_SHIFT    = '>' + clex_Token__DOUBLE_OPERATOR,
    clex_Token_OR             = '|' + clex_Token__DOUBLE_OPERATOR,
    clex_Token__DOUBLE_OPERATOR_END = clex_Token__DOUBLE_OPERATOR + 128,

    clex_Token__ASSIGNMENT_OPERATOR,
    clex_Token_NOT_EQUALS            = '!' + clex_Token__ASSIGNMENT_OPERATOR,
    clex_Token_MODULO_ASSIGN         = '%' + clex_Token__ASSIGNMENT_OPERATOR,
    clex_Token_BIT_AND_ASSIGN        = '&' + clex_Token__ASSIGNMENT_OPERATOR,
    clex_Token_MULTIPLY_ASSIGN       = '*' + clex_Token__ASSIGNMENT_OPERATOR,
    clex_Token_PLUS_ASSIGN           = '+' + clex_Token__ASSIGNMENT_OPERATOR,
    clex_Token_MINUS_ASSIGN          = '-' + clex_Token__ASSIGNMENT_OPERATOR,
    clex_Token_DIVIDE_ASSIGN         = '/' + clex_Token__ASSIGNMENT_OPERATOR,
    clex_Token_LESS_THAN_OR_EQUAL    = '<' + clex_Token__ASSIGNMENT_OPERATOR,
    clex_Token_GREATER_THAN_OR_EQUAL = '>' + clex_Token__ASSIGNMENT_OPERATOR,
    clex_Token_XOR_ASSIGN            = '^' + clex_Token__ASSIGNMENT_OPERATOR,
    clex_Token_BIT_OR_ASSIGN         = '|' + clex_Token__ASSIGNMENT_OPERATOR,
    clex_Token__ASSIGNMENT_OPERATOR_END = clex_Token__ASSIGNMENT_OPERATOR + 128,

    clex_Token__ASSIGNMENT_DOUBLE_OPERATOR = clex_Token__DOUBLE_OPERATOR + clex_Token__ASSIGNMENT_OPERATOR,
    clex_Token_LEFT_SHIFT_ASSIGN  = '<' + clex_Token__ASSIGNMENT_DOUBLE_OPERATOR,
    clex_Token_RIGHT_SHIFT_ASSIGN = '>' + clex_Token__ASSIGNMENT_DOUBLE_OPERATOR,
    clex_Token__ASSIGNMENT_DOUBLE_OPERATOR_END,

    clex_Token_ARROW,
    clex_Token_ELLIPSIS,

    clex_Token_END_OK,
} clex_Token_Kind;

typedef unsigned short clex_Token_Flags;
enum {
    clex_Token_Flag_SUFFIX_FLOAT           = 1 << 0,
    clex_Token_Flag_SUFFIX_UNSIGNED        = 1 << 1,
    clex_Token_Flag_SUFFIX_LONG            = 1 << 2,
    clex_Token_Flag_SUFFIX_LONG_LONG       = 1 << 3,
    clex_Token_Flag_SUFFIX_BIT_PRECISE_INT = 1 << 4,

    clex_Token_Flag_BASE_2  = 1 << 5,
    clex_Token_Flag_BASE_8  = 1 << 6,
    clex_Token_Flag_BASE_10 = 1 << 7,
    clex_Token_Flag_BASE_16 = 1 << 8,

    clex_Token_Flag_PREFIX_UTF8  = 1 <<  9,
    clex_Token_Flag_PREFIX_UTF16 = 1 << 10,
    clex_Token_Flag_PREFIX_UTF32 = 1 << 11,
    clex_Token_Flag_PREFIX_WIDE  = 1 << 12,

    clex_Token_Flag_UNKNOWN_VALUE = 1 << 13,
};

typedef struct {
    clex_Token_Kind kind;
    clex_Data data, data2;
    clex_Token_Flags flags;
} clex_Token;

typedef enum {
    clex_Flag_PARSE_COMMENTS = 1 << 0,
} clex_Flags;

#if !defined(CLEX_EXPANSION_STACK_DEPTH)
    // TODO(felix): Measure the typical nesting depth of real system headers.
    #define CLEX_EXPANSION_STACK_DEPTH 64
#endif

typedef struct {
    const char *path, *path_end;
    const char *start, *end;
    const char *c;
    clex_Data macro_name;
} clex_Cursor;

typedef struct {
    clex_Cursor stack[CLEX_EXPANSION_STACK_DEPTH];
    int depth;
    clex_Flags flags;
    clex_Token token;
    const char *error_message;
} clex_Lexer;

typedef enum {
    clex_Node_LITERAL,
    clex_Node_NAME,
    clex_Node_PREFIX,
    clex_Node_INFIX,
    clex_Node_TERNARY,

    clex_Node_Kind_COUNT,
} clex_Node_Kind;

typedef struct {
    clex_Node_Kind kind;
    unsigned token_index;
    unsigned subtree_size;
} clex_Node;

typedef struct {
    const clex_Token *tokens;
    unsigned token_count;
    unsigned token_index;
    clex_Node *nodes;
    unsigned node_capacity;
    unsigned node_count;
    unsigned depth;
} clex_Parser;

#if !defined(CLEX_PARSE_DEPTH_MAX)
    #define CLEX_PARSE_DEPTH_MAX 256
#endif

typedef struct {
    long long value;
    _Bool is_known;
    unsigned error_token_index;
} clex_Evaluation;

typedef enum {
    clex_Condition_Value_FALSE,
    clex_Condition_Value_TRUE,
    clex_Condition_Value_UNKNOWN,
} clex_Condition_Value;

typedef enum {
    clex_Conditional_State_NOT_TAKEN,
    clex_Conditional_State_MAYBE_TAKEN,
    clex_Conditional_State_TAKEN,
} clex_Conditional_State;

typedef struct {
    clex_Conditional_State state;
    _Bool saw_else;
} clex_Conditional;

typedef enum {
    clex_Condition_Request_TOKEN_MEMORY,
    clex_Condition_Request_NAME,
    clex_Condition_Request_DEFINED,
    clex_Condition_Request_BUILTIN,

    clex_Condition_Request_COUNT,
} clex_Condition_Request;

typedef enum {
    clex_Condition_Error_NONE,
    clex_Condition_Error_EXPANSION_DEPTH,
    clex_Condition_Error_DEFINED_OPERAND,
    clex_Condition_Error_BUILTIN_OPERAND,
    clex_Condition_Error_FUNCTION_LIKE_MACRO,
    clex_Condition_Error_INVALID_TOKEN,
    clex_Condition_Error_INVALID_EXPRESSION,
    clex_Condition_Error_EVALUATION,

    clex_Condition_Error_COUNT,
} clex_Condition_Error;

typedef struct {
    clex_Lexer line;
    clex_Token token;
    clex_Token *tokens;
    unsigned token_count;
    unsigned long long token_capacity;

    clex_Condition_Request request;
    clex_Data name;

    clex_Condition_Error error;
    clex_Data error_text;
} clex_Condition;

#if !defined(CLEX_FUNCTION)
    #define CLEX_FUNCTION
#endif

CLEX_FUNCTION void                 clex_condition_answer_definition(clex_Condition *c, clex_Data definition);
CLEX_FUNCTION void                 clex_condition_answer_value(clex_Condition *c, clex_Condition_Value value);
CLEX_FUNCTION clex_Condition_Value clex_condition_evaluate(clex_Condition *c, clex_Node *nodes);
CLEX_FUNCTION clex_Condition       clex_condition_init(clex_Data text);
CLEX_FUNCTION _Bool                clex_condition_step(clex_Condition *c);
CLEX_FUNCTION _Bool                clex_conditional_enter(clex_Conditional *conditional, clex_Condition_Value value, _Bool is_else);
CLEX_FUNCTION clex_Data            clex_directive_name(clex_Data operand);
CLEX_FUNCTION clex_Evaluation      clex_evaluate_preprocessor_expression(const clex_Token *tokens, const clex_Node *nodes, unsigned root, _Bool *ok);
CLEX_FUNCTION int                  clex_get_current_location(const clex_Lexer *l, int *column, const char **path, const char **path_end);
CLEX_FUNCTION clex_Include         clex_include_operand(clex_Data operand, _Bool *ok);
CLEX_FUNCTION clex_Lexer           clex_init(const char *path, const char *path_end, const char *start, const char *end, clex_Flags flags);
CLEX_FUNCTION _Bool                clex_lex(clex_Lexer *l);
CLEX_FUNCTION unsigned             clex_parse_expression(clex_Parser *p, _Bool *ok);
CLEX_FUNCTION _Bool                clex_push_expansion(clex_Lexer *l, clex_Data name, clex_Data definition, _Bool *ok);
CLEX_FUNCTION _Bool                clex_push_file(clex_Lexer *l, const char *path, const char *path_end, const char *start, const char *end);
CLEX_FUNCTION _Bool                clex_skip_conditional_group(clex_Lexer *l);


#endif // CLEX_H


#if defined(CLEX_IMPLEMENTATION)


#if !defined(CLEX_ASSERT)
    #include <assert.h>
    #define CLEX_ASSERT assert
#endif

static void clex__condition_push(clex_Condition *c, clex_Token token) {
    CLEX_ASSERT(c->token_count < c->token_capacity);
    c->tokens[c->token_count] = token;
    c->token_count += 1;
}

CLEX_FUNCTION void clex_condition_answer_definition(clex_Condition *c, clex_Data definition) {
    _Bool is_function_like = definition.start < definition.end && definition.start[0] == '(';

    if (is_function_like) {
        c->error = clex_Condition_Error_FUNCTION_LIKE_MACRO;
        c->error_text = c->name;
    } else {
        _Bool expansion_ok = 1;
        _Bool expanded = clex_push_expansion(&c->line, c->name, definition, &expansion_ok);
        if (!expansion_ok) c->error = clex_Condition_Error_EXPANSION_DEPTH;
        else if (!expanded) clex__condition_push(c, c->token);
    }
}

CLEX_FUNCTION void clex_condition_answer_value(clex_Condition *c, clex_Condition_Value value) {
    clex_Token token = c->token;

    if (c->request != clex_Condition_Request_NAME) {
        static const char integer_text[] = "01";
        _Bool is_true = value == clex_Condition_Value_TRUE;
        token = (clex_Token){
            .kind = clex_Token_INTEGER,
            .data = { .start = &integer_text[is_true], .end = &integer_text[is_true + 1] },
            .flags = clex_Token_Flag_BASE_10,
        };
    }

    if (value == clex_Condition_Value_UNKNOWN) token.flags |= clex_Token_Flag_UNKNOWN_VALUE;
    clex__condition_push(c, token);
}

CLEX_FUNCTION clex_Condition_Value clex_condition_evaluate(clex_Condition *c, clex_Node *nodes) {
    clex_Condition_Value value = clex_Condition_Value_FALSE;

    if (c->error == clex_Condition_Error_NONE && c->line.token.kind != clex_Token_END_OK) c->error = clex_Condition_Error_INVALID_TOKEN;

    if (c->error == clex_Condition_Error_NONE) {
        clex_Parser parser = { .tokens = c->tokens, .token_count = c->token_count, .nodes = nodes, .node_capacity = c->token_count };
        _Bool parse_ok = 0;
        unsigned root = clex_parse_expression(&parser, &parse_ok);
        parse_ok = parse_ok && parser.token_index == c->token_count;

        unsigned error_token_index = parser.token_index;
        if (parse_ok) {
            _Bool evaluate_ok = 0;
            clex_Evaluation evaluation = clex_evaluate_preprocessor_expression(c->tokens, nodes, root, &evaluate_ok);
            error_token_index = evaluation.error_token_index;

            if (!evaluate_ok) c->error = clex_Condition_Error_EVALUATION;
            else if (!evaluation.is_known) value = clex_Condition_Value_UNKNOWN;
            else if (evaluation.value != 0) value = clex_Condition_Value_TRUE;
        } else c->error = clex_Condition_Error_INVALID_EXPRESSION;

        if (c->error != clex_Condition_Error_NONE && error_token_index < c->token_count) c->error_text = c->tokens[error_token_index].data;
    }

    return value;
}

CLEX_FUNCTION clex_Condition clex_condition_init(clex_Data text) {
    clex_Condition c = { .line = clex_init(0, 0, text.start, text.end, 0) };
    return c;
}

static _Bool clex__string_equals(const char *start, const char *end, const char *compare) {
    for (const char *c = start; c < end; c += 1) {
        unsigned long long i = (unsigned long long)(c - start);
        if (*c != compare[i]) return 0;
    }
    _Bool compare_has_same_length = compare[end - start] == 0;
    return compare_has_same_length;
}

static _Bool clex__data_starts_with(clex_Data data, const char *prefix) {
    const char *c = data.start;
    while (*prefix != 0 && c < data.end && *c == *prefix) {
        c += 1;
        prefix += 1;
    }
    return *prefix == 0;
}

// TODO(felix): Expand function-like macros in `#if`.
CLEX_FUNCTION _Bool clex_condition_step(clex_Condition *c) {
    _Bool has_request = 0;

    for (_Bool lexing = c->error == clex_Condition_Error_NONE; lexing;) {
        _Bool has_room = c->token_count < c->token_capacity;
        if (!has_room) {
            c->request = clex_Condition_Request_TOKEN_MEMORY;
            has_request = 1;
        } else if (!clex_lex(&c->line)) lexing = 0;
        else {
            clex_Token token = c->line.token;
            _Bool is_identifier = token.kind == clex_Token_IDENTIFIER;
            _Bool is_defined_operator = is_identifier && clex__string_equals(token.data.start, token.data.end, "defined");
            _Bool is_builtin = is_identifier && (clex__data_starts_with(token.data, "__has_") || clex__data_starts_with(token.data, "__is_target_"));

            if (is_defined_operator) {
                _Bool has_parenthesis = clex_lex(&c->line) && c->line.token.kind == '(';
                if (has_parenthesis) clex_lex(&c->line);
                _Bool operand_ok = c->line.token.kind == clex_Token_IDENTIFIER;
                c->name = c->line.token.data;
                if (has_parenthesis) operand_ok = operand_ok && clex_lex(&c->line) && c->line.token.kind == ')';

                c->request = clex_Condition_Request_DEFINED;
                has_request = operand_ok;
                if (!operand_ok) c->error = clex_Condition_Error_DEFINED_OPERAND;
            } else if (is_builtin) {
                _Bool operand_ok = clex_lex(&c->line) && c->line.token.kind == '(';
                for (unsigned nesting = 1; operand_ok && nesting > 0;) {
                    operand_ok = clex_lex(&c->line);
                    nesting += c->line.token.kind == '(';
                    nesting -= c->line.token.kind == ')';
                }

                c->name = token.data;
                c->request = clex_Condition_Request_BUILTIN;
                has_request = operand_ok;
                if (!operand_ok) c->error = clex_Condition_Error_BUILTIN_OPERAND;
            } else if (is_identifier) {
                c->token = token;
                c->name = token.data;
                c->request = clex_Condition_Request_NAME;
                has_request = 1;
            } else clex__condition_push(c, token);
        }

        lexing = lexing && !has_request && c->error == clex_Condition_Error_NONE;
    }

    return has_request;
}

CLEX_FUNCTION _Bool clex_conditional_enter(clex_Conditional *conditional, clex_Condition_Value value, _Bool is_else) {
    _Bool chain_is_decided = conditional->state == clex_Conditional_State_TAKEN;
    _Bool read_group = !chain_is_decided && value != clex_Condition_Value_FALSE;
    if (read_group) conditional->state = value == clex_Condition_Value_TRUE ? clex_Conditional_State_TAKEN : clex_Conditional_State_MAYBE_TAKEN;
    conditional->saw_else = is_else;
    return read_group;
}

static _Bool clex__is_nondigit(char c) {
    _Bool is = c == '_';
    is = is || ('A' <= c && c <= 'Z');
    is = is || ('a' <= c && c <= 'z');
    return is;
}

static _Bool clex__is_digit(char c) { return '0' <= c && c <= '9'; }

CLEX_FUNCTION clex_Data clex_directive_name(clex_Data operand) {
    clex_Data name = { .start = operand.start, .end = operand.start };
    _Bool starts_identifier = operand.start < operand.end && (clex__is_nondigit(*operand.start) || *operand.start == '$');
    if (starts_identifier) {
        name.end += 1;
        while (name.end < operand.end && (clex__is_nondigit(*name.end) || clex__is_digit(*name.end) || *name.end == '$')) name.end += 1;
    }
    return name;
}

typedef struct {
    long long value;
    _Bool is_known;
} clex__Value;

static clex__Value clex__evaluate(const clex_Token *tokens, const clex_Node *nodes, unsigned node_index, unsigned *error_token_index, _Bool *ok);

static unsigned clex__digit_value(char c) {
    unsigned decimal_value = (unsigned)(c - '0');
    unsigned letter_value = (unsigned)((c | 0x20) - 'a');
    unsigned value = 36;
    if (decimal_value < 10) value = decimal_value;
    else if (letter_value < 26) value = letter_value + 10;
    return value;
}

static clex__Value clex__evaluate_speculatively(const clex_Token *tokens, const clex_Node *nodes, unsigned node_index) {
    _Bool speculative_ok = 1;
    unsigned ignored_error_token_index = 0;
    clex__Value result = clex__evaluate(tokens, nodes, node_index, &ignored_error_token_index, &speculative_ok);
    result.is_known = result.is_known && speculative_ok;
    return result;
}

// TODO(felix): C11 6.10.1p4: Evaluate unsigned operands, including those with `u` suffixes, as uintmax_t, with the usual arithmetic conversions.
static clex__Value clex__evaluate(const clex_Token *tokens, const clex_Node *nodes, unsigned node_index, unsigned *error_token_index, _Bool *ok) {
    clex_Node node = nodes[node_index];
    clex_Token token = tokens[node.token_index];
    clex_Token_Kind operator = token.kind;
    unsigned last_child = node_index - 1;
    unsigned long long result = 0;
    _Bool is_known = 1;
    _Bool node_ok = 1;

    switch (node.kind) {
        case clex_Node_LITERAL: case clex_Node_NAME: {
            is_known = !(token.flags & clex_Token_Flag_UNKNOWN_VALUE);
            if (is_known && node.kind == clex_Node_LITERAL) {
                node_ok = token.kind == clex_Token_INTEGER || token.kind == clex_Token_CHARACTER;

                // TODO(felix): Report integer constants that overflow.
                // TODO(felix): Character constants with more than one character, with an encoding prefix, or of type char where char is signed.
                if (token.kind == clex_Token_INTEGER) {
                    unsigned base = 10;
                    if (token.flags & clex_Token_Flag_BASE_2) base = 2;
                    if (token.flags & clex_Token_Flag_BASE_8) base = 8;
                    if (token.flags & clex_Token_Flag_BASE_16) base = 16;
                    unsigned prefix_size = (base == 2 || base == 16) ? 2 : 0;

                    for (const char *c = token.data.start + prefix_size; c < token.data.end; c += 1) {
                        unsigned digit = clex__digit_value(*c);
                        if (digit < base) result = result * base + digit;
                    }
                }

                if (token.kind == clex_Token_CHARACTER) {
                    const char *c = token.data.start;
                    result = (unsigned char)*c;

                    if (*c == '\\') {
                        c += 1;
                        switch (*c) {
                            case 'a': result = '\a'; break;
                            case 'b': result = '\b'; break;
                            case 'f': result = '\f'; break;
                            case 'n': result = '\n'; break;
                            case 'r': result = '\r'; break;
                            case 't': result = '\t'; break;
                            case 'v': result = '\v'; break;
                            case 'x': {
                                result = 0;
                                for (c += 1; c < token.data.end; c += 1) result = result * 16 + clex__digit_value(*c);
                            } break;
                            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': {
                                result = 0;
                                for (; c < token.data.end; c += 1) result = result * 8 + clex__digit_value(*c);
                            } break;
                            default: result = (unsigned char)*c; break;
                        }
                    }
                }
            }
        } break;
        case clex_Node_PREFIX: {
            clex__Value operand = clex__evaluate(tokens, nodes, last_child, error_token_index, ok);
            unsigned long long operand_bits = (unsigned long long)operand.value;
            is_known = operand.is_known;
            switch (operator) {
                case clex_Token_PLUS: result = operand_bits; break;
                case clex_Token_MINUS: result = 0ull - operand_bits; break;
                case clex_Token_BIT_NOT: result = ~operand_bits; break;
                case clex_Token_NOT: result = operand_bits == 0; break;
                default: node_ok = 0; break;
            }
        } break;
        case clex_Node_INFIX: {
            unsigned right_index = last_child;
            unsigned left_index = right_index - nodes[right_index].subtree_size;
            clex__Value left = clex__evaluate(tokens, nodes, left_index, error_token_index, ok);

            _Bool is_logical = operator == clex_Token_AND || operator == clex_Token_OR;
            _Bool deciding_value = operator == clex_Token_OR;
            _Bool left_decides = is_logical && left.is_known && (left.value != 0) == deciding_value;

            clex__Value right = { .value = 0, .is_known = 1 };
            if (is_logical && !left.is_known) right = clex__evaluate_speculatively(tokens, nodes, right_index);
            else if (!left_decides) right = clex__evaluate(tokens, nodes, right_index, error_token_index, ok);

            unsigned long long left_bits = (unsigned long long)left.value;
            unsigned long long right_bits = (unsigned long long)right.value;
            long long minimum = (long long)(1ull << 63);
            _Bool both_known = left.is_known && right.is_known;
            _Bool division_ok = !right.is_known || (right.value != 0 && !(both_known && left.value == minimum && right.value == -1));
            _Bool shift_ok = !right.is_known || (0 <= right.value && right.value < 64);
            is_known = both_known;

            switch (operator) {
                case clex_Token_AND: case clex_Token_OR: {
                    _Bool right_decides = right.is_known && (right.value != 0) == deciding_value;
                    is_known = left_decides || right_decides || both_known;
                    result = deciding_value;
                    if (!left_decides && !right_decides) result = !deciding_value;
                } break;
                case clex_Token_COMMA: {
                    is_known = right.is_known;
                    result = right_bits;
                } break;
                case clex_Token_PIPE: result = left_bits | right_bits; break;
                case clex_Token_CARET: result = left_bits ^ right_bits; break;
                case clex_Token_AMPERSAND: result = left_bits & right_bits; break;
                case clex_Token_EQUALITY_CHECK: result = left.value == right.value; break;
                case clex_Token_NOT_EQUALS: result = left.value != right.value; break;
                case clex_Token_LESS_THAN: result = left.value < right.value; break;
                case clex_Token_GREATER_THAN: result = left.value > right.value; break;
                case clex_Token_LESS_THAN_OR_EQUAL: result = left.value <= right.value; break;
                case clex_Token_GREATER_THAN_OR_EQUAL: result = left.value >= right.value; break;
                case clex_Token_LEFT_SHIFT: {
                    node_ok = shift_ok;
                    if (node_ok && both_known) result = left_bits << right.value;
                } break;
                case clex_Token_RIGHT_SHIFT: {
                    node_ok = shift_ok;
                    if (node_ok && both_known) result = (unsigned long long)(left.value >> right.value);
                } break;
                case clex_Token_PLUS: result = left_bits + right_bits; break;
                case clex_Token_MINUS: result = left_bits - right_bits; break;
                case clex_Token_ASTERISK: result = left_bits * right_bits; break;
                case clex_Token_DIVIDE: {
                    node_ok = division_ok;
                    if (node_ok && both_known) result = (unsigned long long)(left.value / right.value);
                } break;
                case clex_Token_MODULO: {
                    node_ok = division_ok;
                    if (node_ok && both_known) result = (unsigned long long)(left.value % right.value);
                } break;
                default: node_ok = 0; break;
            }
        } break;
        case clex_Node_TERNARY: {
            unsigned false_index = last_child;
            unsigned true_index = false_index - nodes[false_index].subtree_size;
            unsigned condition_index = true_index - nodes[true_index].subtree_size;
            clex__Value condition = clex__evaluate(tokens, nodes, condition_index, error_token_index, ok);

            if (condition.is_known) {
                unsigned chosen_index = condition.value != 0 ? true_index : false_index;
                clex__Value chosen = clex__evaluate(tokens, nodes, chosen_index, error_token_index, ok);
                result = (unsigned long long)chosen.value;
                is_known = chosen.is_known;
            } else {
                clex__Value if_true = clex__evaluate_speculatively(tokens, nodes, true_index);
                clex__Value if_false = clex__evaluate_speculatively(tokens, nodes, false_index);
                result = (unsigned long long)if_true.value;
                is_known = if_true.is_known && if_false.is_known && if_true.value == if_false.value;
            }
        } break;
        default: node_ok = 0; break;
    }

    if (*ok && !node_ok) {
        *ok = 0;
        *error_token_index = node.token_index;
    }

    if (!is_known) result = 0;
    clex__Value value = { .value = (long long)result, .is_known = is_known };
    return value;
}

CLEX_FUNCTION clex_Evaluation clex_evaluate_preprocessor_expression(const clex_Token *tokens, const clex_Node *nodes, unsigned root, _Bool *ok) {
    _Bool evaluate_ok = 1;
    unsigned error_token_index = 0;
    clex__Value result = clex__evaluate(tokens, nodes, root, &error_token_index, &evaluate_ok);
    *ok = evaluate_ok;
    clex_Evaluation evaluation = { .value = result.value, .is_known = result.is_known, .error_token_index = error_token_index };
    return evaluation;
}

CLEX_FUNCTION int clex_get_current_location(const clex_Lexer *l, int *column, const char **path, const char **path_end) {
    int file_depth = l->depth;
    while (file_depth > 0 && l->stack[file_depth].macro_name.start != 0) file_depth -= 1;
    clex_Cursor cursor = l->stack[file_depth];
    *path = cursor.path;
    *path_end = cursor.path_end;

    int line = 1;
    *column = 1;

    for (const char *c = cursor.start; c < cursor.c; c += 1) {
        if (*c == '\n') {
            line += 1;
            *column = 1;
        } else *column += 1;
    }

    return line;
}

// TODO(felix): `#include` with a macro operand. Is this ever used?
CLEX_FUNCTION clex_Include clex_include_operand(clex_Data operand, _Bool *ok) {
    clex_Include include = {0};
    _Bool has_opening = operand.start < operand.end && (*operand.start == '"' || *operand.start == '<');

    const char *closing = operand.start + 1;
    char closing_character = 0;
    if (has_opening) {
        include.is_system = *operand.start == '<';
        closing_character = include.is_system ? '>' : '"';
        while (closing < operand.end && *closing != closing_character) closing += 1;
    }

    _Bool path_is_valid = has_opening && closing < operand.end && closing > operand.start + 1;
    if (path_is_valid) include.path = (clex_Data){ .start = operand.start + 1, .end = closing };

    *ok = path_is_valid;
    return include;
}

CLEX_FUNCTION clex_Lexer clex_init(const char *path, const char *path_end, const char *start, const char *end, clex_Flags flags) {
    clex_Lexer lexer = {
        .stack[0] = {
            .path = path,
            .path_end = path_end,
            .start = start,
            .end = end,
            .c = start,
        },
        .flags = flags,
    };
    return lexer;
}

static _Bool clex__is_at_end(const clex_Cursor *c) { return c->c >= c->end || *c->c == 0; }

static _Bool clex__is_newline(const char *c) {
    if (*c == '\n') return 1;
    if (*c == '\r' && c[1] == '\n') return 1;
    return 0;
}

static unsigned clex__line_continuation_size(const char *c) {
    unsigned size = 0;
    if (*c == '\\' && clex__is_newline(&c[1])) size = 2 + (c[1] == '\r');
    return size;
}

static void clex__skip_horizontal_whitespace(clex_Cursor *c) {
    for (_Bool skipped = 1; skipped && c->c < c->end;) {
        _Bool is_blank = *c->c == ' ' || *c->c == '\t';
        unsigned continuation_size = clex__line_continuation_size(c->c);
        skipped = is_blank || continuation_size > 0;
        c->c += is_blank + continuation_size;
    }
}

static void clex__skip_identifier(clex_Cursor *c) {
    // identifier:
    //      identifier-start
    //      identifier identifier-continue
    // identifier-start:
    //      nondigit
    //      XID_Start character
    // identifier-continue:
    //      digit
    //      nondigit
    //      XID_Continue character
    // XID_Start, XID_continue:
    //      implementation-defined, something to do with unicode? ignore for now

    if (!clex__is_nondigit(*c->c) && *c->c != '$') return;
    c->c += 1;
    while (clex__is_nondigit(*c->c) || clex__is_digit(*c->c) || *c->c == '$') c->c += 1;
}

static void clex__skip_to_newline(clex_Cursor *c) {
    while (!clex__is_at_end(c) && !clex__is_newline(c->c)) {
        unsigned continuation_size = clex__line_continuation_size(c->c);
        c->c += continuation_size > 0 ? continuation_size : 1;
    }
}

static _Bool clex__skip_escape_sequence(clex_Cursor *c) {
    _Bool ok = 0;
    CLEX_ASSERT(*c->c == '\\');

    switch (*(++c->c)) {
        case '"': case '\'': case '?': case '\\': case 'a': case 'b': case 'f': case 'n': case 'r': case 't': case 'v': {
            ok = 1;
            c->c += 1;
        } break;
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': {
            ok = 1;
            enum { OCTAL_DIGIT_COUNT_MAX = 3 };
            for (int i = 0; i < OCTAL_DIGIT_COUNT_MAX && '0' <= *c->c && *c->c <= '7'; i += 1) c->c += 1;
        } break;
        case 'x': {
            c->c += 1;
            const char *digits = c->c;
            for (_Bool is_hex_digit = 1; is_hex_digit; c->c += is_hex_digit) {
                char lowercase = *c->c | 0x20;
                is_hex_digit = clex__is_digit(*c->c) || ('a' <= lowercase && lowercase <= 'f');
            }
            ok = c->c > digits;
        } break;
        default: break;
    }

    return ok;
}

CLEX_FUNCTION _Bool clex_lex(clex_Lexer *l) {
    l->token = (clex_Token){0};

    next_token:;

    clex_Cursor *c = &l->stack[l->depth];
    for (; c->c < c->end; c->c += 1) {
        char ch = *c->c;
        if (ch != '\n' && ch != '\t' && ch != '\r' && ch != ' ' && !(ch == '\\' && c->c[1] != '\\')) break;
    }

    if (clex__is_at_end(c)) {
        if (l->depth > 0) {
            l->depth -= 1;
            goto next_token;
        } else {
            l->token = (clex_Token){
                .kind = clex_Token_END_OK,
                .data = { .start = c->c, .end = c->c },
            };
            return 0;
        }
    }

    const char *quote_after_prefix = c->c + 1 + (c->c[0] == 'u' && c->c[1] == '8');
    _Bool is_quote_after_prefix = *quote_after_prefix == '"' || *quote_after_prefix == '\'';
    _Bool utf8_prefix = is_quote_after_prefix && c->c[0] == 'u' && c->c[1] == '8';
    _Bool utf16_prefix = is_quote_after_prefix && c->c[0] == 'u' && !utf8_prefix;
    _Bool utf32_prefix = is_quote_after_prefix && c->c[0] == 'U';
    _Bool wide_prefix = is_quote_after_prefix && c->c[0] == 'L';

    clex_Token_Flags prefix_flags = 0;
    prefix_flags |= utf8_prefix  * clex_Token_Flag_PREFIX_UTF8;
    prefix_flags |= utf16_prefix * clex_Token_Flag_PREFIX_UTF16;
    prefix_flags |= utf32_prefix * clex_Token_Flag_PREFIX_UTF32;
    prefix_flags |= wide_prefix  * clex_Token_Flag_PREFIX_WIDE;
    c->c += 2 * utf8_prefix + (utf16_prefix || utf32_prefix || wide_prefix);

    switch (*c->c) {
        case '#': {
            // If we find non-whitespace between this and the last newline, this is a punctuator, not a preprocessor directive.
            _Bool is_directive = c->macro_name.start == 0;
            for (const char *back = &c->c[-1]; is_directive && back >= c->start && *back != '\n'; back -= 1) {
                is_directive = *back == ' ' || *back == '\t';
            }

            if (!is_directive) {
                l->token = (clex_Token){
                    .kind = (clex_Token_Kind)(*c->c),
                    .data = { .start = c->c, .end = ++c->c },
                };
                return 1;
            }

            c->c += 1;
            clex__skip_horizontal_whitespace(c);

            const char *directive = c->c;
            clex__skip_identifier(c);
            const char *directive_end = c->c;
            if (directive_end == directive) { l->error_message = "expected a directive name after `#`"; goto error; }

            clex__skip_horizontal_whitespace(c);
            const char *definition = c->c;
            clex_Cursor cursor_at_definition = *c;
            clex__skip_to_newline(c);
            const char *definition_end = c->c;

            l->token = (clex_Token){
                .data = { .start = directive, .end = directive_end },
                .data2 = { .start = definition, .end = definition_end },
            };

            if (clex__string_equals(directive, directive_end, "define")) {
                l->token.kind = clex_Token_DEFINE;

                clex_Cursor *d = &cursor_at_definition;
                l->token.data.start = d->c;

                clex__skip_identifier(d);
                if (d->c == l->token.data.start) { l->error_message = "expected a macro name after `#define`"; goto error; }
                l->token.data.end = d->c;

                l->token.data2.start = d->c;
                clex__skip_to_newline(d);
                l->token.data2.end = d->c;
            } else if (clex__string_equals(directive, directive_end, "elif")) {
                l->token.kind = clex_Token_ELIF;
            } else if (clex__string_equals(directive, directive_end, "else")) {
                l->token.kind = clex_Token_ELSE;
            } else if (clex__string_equals(directive, directive_end, "endif")) {
                l->token.kind = clex_Token_ENDIF;
            } else if (clex__string_equals(directive, directive_end, "error")) {
                l->token.kind = clex_Token_ERROR_DIRECTIVE;
            } else if (clex__string_equals(directive, directive_end, "if")) {
                l->token.kind = clex_Token_IF;

            } else if (clex__string_equals(directive, directive_end, "ifdef")) {
                l->token.kind = clex_Token_IFDEF;
            } else if (clex__string_equals(directive, directive_end, "ifndef")) {
                l->token.kind = clex_Token_IFNDEF;
            } else if (clex__string_equals(directive, directive_end, "include")) {
                l->token.kind = clex_Token_INCLUDE;
            } else if (clex__string_equals(directive, directive_end, "pragma")) {
                l->token.kind = clex_Token_PRAGMA;
            } else if (clex__string_equals(directive, directive_end, "undef")) {
                l->token.kind = clex_Token_UNDEF;
            } else { l->error_message = "unknown preprocessor directive"; goto error; }

            return 1;
        } break;
        case '(': case ')': case ',': case ':': case ';': case'[': case ']': case '{': case '}': case '?': {
            l->token = (clex_Token){
                .kind = (clex_Token_Kind)(*c->c),
                .data = { .start = c->c, .end = ++c->c },
            };
            return 1;
        } break;
        case '.': {
            _Bool starts_fraction = clex__is_digit(c->c[1]);
            if (starts_fraction) break;

            _Bool is_ellipsis = c->c[1] == '.' && c->c[2] == '.';
            const char *dot = c->c;
            c->c += 1 + 2 * is_ellipsis;

            l->token = (clex_Token){
                .kind = is_ellipsis ? clex_Token_ELLIPSIS : clex_Token_DOT,
                .data = { .start = dot, .end = c->c },
            };
            return 1;
        } break;
        case '!': case '%': case '&': case '*': case '+': case '-': case '/': case '<': case '>': case '^': case '|': // can be immediately followed by `=`
        case '=': case '~':
        {
            _Bool single_line_comment = *c->c == '/' && c->c[1] == '/';
            _Bool old_comment = *c->c == '/' && c->c[1] == '*';
            _Bool any_comment = single_line_comment || old_comment;

            if (any_comment) {
                c->c += 2;
                const char *comment = c->c;
                const char *comment_end = comment;

                if (single_line_comment) {
                    while (*comment_end != 0 && !clex__is_newline(comment_end)) comment_end += 1;
                    c->c = comment_end;
                } else {
                    CLEX_ASSERT(old_comment);
                    while (*comment_end != 0 && !(*comment_end == '*' && comment_end[1] == '/')) comment_end += 1;
                    if (*comment_end == 0) { l->error_message = "block comment has no closing `*/`"; goto error; }
                    c->c = comment_end + 2;
                }

                if (!(l->flags & clex_Flag_PARSE_COMMENTS)) goto next_token;

                l->token = (clex_Token){
                    .kind = clex_Token_COMMENT,
                    .data = { .start = comment, .end = comment_end },
                };
                return 1;
            }

            const char *operator = c->c;

            _Bool is_arrow = *c->c == '-' && c->c[1] == '>';
            if (is_arrow) {
                c->c += 2;
                l->token = (clex_Token){
                    .kind = clex_Token_ARROW,
                    .data = { .start = operator, .end = c->c },
                };
                return 1;
            }

            _Bool doubled = c->c[1] == *c->c;
            if (doubled) switch (*operator) {
                case '+': case '-': case '<': case '>': case '=': case '&': case '|': break;
                default: doubled = 0; break;
            }
            c->c += doubled;

            _Bool has_assignment_form = 0;
            switch (*operator) {
                case '!': case '<': case '>': case '*': case '/': case '%': case '+': case '-': case '&': case '^': case '|': has_assignment_form = 1; break;
                default: break;
            }
            _Bool is_shift = *operator == '<' || *operator == '>';
            if (doubled) has_assignment_form = is_shift;

            _Bool assignment = has_assignment_form && c->c[1] == '=';
            c->c += assignment;

            const char *operator_end = ++c->c;

            clex_Token_Kind kind = (clex_Token_Kind)(*operator + doubled * clex_Token__DOUBLE_OPERATOR + assignment * clex_Token__ASSIGNMENT_OPERATOR);

            l->token = (clex_Token){
                .kind = kind,
                .data = { .start = operator, .end = operator_end },
            };
            return 1;
        } break;
        case '\'': {
            if (c->c[1] == 0 || c->c[2] == 0) { l->error_message = "character constant has no closing quote"; goto error; }

            c->c += 1;
            const char *character = c->c;

            const char *character_end = c->c + 1;
            if (*c->c == '\\') {
                if (!clex__skip_escape_sequence(c)) { l->error_message = "invalid escape sequence in character constant"; goto error; }
                character_end = c->c;
            }

            if (*character_end != '\'') { l->error_message = "character constant has no closing quote after one character"; goto error; }
            c->c = character_end + 1;

            l->token = (clex_Token){
                .kind = clex_Token_CHARACTER,
                .data = { .start = character, .end = character_end },
                .flags = prefix_flags,
            };
            return 1;
        } break;
        case '"': {
            c->c += 1;
            if (*c->c == 0) { l->error_message = "string has no closing quote"; goto error; }

            const char *string = c->c;

            while (*c->c != 0) {
                if (*c->c == '\\') {
                    if (!clex__skip_escape_sequence(c)) { l->error_message = "invalid escape sequence in string"; goto error; }
                    continue;
                }
                if (*c->c == '"') break;
                if (clex__is_newline(c->c)) { l->error_message = "string has no closing quote before the end of the line"; goto error; }

                c->c += 1;
            }
            if (*c->c == 0) { l->error_message = "string has no closing quote"; goto error; }

            const char *string_end = c->c++;
            l->token = (clex_Token){
                .kind = clex_Token_STRING,
                .data = { .start = string, .end = string_end },
                .flags = prefix_flags,
            };
            return 1;
        } break;
        default: break;
    }

    // primary-expression:
    //      identifier
    //      constant
    //      string-literal
    //      ( expression )
    //      generic-selection
    // constant:
    //      integer-constant
    //      floating-constant
    //      enumeration-constant
    //      character-constant
    //      predefined-constant
    // string-literal:
    //      encoding-prefix[opt] " s-char-sequence[opt] "
    // expression:
    //      assignment-expression
    //      expression , assignment-expression
    // generic-selection:
    //      _Generic ( assignment-expression , generic-assoc-list )
    // assignment-expression:
    //      conditional-expression
    //      unary-expression assignment-operator assignment-expression
    // unary-expression:
    //      postfix-expression
    //      ++ unary-expression
    //      -- unary-expression
    //      unary-operator cast-expression
    //      sizeof unary-expression
    //      sizeof ( type-name )
    //      alignof ( type-name )
    // postfix-expression:
    //      primary-expression
    //      postfix-expression [ expression ]
    //      postfix-expression ( argument-expression-list[opt] )
    //      postfix-expression . identifier
    //      postfix-expression -> identifier
    //      postfix-expression ++
    //      postfix-expression --
    //      compound-literal
    // unary-operator: one of
    //      & * + - ~ !
    // assignment-operator: one of
    //      = *= /= %= += -= <<= >>= &= ^= |=

    const char *identifier = c->c;
    clex__skip_identifier(c);
    const char *identifier_end = c->c;
    _Bool is_identifier = identifier_end > identifier;
    if (is_identifier) {
        l->token = (clex_Token){
            .kind = clex_Token_IDENTIFIER,
            .data = { .start = identifier, .end = identifier_end },
        };
        return 1;
    }

    // constant:
    //      integer-constant: before integer-suffix[opt]
    //          decimal-constant:
    //              nonzero-digit
    //              decimal-constant ,[opt] digit
    //          octal-constant:
    //              0
    //              octal-constant ,[opt] octal-digit
    //          hexadecimal-constant:
    //              hexadecimal-prefix hexadecimal-digit-sequence
    //          binary-constant
    //              binary-prefix binary-digit
    //              binary-constant ,[opt] binary-digit
    //      floating-constant:
    //          decimal-floating-constant: before floating-suffix[opt]
    //              fractional-constant exponent-part[opt]
    //              digit-sequence exponent-part
    //          hexadecimal-floating-constant: before floating-suffix[opt]
    //              hexadecimal-prefix hexadecimal-fractional-constant binary-exponent-part
    //              hexadecimal-prefix hexadecimal-digit-sequence binary-exponent-part
    //      enumeration-constant = identifier
    //      character-constant:
    //          encoding-prefix[opt] ' c-char-sequence '

    _Bool starts_number = clex__is_digit(*c->c) || (*c->c == '.' && clex__is_digit(c->c[1]));
    if (starts_number) {
        const char *number = c->c;

        unsigned base = 10;
        clex_Token_Flags flags = clex_Token_Flag_BASE_10;
        if (*c->c == '0') switch (c->c[1] | 0x20) {
            case 'b': base = 2;  flags = clex_Token_Flag_BASE_2;  c->c += 2; break;
            case 'x': base = 16; flags = clex_Token_Flag_BASE_16; c->c += 2; break;
            case '.': break; // decimal fraction, as in 0.5
            default:  base = 8;  flags = clex_Token_Flag_BASE_8;  c->c += 1; break;
        }

        const char *digits = c->c;
        for (unsigned value = 0; value < base; c->c += value < base) {
            unsigned decimal_value = (unsigned)(*c->c - '0');
            unsigned letter_value = (unsigned)((*c->c | 0x20) - 'a');
            _Bool is_decimal_digit = decimal_value < 10;
            _Bool is_hex_letter = letter_value < 6;

            value = is_decimal_digit * decimal_value;
            value += is_hex_letter * (letter_value + 10);
            value += !(is_decimal_digit || is_hex_letter) * base;
        }

        // NOTE(felix): 0 is a complete octal constant, and a decimal like .5 can have no digits before the decimal point.
        _Bool is_prefix_without_digits = c->c == digits && (base == 2 || base == 16);
        _Bool is_digit_invalid_for_base = clex__is_digit(*c->c);
        if (is_prefix_without_digits || is_digit_invalid_for_base) {
            l->error_message = is_prefix_without_digits ? "expected digits after the number prefix" : "digit is not valid in the base of this number";
            goto error;
        }

        _Bool has_fraction = base == 10 && *c->c == '.';
        if (has_fraction) {
            c->c += 1;
            while (clex__is_digit(*c->c)) c->c += 1;
        }

        _Bool has_exponent = base == 10 && (*c->c | 0x20) == 'e';
        if (has_exponent) {
            const char *exponent_digits = c->c + 1;
            exponent_digits += *exponent_digits == '+' || *exponent_digits == '-';
            if (!clex__is_digit(*exponent_digits)) { l->error_message = "expected digits in the exponent"; goto error; }

            c->c = exponent_digits;
            while (clex__is_digit(*c->c)) c->c += 1;
        }

        _Bool floating = has_fraction || has_exponent;
        const char *number_end = c->c;

        for (;;) {
            const char *old_c = c->c;

            _Bool floating_suffix = *c->c == 'f';
            c->c += floating_suffix;

            _Bool unsigned_suffix = (*c->c | 0x20) == 'u';
            c->c += unsigned_suffix;

            _Bool long_suffix = (*c->c | 0x20) == 'l';
            _Bool long_long_suffix = long_suffix && c->c[1] == *c->c;
            long_suffix = long_suffix && !long_long_suffix;
            c->c += long_suffix;
            c->c += 2 * long_long_suffix;

            _Bool bit_suffix = 0;
            if ((*c->c | 0x20) == 'w' && c->c[1] != 0) {
                char next = 'B' | (*c->c & 0x20);
                bit_suffix = c->c[1] == next;
            }
            c->c += 2 * bit_suffix;

            flags |= floating_suffix * clex_Token_Flag_SUFFIX_FLOAT;
            flags |= unsigned_suffix * clex_Token_Flag_SUFFIX_UNSIGNED;
            flags |= long_suffix * clex_Token_Flag_SUFFIX_LONG;
            flags |= long_long_suffix * clex_Token_Flag_SUFFIX_LONG_LONG;
            flags |= bit_suffix * clex_Token_Flag_SUFFIX_BIT_PRECISE_INT;

            _Bool no_more_suffixes = c->c == old_c;
            if (no_more_suffixes) break;
        }

        l->token = (clex_Token){
            .kind = floating ? clex_Token_FLOAT : clex_Token_INTEGER,
            .data = { .start = number, .end = number_end },
            .flags = flags,
        };
        return 1;
    }

    l->error_message = "unexpected character";

    error:
    l->token = (clex_Token){0};
    return 0;
}

static clex_Token_Kind clex__parse_peek(const clex_Parser *p) {
    clex_Token_Kind kind = clex_Token_END_OK;
    if (p->token_index < p->token_count) kind = p->tokens[p->token_index].kind;
    return kind;
}

static void clex__parse_push_node(clex_Parser *p, clex_Node_Kind kind, unsigned token_index, unsigned subtree_start, _Bool *ok) {
    *ok = *ok && p->node_count < p->node_capacity;
    if (*ok) {
        p->nodes[p->node_count] = (clex_Node){ .kind = kind, .token_index = token_index, .subtree_size = p->node_count + 1 - subtree_start };
        p->node_count += 1;
    }
}

static void clex__parse_expect(clex_Parser *p, clex_Token_Kind kind, _Bool *ok) {
    *ok = *ok && clex__parse_peek(p) == kind;
    p->token_index += *ok;
}

// TODO(felix): Postfix operators (calls, subscripts, member access, `++`, `--`), casts, `sizeof`, `alignof`, compound literals and `_Generic`, for parsing C.
static void clex__parse_expression(clex_Parser *p, unsigned minimum_binding_power, _Bool *ok) {
    enum {
        clex__Binding_Power_COMMA          =  1,
        clex__Binding_Power_ASSIGNMENT     =  2,
        clex__Binding_Power_TERNARY        =  3,
        clex__Binding_Power_LOGICAL_OR     =  4,
        clex__Binding_Power_LOGICAL_AND    =  5,
        clex__Binding_Power_BIT_OR         =  6,
        clex__Binding_Power_BIT_XOR        =  7,
        clex__Binding_Power_BIT_AND        =  8,
        clex__Binding_Power_EQUALITY       =  9,
        clex__Binding_Power_RELATIONAL     = 10,
        clex__Binding_Power_SHIFT          = 11,
        clex__Binding_Power_ADDITIVE       = 12,
        clex__Binding_Power_MULTIPLICATIVE = 13,
        clex__Binding_Power_PREFIX         = 14,
    };

    unsigned subtree_start = p->node_count;
    p->depth += 1;
    *ok = *ok && p->depth <= CLEX_PARSE_DEPTH_MAX;

    unsigned operator_index = p->token_index;
    if (*ok) switch (clex__parse_peek(p)) {
        case clex_Token_INTEGER: case clex_Token_FLOAT: case clex_Token_CHARACTER: case clex_Token_STRING: {
            p->token_index += 1;
            clex__parse_push_node(p, clex_Node_LITERAL, operator_index, subtree_start, ok);
        } break;
        case clex_Token_IDENTIFIER: {
            p->token_index += 1;
            clex__parse_push_node(p, clex_Node_NAME, operator_index, subtree_start, ok);
        } break;
        case clex_Token_OPEN_PAREN: {
            p->token_index += 1;
            clex__parse_expression(p, 0, ok);
            clex__parse_expect(p, clex_Token_CLOSE_PAREN, ok);
        } break;
        case clex_Token_PLUS: case clex_Token_MINUS: case clex_Token_NOT: case clex_Token_BIT_NOT:
        case clex_Token_ASTERISK: case clex_Token_AMPERSAND: case clex_Token_PLUS_PLUS: case clex_Token_MINUS_MINUS: {
            p->token_index += 1;
            clex__parse_expression(p, clex__Binding_Power_PREFIX, ok);
            clex__parse_push_node(p, clex_Node_PREFIX, operator_index, subtree_start, ok);
        } break;
        default: *ok = 0; break;
    }

    for (_Bool continuing = *ok; continuing;) {
        clex_Token_Kind kind = clex__parse_peek(p);

        unsigned char left_power = 0;
        _Bool is_right_associative = 0;
        switch (kind) {
            case clex_Token_COMMA: left_power = clex__Binding_Power_COMMA; break;
            case clex_Token_EQUALS: case clex_Token_MODULO_ASSIGN: case clex_Token_BIT_AND_ASSIGN: case clex_Token_MULTIPLY_ASSIGN:
            case clex_Token_PLUS_ASSIGN: case clex_Token_MINUS_ASSIGN: case clex_Token_DIVIDE_ASSIGN: case clex_Token_XOR_ASSIGN:
            case clex_Token_BIT_OR_ASSIGN: case clex_Token_LEFT_SHIFT_ASSIGN: case clex_Token_RIGHT_SHIFT_ASSIGN: {
                left_power = clex__Binding_Power_ASSIGNMENT;
                is_right_associative = 1;
            } break;
            case clex_Token_QUESTION: {
                left_power = clex__Binding_Power_TERNARY;
                is_right_associative = 1;
            } break;
            case clex_Token_OR: left_power = clex__Binding_Power_LOGICAL_OR; break;
            case clex_Token_AND: left_power = clex__Binding_Power_LOGICAL_AND; break;
            case clex_Token_PIPE: left_power = clex__Binding_Power_BIT_OR; break;
            case clex_Token_CARET: left_power = clex__Binding_Power_BIT_XOR; break;
            case clex_Token_AMPERSAND: left_power = clex__Binding_Power_BIT_AND; break;
            case clex_Token_EQUALITY_CHECK: case clex_Token_NOT_EQUALS: left_power = clex__Binding_Power_EQUALITY; break;
            case clex_Token_LESS_THAN: case clex_Token_GREATER_THAN: case clex_Token_LESS_THAN_OR_EQUAL: case clex_Token_GREATER_THAN_OR_EQUAL: {
                left_power = clex__Binding_Power_RELATIONAL;
            } break;
            case clex_Token_LEFT_SHIFT: case clex_Token_RIGHT_SHIFT: left_power = clex__Binding_Power_SHIFT; break;
            case clex_Token_PLUS: case clex_Token_MINUS: left_power = clex__Binding_Power_ADDITIVE; break;
            case clex_Token_ASTERISK: case clex_Token_DIVIDE: case clex_Token_MODULO: left_power = clex__Binding_Power_MULTIPLICATIVE; break;
            default: break;
        }

        continuing = left_power != 0 && left_power >= minimum_binding_power;
        if (continuing) {
            operator_index = p->token_index;
            p->token_index += 1;

            clex_Node_Kind node_kind = clex_Node_INFIX;
            if (kind == clex_Token_QUESTION) {
                node_kind = clex_Node_TERNARY;
                clex__parse_expression(p, 0, ok);
                clex__parse_expect(p, clex_Token_COLON, ok);
            }

            unsigned char right_power = (unsigned char)(left_power + !is_right_associative);
            clex__parse_expression(p, right_power, ok);
            clex__parse_push_node(p, node_kind, operator_index, subtree_start, ok);
            continuing = *ok;
        }
    }

    p->depth -= 1;
}

CLEX_FUNCTION unsigned clex_parse_expression(clex_Parser *p, _Bool *ok) {
    _Bool parse_ok = 1;
    clex__parse_expression(p, 0, &parse_ok);
    unsigned root = parse_ok ? p->node_count - 1 : 0;
    *ok = parse_ok;
    return root;
}

// TODO(felix): Function-like macros:
//  - read the arguments from the input and push each argument's range when the body names its parameter;
//  - `#` and `##`;
//  - full expansion of arguments before substitution.
// TODO(felix): A name that is not expanded because its macro is already being expanded must stay unexpandable afterwards (C11 6.10.3.4p2).
CLEX_FUNCTION _Bool clex_push_expansion(clex_Lexer *l, clex_Data name, clex_Data definition, _Bool *ok) {
    _Bool is_already_expanding = 0;
    for (int i = 0; i <= l->depth && !is_already_expanding; i += 1) {
        clex_Data expanding = l->stack[i].macro_name;
        is_already_expanding = expanding.start != 0 && expanding.end - expanding.start == name.end - name.start;
        for (const char *x = expanding.start, *y = name.start; is_already_expanding && x < expanding.end; x += 1, y += 1) is_already_expanding = *x == *y;
    }

    _Bool has_space = l->depth + 1 < CLEX_EXPANSION_STACK_DEPTH;
    _Bool expanded = !is_already_expanding && has_space;
    if (expanded) {
        clex_Cursor *file = &l->stack[l->depth];
        l->depth += 1;
        l->stack[l->depth] = (clex_Cursor){
            .path = file->path,
            .path_end = file->path_end,
            .start = definition.start,
            .end = definition.end,
            .c = definition.start,
            .macro_name = name,
        };
    }

    *ok = has_space || is_already_expanding;
    return expanded;
}

CLEX_FUNCTION _Bool clex_push_file(clex_Lexer *l, const char *path, const char *path_end, const char *start, const char *end) {
    _Bool had_enough_space = 0;

    if (l->depth + 1 < CLEX_EXPANSION_STACK_DEPTH) {
        clex_Cursor cursor = {
            .path = path,
            .path_end = path_end,
            .start = start,
            .end = end,
            .c = start,
        };
        l->stack[++l->depth] = cursor;
        had_enough_space = 1;
    }

    return had_enough_space;
}

CLEX_FUNCTION _Bool clex_skip_conditional_group(clex_Lexer *l) {
    clex_Cursor *c = &l->stack[l->depth];
    unsigned nesting = 0;
    _Bool found_group_end = 0;
    _Bool is_in_block_comment = 0;

    while (!found_group_end && !clex__is_at_end(c)) {
        const char *line = c->c;

        clex_Cursor probe = *c;
        clex__skip_horizontal_whitespace(&probe);
        if (!is_in_block_comment && *probe.c == '#') {
            probe.c += 1;
            clex__skip_horizontal_whitespace(&probe);
            const char *directive = probe.c;
            clex__skip_identifier(&probe);
            const char *directive_end = probe.c;

            _Bool opens_group = clex__string_equals(directive, directive_end, "if");
            opens_group = opens_group || clex__string_equals(directive, directive_end, "ifdef");
            opens_group = opens_group || clex__string_equals(directive, directive_end, "ifndef");
            _Bool closes_group = clex__string_equals(directive, directive_end, "endif");
            _Bool continues_group = clex__string_equals(directive, directive_end, "elif") || clex__string_equals(directive, directive_end, "else");

            found_group_end = nesting == 0 && (closes_group || continues_group);
            nesting += opens_group;
            nesting -= closes_group && !found_group_end;
        }

        if (found_group_end) c->c = line;
        else {
            while (!clex__is_at_end(c) && !clex__is_newline(c->c)) {
                char ch = *c->c;
                unsigned continuation_size = clex__line_continuation_size(c->c);
                _Bool closes_block_comment = is_in_block_comment && ch == '*' && c->c[1] == '/';
                _Bool opens_block_comment = !is_in_block_comment && ch == '/' && c->c[1] == '*';
                _Bool opens_line_comment = !is_in_block_comment && ch == '/' && c->c[1] == '/';
                _Bool opens_quote = !is_in_block_comment && (ch == '"' || ch == '\'');

                if (continuation_size > 0) c->c += continuation_size;
                else if (closes_block_comment || opens_block_comment) {
                    is_in_block_comment = opens_block_comment;
                    c->c += 2;
                } else if (opens_line_comment) clex__skip_to_newline(c);
                else if (opens_quote) {
                    c->c += 1;
                    while (!clex__is_at_end(c) && !clex__is_newline(c->c) && *c->c != ch) c->c += 1 + (*c->c == '\\' && c->c[1] != 0);
                    c->c += *c->c == ch;
                } else c->c += 1;
            }

            c->c += *c->c == '\r';
            c->c += *c->c == '\n';
        }
    }

    return found_group_end;
}

#endif // CLEX_IMPLEMENTATION
