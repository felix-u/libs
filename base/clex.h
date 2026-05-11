// https://github.com/felix-u 2026-05-11
// Public domain. NO WARRANTY - use at your own risk

#if !defined(CLEX_H)
#define CLEX_H


typedef struct { const char *start, *end; } clex_Data;

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
    clex_Token_Kind__DOUBLE_OPERATOR_END = clex_Token__DOUBLE_OPERATOR + 128,

    // TODO(felix): ASCII order
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
    clex_Token__ASSIGNMENT_OPERATOR_END = clex_Token__DOUBLE_OPERATOR + 128,

    clex_Token__ASSIGNMENT_DOUBLE_OPERATOR = clex_Token__DOUBLE_OPERATOR + clex_Token__ASSIGNMENT_OPERATOR,
    clex_Token_LEFT_SHIFT_ASSIGN  = '<' + clex_Token__ASSIGNMENT_DOUBLE_OPERATOR,
    clex_Token_RIGHT_SHIFT_ASSIGN = '>' + clex_Token__ASSIGNMENT_DOUBLE_OPERATOR,
    clex_Token__ASSIGNMENT_DOUBLE_OPERATOR_END,

    clex_Token_END_OK,
} clex_Token_Kind;

typedef enum {
    clex_Suffix_FLOAT = 1 << 0,

    clex_Suffix_UNSIGNED = 1 << 1,
    clex_Suffix_LONG = 1 << 2,
    clex_Suffix_LONG_LONG = 1 << 3,
    clex_Suffix_BIT_PRECISE_INT = 1 << 4,
} clex_Suffixes;

typedef struct {
    clex_Token_Kind kind;
    clex_Data data, data2;
    clex_Suffixes suffixes;
} clex_Token;

typedef enum {
    clex_PARSE_COMMENTS = 1 << 0,
} clex_Flags;

#if !defined(CLEX_EXPANSION_STACK_DEPTH)
    #define CLEX_EXPANSION_STACK_DEPTH 8
#endif

typedef struct {
    const char *path, *path_end;
    const char *start;
    const char *c;
} clex_Cursor;

typedef const char *clex_define_function(void *user_data, const char *symbol, const char *definition);

typedef struct {
    clex_Cursor stack[CLEX_EXPANSION_STACK_DEPTH];
    int depth;
    clex_Flags flags;
    clex_Token token;
    clex_define_function *define;
    void *define_data;
} clex_Lexer;

#if !defined(CLEX_FUNCTION)
    #define CLEX_FUNCTION
#endif

CLEX_FUNCTION const char *clex_identifier_end(const char *start);
CLEX_FUNCTION clex_Lexer  clex_init(const char *start, clex_Flags flags, clex_define_function *, void *define_data);
CLEX_FUNCTION _Bool       clex_lex(clex_Lexer *l);
CLEX_FUNCTION _Bool       clex_push_file(clex_Lexer *l, const char *path, const char *path_end, const char *start);


#endif // CLEX_H


#if defined(CLEX_IMPLEMENTATION)


#if !defined(CLEX_ASSERT)
    #include <assert.h>
    #define CLEX_ASSERT assert
#endif

static _Bool clex__is_nondigit(char c) {
    _Bool is = c == '_';
    is = is || ('A' <= c && c <= 'Z');
    is = is || ('a' <= c && c <= 'z');
    return is;
}

static _Bool clex__is_digit(char c) { return '0' <= c && c <= '9'; }

CLEX_FUNCTION const char *clex_identifier_end(const char *start) {
    const char *end = &start[1];
    while (clex__is_nondigit(*end) || clex__is_digit(*end) || *end == '$') end += 1;
    return end;
}

CLEX_FUNCTION clex_Lexer clex_init(const char *start, clex_Flags flags, clex_define_function *define, void *define_data) {
    CLEX_ASSERT(define != 0);
    clex_Lexer lexer = {
        .stack[0] = { .start = start, .c = start },
        .flags = flags,
        .define = define,
        .define_data = define_data,
    };
    return lexer;
}

static _Bool clex__is_newline(clex_Cursor *cursor, const char *c) {
    // TODO(felix): remove
    (void)cursor;
    if (*c == '\n') return 1;
    if (*c == '\r' && c[1] == '\n') return 1;
    return 0;
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
    while (!clex__is_newline(c, c->c)) c->c += 1;
}

static void clex__skip_whitespace_except_newline(clex_Cursor *c) {
    for (;; c->c += 1) {
        char ch = *c->c;
        if (ch != '\t' && ch != '\r' && ch != ' ') break;
    }
}

static void clex__skip_whitespace(clex_Cursor *c) {
    for (;; c->c += 1) {
        char ch = *c->c;
        if (ch != '\n' && ch != '\t' && ch != '\r' && ch != ' ' && !(ch == '\\' && c->c[1] != '\\')) break;
    }
}

static _Bool clex__string_equals(const char *start, const char *end, const char *compare) {
    for (const char *c = start; c < end; c += 1) {
        unsigned long long i = (unsigned long long)(c - start);
        if (*c != compare[i]) return 0;
    }
    return 1;
}

#define CLEX__TOKEN_STACK_COUNT 2
typedef struct {
    clex_Token stack[CLEX__TOKEN_STACK_COUNT];
    unsigned count;
} clex__Token_Stack;
static _Bool clex__pop_token(clex__Token_Stack *tokens, clex_Token *popped) {
    if (tokens->count == 0) return 0;
    *popped = tokens->stack[--tokens->count];
    return 1;
}
static void clex__push_token(clex__Token_Stack *tokens, clex_Token push) {
    CLEX_ASSERT(tokens->count < CLEX__TOKEN_STACK_COUNT);
    tokens->stack[tokens->count++] = push;
}

static _Bool clex__skip_escape_sequence(clex_Cursor *c) {
    _Bool ok = 0;
    CLEX_ASSERT(*c->c == '\\');

    switch (*(++c->c)) {
        case '"': case '\'': case '?': case '\\': case 'a': case 'b': case 'f': case 'n': case 'r': case 't': case 'v': {
            ok = 1;
            c->c += 1;
        } break;
        case '0': {
            ok = 1;
            c->c += 1;

            if ('0' <= *c->c && *c->c <= '7') {
                c->c += 1;
                if ('0' <= *c->c && *c->c <= '7') c->c += 1;
            }
        } break;
        case 'x': CLEX_ASSERT(0 && "TODO(felix): hex digit escape sequence");
        default: break;
    }

    return ok;
}

CLEX_FUNCTION _Bool clex_lex(clex_Lexer *l) {
    l->token = (clex_Token){0};

    // TODO(felix): Not threadsafe. Can we replace this anyway?
    static clex__Token_Stack tokens = {0};
    if (clex__pop_token(&tokens, &l->token)) return 1;

    next_token:

    clex_Cursor *c = &l->stack[l->depth];
    clex__skip_whitespace(c);

    if (*c->c == 0) {
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

    switch (*c->c) {
        case '#': {
            // If we find non-whitespace between this and the last newline, this is a punctuator, not a preprocessor directive.
            for (const char *back = &c->c[-1]; back >= c->start; back -= 1) {
                if (*back == '\n') break;
                if (*back == ' ' || *back == '\t') continue;

                l->token = (clex_Token){
                    .kind = (clex_Token_Kind)(*c->c),
                    .data = { .start = c->c, .end = ++c->c },
                };
                return 1;
            }

            c->c += 1;
            clex__skip_whitespace(c);

            const char *directive = c->c;
            clex__skip_identifier(c);
            const char *directive_end = c->c;
            if (directive_end == directive) goto error;

            clex__skip_whitespace(c);
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
                if (d->c == l->token.data.start) goto error;
                l->token.data.end = d->c;

                l->token.data2.start = d->c;
                clex__skip_to_newline(d);
                l->token.data2.end = d->c;

                l->define(l->define_data, l->token.data.start, l->token.data2.start);
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
            } else CLEX_ASSERT(0 && "TODO(felix)");

            return 1;
        } break;
        case '(': case ')': case ',': case '.': case ':': case ';': case'[': case ']': case '{': case '}': case '?': {
            l->token = (clex_Token){
                .kind = (clex_Token_Kind)(*c->c),
                .data = { .start = c->c, .end = ++c->c },
            };
            return 1;
        } break;
        case '0': {
            const char *integer = c->c;
            char next = c->c[1];

            if (next == '.') {
                const char *floating = c->c;

                c->c += 2;
                while ('0' <= *c->c && *c->c <= '9') c->c += 1;
                if (*c->c == 0) goto error;

                const char *floating_end = c->c;
                _Bool float_suffix = *c->c == 'f';
                c->c += float_suffix;

                l->token = (clex_Token){
                    .kind = clex_Token_FLOAT,
                    .data = { .start = floating, .end = floating_end },
                    .suffixes = float_suffix * clex_Suffix_FLOAT,
                };
                return 1;
            }

            if (next == 'b' || next == 'B') {
                CLEX_ASSERT(0 && "TODO(felix)");
            }

            if (next == 'x' || next == 'X') {
                const char *hex = c->c;

                c->c += 2;
                for (;; c->c += 1) {
                    char lowercase = *c->c | 0x20;
                    if (!(('0' <= *c->c && *c->c <= '9') || ('a' <= lowercase && lowercase <= 'f'))) break;
                }

                const char *hex_end = c->c;
                // TODO(felix): suffixes

                l->token = (clex_Token){
                    .kind = clex_Token_INTEGER,
                    .data = { .start = hex, .end = hex_end },
                    // TODO(felix): hex flag
                };
                return 1;
            }

            if ('1' <= next && next <= '7') {
                const char *octal = c->c;

                c->c += 1;
                while ('0' <= *c->c && *c->c <= '7') c->c += 1;
                if (*c->c == '8' || *c->c == '9') goto error;

                const char *octal_end = c->c;
                // TODO(felix): suffixes

                l->token = (clex_Token){
                    .kind = clex_Token_INTEGER,
                    .data = { .start = octal, .end = octal_end },
                };
                return 1;
            }

            if (next == '8' || next == '9') goto error;

            l->token = (clex_Token){
                .kind = clex_Token_INTEGER,
                .data = { .start = integer, .end = ++c->c },
            };
            return 1;
        } break;
        case '!': case '%': case '&': case '*': case '+': case '-': case '/': case '<': case '>': case '^': case '|': // can be immediately followed by `=`
        case '=': case '~':
        {
            _Bool single_line_comment = *c->c == '/' && c->c[1] == '/';
            if (single_line_comment) {
                c->c += 2;
                const char *comment = c->c;
                const char *comment_end = comment;

                while (*c->c != 0 && !clex__is_newline(c, comment_end)) comment_end += 1;
                if (*comment_end == 0) goto error;

                c->c = comment_end + 1;

                if (!(l->flags & clex_PARSE_COMMENTS)) goto next_token;

                l->token = (clex_Token){
                    .kind = clex_Token_COMMENT,
                    .data = { .start = comment, .end = comment_end },
                };
                return 1;
            }

            const char *operator = c->c;

            _Bool doubled = c->c[1] == *c->c;
            c->c += doubled;

            if (doubled) switch (*operator) {
                case '+': case '-': case '<': case '>': case '=': case '&': case '|': break;
                // these are just repeated operators with no special meaning conveyed by this
                case '!': case '~': case '^': case '*': {
                    clex_Token token = {
                        .kind = (clex_Token_Kind)*operator,
                        .data = { .start = operator + 1, .end = operator + 2 },
                    };
                    clex__push_token(&tokens, token);

                    token.data.start += 1;
                    token.data.end += 1;
                    l->token = token;
                    return 1;
                } break;
                default: goto error;
            }

            _Bool assignment = c->c[1] == '=';
            c->c += assignment;

            // only some operators have `!=` form
            if (assignment) switch (*operator) {
                case '!': case '<': case '>': case '*': case '/': case '%': case '+': case '-': case '&': case '^': case '|': break;
                default: goto error;
            }

            const char *operator_end = ++c->c;

            // only `<<=` and `>>=` have this form
            if (doubled && assignment && *operator != '<' && *operator != '>') goto error;

            clex_Token_Kind kind = (clex_Token_Kind)(*operator + doubled * clex_Token__DOUBLE_OPERATOR + assignment * clex_Token__ASSIGNMENT_OPERATOR);

            l->token = (clex_Token){
                .kind = kind,
                .data = { .start = operator, .end = operator_end },
            };
            return 1;
        } break;
        case '\'': {
            // TODO(felix): handle encoding prefix somewhere, probably in the identifier parsing logic

            if (c->c[1] == 0 || c->c[2] == 0) goto error;

            c->c += 1;
            const char *character = c->c;

            const char *character_end = c->c + 1;
            if (*c->c == '\\') {
                if (!clex__skip_escape_sequence(c)) goto error;
                character_end = c->c;
            }

            if (*character_end != '\'') goto error;
            c->c += 2; // after closing quote

            l->token = (clex_Token){
                .kind = clex_Token_INTEGER,
                .data = { .start = character, .end = character_end },
                // TODO(felix): flag for character literal
            };
            return 1;
        } break;
        case '"': {
            // TODO(felix): handle encoding prefix somewhere, probably in the identifier parsing logic
            c->c += 1;
            if (*c->c == 0) goto error;

            const char *string = c->c;

            while (*c->c != 0) {
                if (*c->c == '\\') {
                    if (!clex__skip_escape_sequence(c)) goto error;
                    continue;
                }
                if (*c->c == '"') break;
                if (clex__is_newline(c, c->c)) goto error;

                c->c += 1;
            }
            CLEX_ASSERT(*c->c != 0);

            const char *string_end = c->c++;
            l->token = (clex_Token){
                .kind = clex_Token_STRING,
                .data = { .start = string, .end = string_end },
                // TODO(felix): prefix
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

    if ('1' <= *c->c && *c->c <= '9') {
        const char *number = c->c;
        while ('0' <= *c->c && *c->c <= '9') c->c += 1;

        _Bool floating = *c->c == '.';
        c->c += floating;
        if (floating) while ('0' <= *c->c && *c->c <= '9') c->c += 1;
        const char *number_end = c->c;

        _Bool unsigned_suffix = 0, long_suffix = 0, long_long_suffix = 0, bit_suffix = 0, floating_suffix = 0;
        for (;;) {
            const char *old_c = c->c;

            floating_suffix = *c->c == 'f';
            c->c += floating_suffix;

            unsigned_suffix = (*c->c | 0x20) == 'u';
            c->c += unsigned_suffix;

            long_suffix = (*c->c | 0x20) == 'l';
            long_long_suffix = long_suffix && c->c[1] == *c->c;
            long_suffix = long_suffix && !long_long_suffix;
            c->c += long_suffix;
            c->c += 2 * long_long_suffix;

            bit_suffix = 0;
            if ((*c->c | 0x20) == 'w' && c->c[1] != 0) {
                char next = 'B' | (*c->c & 0x20);
                bit_suffix = c->c[1] == next;
            }
            c->c += 2 * bit_suffix;

            _Bool no_more_suffixes = c->c == old_c;
            if (no_more_suffixes) break;
        }

        clex_Suffixes suffixes = 0;
        suffixes |= floating_suffix * clex_Suffix_FLOAT;
        suffixes |= unsigned_suffix * clex_Suffix_UNSIGNED;
        suffixes |= long_suffix * clex_Suffix_LONG;
        suffixes |= long_long_suffix * clex_Suffix_LONG_LONG;
        suffixes |= bit_suffix * clex_Suffix_BIT_PRECISE_INT;

        l->token = (clex_Token){
            .kind = floating ? clex_Token_FLOAT : clex_Token_INTEGER,
            .data = { .start = number, .end = number_end },
            .suffixes = suffixes,
        };
        return 1;
    }

    CLEX_ASSERT(0 && "TODO(felix)");

    error:
    l->token = (clex_Token){0};
    return 0;
}

CLEX_FUNCTION _Bool clex_push_file(clex_Lexer *l, const char *path, const char *path_end, const char *start) {
    _Bool had_enough_space = 0;

    if (l->depth + 1 < CLEX_EXPANSION_STACK_DEPTH) {
        clex_Cursor cursor = {
            .path = path,
            .path_end = path_end,
            .start = start,
            .c = start,
        };
        l->stack[++l->depth] = cursor;
        had_enough_space = 1;
    }

    return had_enough_space;
}


#endif // CLEX_IMPLEMENTATION
