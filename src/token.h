#pragma once
#include "ext.h"

// token id enum and character arrays
#include "const.h"

struct Token
{
	int16 id = -1;
	int64 pos = 0; // position in (input) file
	char* value = 0x0;
};

enum TYPE
{
	INVALID = 0,
	ALPHA, INT,
	NUMERIC, SYMBOL,
	CMNT, STR,
	CHAR,
};

bool token_finished(char next, int8* type, int16* id)
{
	static bool escape = false;
	
	if (next == EOF) return true;
	
// ----- first character -----
	if (*type == INVALID)
	{
		if (next == '"')
		{
			*type = STR; 
			*id = STRING_LITERAL;
		}
		
		else if (next == '\'')
		{
			*type = CHAR;
			*id = CHAR_LITERAL;
		}
		
		else if (strhaschar(alpha, next)) *type = ALPHA;
		
		else if (strhaschar(numeric + 1, next)) *type = INT;
		
		else *type = SYMBOL;
		
		return false;
	}
// ----- following first character -----
	
	if (*type == STR or *type == CHAR)
	{
		if (escape)
		{
			escape = false;
			return false;
		}
		if (next == '\\')
		{
			escape = true;
			return false;
		}
		return (next == ((*type == STR) ? '"' : '\''));
	}
	
	if (*type == ALPHA)
	{
		return (!strhaschar(alpha, next) and
		    !strhaschar(numeric + 1, next));
	}
	
	// second character of a number
	if (*type == INT)
	{
		*type = NUMERIC;
		
		if (next == 'b')
		{
			*id = BIN_LITERAL;
			return false;
		}
		
		if (next == 'x')
		{
			*id = HEX_LITERAL;
			return false;
		}
		
		if (next == 'o')
		{
			*id = OCT_LITERAL;
			return false;
		}
		
		*id = INT_LITERAL;
		
		return !strhaschar(numeric, next);
	}
	
	if (*type == NUMERIC)
	{
		if (next == '.' and *id == INT_LITERAL)
		{
			*id = FLOAT_LITERAL;
			return false;
		}
		
		// continue token if character is numeric
		// or if token is hex and character is a-f or A-F
		return !(strhaschar(numeric, next) or
		        (*id == HEX_LITERAL and strhaschar(alpha + 41, next)));
	}
	
	return true;
}

void token_advance(string* str, char next)
{
	// if needed, expand string buffer
	// then copy character into buffer
	if (!str->space()) str->grow();
	str->append(next);
}

bool is_whitespace(char c)
{
	return (c == ' ' or c == '\t' or
	        c == '\n' or c == '\r' or
		   c == '\v' or c == '\f');
}

Token get_token(FILE* file)
{
	static int64 pos = 0;
	static char next = fgetc(file);
	static bool cmnt = false;
	
	#define getnext() next = fgetc(file); pos++;
	
	Token token;
	token.pos = pos;
	
	string str = string(8);
	int8 type = INVALID;
	
	// flush space
	while (is_whitespace(next)) getnext();
	
	if (next == EOF)
	{
		token.id = 0;
		goto finalize;
	}
	
	while (!token_finished(next, &type, &token.id))
	{
		token_advance(&str, next);
		getnext();
	}
	
	if (type == STR or type == CHAR)
	{
		token_advance(&str, next);
		getnext();
	}
	
	#define key(ID,TOK) if(streq(ID,str.data)) { token.id = TOK; goto finalize; }
	
	if (type == ALPHA)
	{
		// keywords and builtins
		key ("int",        ARCH_TYPE)
		key ("int8",       INT8_TYPE)
		key ("int16",      INT16_TYPE)
		key ("int32",      INT32_TYPE)
		key ("int64",      INT64_TYPE)
		
		key ("uint",       ARCH_UNSIGNED_TYPE)
		key ("uint8",      INT8_UNSIGNED_TYPE)
		key ("uint16",     INT16_UNSIGNED_TYPE)
		key ("uint32",     INT32_UNSIGNED_TYPE)
		key ("uint64",     INT64_UNSIGNED_TYPE)
		
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
		
		key ("and",        AND)
		key ("or",         OR)
		
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
		
		token.id = NAME;
		goto finalize;
	}
	
	#define match(C,TOK) if (*str.data == C) { token.id = TOK; goto finalize; }
	#define match2(C1,C2,TOK) if (*str.data == C1 and next == C2) {\
		token.id = TOK;\
		token_advance(&str, next);\
		getnext();\
		goto finalize;\
	}
	
	if (type == SYMBOL)
	{
		if (*str.data == '/' and next == '*')
		{
			char current = next;
			getnext();
			current = next;
			getnext();
			
			while (current != '*' or next != '/')
			{
				if (current == EOF or next == EOF)
				{
					token.id = NONE;
					str.clear(); str = string("");
					goto finalize;
				}
				
				current = next;
				next = getnext();
			}
			
			next = getnext();
			token.id = COMMENT_BLOCK;
			str.clear(); str = string("");
			goto finalize;
		}
		
		if (*str.data == '/' and next == '/')
		{
			getnext()
			while (next != '\n') getnext();
			token.id = COMMENT;
			str.clear(); str = string("");
			goto finalize;
		}
		
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
	}
	
finalize:
	token.value = str.data;
	printf("%i: %s\n", token.id, token.value);
	return token;
}