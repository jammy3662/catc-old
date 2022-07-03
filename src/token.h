#pragma once
#include "ext.h"

// +,.<>/|\\!@#$%^&*(){}[]:;\'\"`~=

// valid characters for identifiers
char* const alpha = "_ghijklmnopqrstuvwxyzGHIJKLMNOPQRSTUVWXYZabcdefABCDEF";
// valid characters for numerics
char* const numeric = "_0123456789";

enum TOKEN
{
	NONE = -1,
	
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
	
	// --- USER DEFINES ---
	STRUCT,
	PUBLIC,
	PRIVATE,
	UNION,
	LET,
	FN,
	NAME, // anything not mapped to the language
	
	// --- LITERALS ---
	
	   INT_LITERAL, // 235
	   HEX_LITERAL, // 12.45456
	   BIN_LITERAL, // 0b011011
	   OCT_LITERAL, // 0o734721
	   
	 FLOAT_LITERAL, // 12.45456
	 
	  BOOL_LITERAL, // true | false
	  CHAR_LITERAL, // 'e'
	STRING_LITERAL, // "sdfa35 sd f"
	
	// --- BUILTIN TYPES ---
	
	  BYTE_TYPE,
	 SHORT_TYPE,
	   INT_TYPE,
	  LONG_TYPE,
	
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

struct Token
{
	short id = 0;
	long pos = 0; // position in (input) file
	char* value = 0x0;
};

Token get_token(FILE* file)
{
	static long fpos = 0;
	static char current = fgetc(file);
	static bool skip = false;
	
	Token token;
	token.pos = fpos;
	
	// 7 character buffer, 1 terminator byte
	string str = string(8);
	
	byte comment = 0;

read:
	if (current == EOF) goto done;
	
	if      (str.last == 0 and strhaschar(numeric + 1, current)) token.id = INT_LITERAL;
	else if (str.last == 0 and strhaschar(alpha,       current)) token.id =        NAME;
	
	// some numeric token
	if (token.id > NAME and token.id < BOOL_LITERAL)
	{
		// support for formatted numeric literals
		if (str.last == 1)
		{
			switch (current)
			{
				// (binary 0b101101)
				case 'b':
					token.id = BIN_LITERAL;
					goto next;
				// (hex 0x1fe55a)
				case 'x':
					token.id = HEX_LITERAL;
					goto next;
				// (octal 0o715237)
				case 'o':
					token.id = OCT_LITERAL;
					goto next;
				default:
					break;
			}
		}
		
		if (!strhaschar(numeric, current))
		{
			// allow characters 'abcdefABCDEF' in hex literals
			if (strhaschar(alpha + 41, current) and token.id == HEX_LITERAL) goto next;
			// allow a decimal point to turn an int into a float
			if (current == '.' and token.id != FLOAT_LITERAL) { token.id = FLOAT_LITERAL; goto next; }
			// otherwise, character is not valid for the numeric token; finish token
			goto done;
		}
		goto next;
	}
	
	#define key(ID,TOK) if(streq(ID,str.data)) { token.id = TOK; goto done; }
	
	// some alphabetical token like a name or identifier
	if (token.id == NAME)
	{
		// character is not valid for an identifier; finish token
		if (!strhaschar(alpha, current) and !strhaschar(numeric + 1, current))
		{
			// keywords and builtins
			key ("byte",       BYTE_TYPE)
			key ("short",      SHORT_TYPE)
			key ("int",        INT_TYPE)
			key ("long",       LONG_TYPE)
			key ("float",      FLOAT_TYPE)
			key ("double",     DOUBLE_TYPE)
			key ("bool",       BOOL_TYPE)
			key ("char",       CHAR_TYPE)
			key ("string",     STRING_TYPE)
			key ("slice",      SLICE_TYPE)
			key ("array",      ARRAY_TYPE)
			
			// user symbols
			key ("struct",     STRUCT)
			key ("public",     PUBLIC)
			key ("private",    PRIVATE)
			key ("union",      UNION)
			
			// control flow
			key ("if",         IF)
			key ("elif",       ELIF)
			key ("else",       ELSE)
			
			key ("for",        FOR)
			key ("in",         IN)
			key ("while",      WHILE)
			key ("continue",   CONTINUE)
			key ("break",      BREAK)
			
			key ("key",        SWITCH)
			key ("case",       CASE)
			key ("default",    DEFAULT)
			
			key ("key",        GOTO)
			key ("return",     RETURN)
			
			key ("start",      START)
			key ("end",        END)
			
			// attributes
			key ("const",      CONST)
			key ("static",     STATIC)
			key ("extern",     EXTERN)
			key ("volatile",   VOLATILE)
			
			key ("stack",      STACK)
			key ("heap",       HEAP)
			
			// preprocessor
			key ("alias",      ALIAS)
			
			goto done;
		}
		else goto next;
	}
	
	#define match(C,TOK) if (str.data[str.last-1] == C) { token.id = TOK; goto done; }
	
	#define match2(C1,C2,TOK)\
		if ( str.last > 0               and\
			 str.data[str.last-1] == C1 and\
			 current == C2 )\
		{\
			token.id = TOK;\
				str.append(current);\
				current = fgetc(file);\
				fpos++;\
			goto done;\
		}
	
	match2 ('/','*',  COMMENT_L)
	match2 ('*','/',  COMMENT_R)
	match2 ('/','/',  COMMENT)
	
	match  ('+',      PLUS)
	match  ('-',      MINUS)
	match  ('*',      STAR)
	match  ('/',      SLASH)
	
	match2 ('<','=',  LESSEQUAL)
	match2 ('>','=',  MOREEQUAL)
	match2 ('<','<',  SHIFT_L)
	match2 ('>','>',  SHIFT_R)
	
	match ('.',       DOT)
	match (',',       COMMA)
	match ('<',       LESSTHAN)
	match ('>',       MORETHAN)
	
	match2 ('=','=',  EQUAL)
	match  ('=',      ASSIGN)
	
	match ('\\',      BACKSLASH)
	match ('|',       PIPE)
	match ('/',       SLASH)
	
	match ('!',       BANG)
	match ('?',       QUESTION)
	match ('@',       AT)
	match ('#',       HASH)
	match ('$',       DOLLAR)
	match ('%',       PERCENT)
	match ('^',       CARET)
	match ('&',       AMPERSAND)
	
	match ('{',       BRACE_L)
	match ('}',       BRACE_R)
	match ('[',       BRACKET_L)
	match (']',       BRACKET_R)
	match ('(',       PARENTHESES_L)
	match (')',       PARENTHESES_R)
	
	match (':',       COLON)
	match (';',       SEMICOLON)
	match ('\'',      QUOTE)
	match ('"',       DQUOTE)
	match ('`',       BACKTICK)
	match ('~',       TILDE)
	
next:
	// write current character into string
	str.append(current);
	if (!str.space()) str.grow();
	
	// move to next character in input stream
	current = fgetc(file);
	fpos++;
	
	goto read;
	
done:
	while (current == ' ' or current == '\t' or current == '\n' or current == '\r' or current == '\v' or current == '\f')
	{
		fpos++;
		current = fgetc(file);
	}
	
	printf("%i: %s\n", token.id, str.data);
	
	str.trim();
	token.value = str.data;
	return token;
}