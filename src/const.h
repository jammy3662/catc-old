#pragma once

// +,.<>/|\!@#$%^&*(){}[]:;'"`~=

// valid characters for identifiers
char* const alpha = "_ghijklmnopqrstuvwxyzGHIJKLMNOPQRSTUVWXYZabcdefABCDEF";
// valid characters for numerics
char* const numeric = "_0123456789";

enum TOKEN {

NONE = -1,

_EOF = 0,

// --- LITERALS ---

INT_LITERAL, // 235
HEX_LITERAL, // 12.45456
BIN_LITERAL, // 0b011011
OCT_LITERAL, // 0o734721

FLOAT_LITERAL, // 12.45456

BOOL_LITERAL, // true | false
CHAR_LITERAL, // 'e'
STRING_LITERAL, // "sdfa35 sd f"

// --- SYMBOLS ---
PLUS,          // +
MINUS,         // -
STAR,          // *
SLASH,         // /
BACKSLASH,     // (\)
PIPE,          // |   (pip)

DOT,           // .
COMMA,         // ,
LESSTHAN,      // <
MORETHAN,      // >
LESSEQUAL,     // <=
MOREEQUAL,     // >=
SHIFT_L,       // <<
SHIFT_R,       // >>

EQUAL,         // ==
ASSIGN,        // =

BANG,          // !
QUESTION,      // ?
AT,            // @
HASH,          // #
DOLLAR,        // $
PERCENT,       // %
CARET,         // ^
AMPERSAND,     // &

BRACE_L,       // {
BRACE_R,       // }
BRACKET_L,     // [
BRACKET_R,     // ]
PARENTHESES_L, // (
PARENTHESES_R, // )

COLON,         // :
SEMICOLON,     // ;
QUOTE,         // '
DQUOTE,        // "
BACKTICK,      // `
TILDE,         // ~

COMMENT_L,     // /*
COMMENT_R,     // */
COMMENT,       // //
COMMENT_BLOCK,

// --- USER DEFINES ---
STRUCT,
PUBLIC,
PRIVATE,
UNION,
LET,
FN,
NAME, // anything not mapped to the language

// --- BUILTIN TYPES ---

INT8_TYPE,
INT16_TYPE,
INT32_TYPE,
INT64_TYPE,
ARCH_TYPE,

INT8_UNSIGNED_TYPE,
INT16_UNSIGNED_TYPE,
INT32_UNSIGNED_TYPE,
INT64_UNSIGNED_TYPE,
ARCH_UNSIGNED_TYPE,

FLOAT_TYPE,
DOUBLE_TYPE,

BOOL_TYPE,

CHAR_TYPE,
STRING_TYPE,

SLICE_TYPE,
ARRAY_TYPE,

// --- CONTROL FLOW ---
IF,
ELIF,
ELSE,

AND,
OR,

FOR,
IN,
WHILE,
CONTINUE,
BREAK,

SWITCH,
CASE,
DEFAULT,

GOTO,
RETURN,

START,
END,

// --- ATTRIBUTES ---
CONST,
STATIC,
EXTERN,
VOLATILE,

STACK,
HEAP,

// --- PREPROCESSOR ---
ALIAS,

};