#pragma once

#include <stdlib.h>
#include <stdio.h>

// type based allocation (using malloc)
#define talloc(COUNT, TYPE) (TYPE*)malloc(COUNT * sizeof(TYPE))
#define tcalloc(COUNT, TYPE) (TYPE*)calloc(COUNT, sizeof(TYPE))
#define trealloc(PTR, COUNT, TYPE) (TYPE*)realloc(PTR, COUNT * sizeof(TYPE))

/* dont think i actually need this

// characters considered to mark the end of the current token being read
char* word_terminators = " \t\n+,.<>/|\\!@#$%^&*(){}[]:;\'\"`~=";

*/

// valid characters for identifiers
char* alpha = "_ghijklmnopqrstuvwxyzGHIJKLMNOPQRSTUVWXYZabcdefABCDEF";
// valid characters for numerics
char* numeric = "_0123456789";

enum
{
	t_null = -1, // invalid token (error handling)
	
	t_idtf, // identifier
	t_const,
	
	t_endcmd,
	t_endscope,
	t_closepar,
	t_openpar,
	
	t_type,
	
	t_include,
	t_define,
	t_alias,
	
	t_func,
	t_section,
	t_goto,
	t_for,
	t_while,
	t_if,
	t_else,
	t_elif,
	t_switch,
	t_continue,
	t_break,
	t_return,
};

// user modifiable for dynamic custom syntax
typedef struct {char* str; short id;} __tokendict;
__tokendict tokenid[] = 
{
	// syntax
	{";", t_endcmd}, // end command
	{"(", t_openpar},
	{")", t_closepar},
	{"end", t_endscope}, // close scope
	
	// builtins
	{"byte", t_type},
	{"short", t_type},
	{"int", t_type},
	{"long", t_type},
	
	{"float", t_type},
	{"double", t_type},
	
	{"char", t_type},
	
	// compiler directives
	{"include", t_include},
	{"define", t_define},
	{"alias", t_alias},
	
	// control flow
	{"fn", t_func},
	{"section", t_section},
	{"goto", t_goto},
	{"for", t_for},
	{"while", t_while},
	{"if", t_if},
	{"else", t_else},
	{"elif", t_elif}, // maps to else + if
	{"switch", t_switch},
	{"continue", t_continue},
	{"break", t_break},
	{"return", t_return},
};

short process_word(char* word)
{
	if (*word != '\n') printf("'%s'\n", word);
	else printf("'\\n'\n");
	
	if (*word == '\n') return t_endcmd;
	//if (strhas(numeric, *word) > -1) return t_
	
	for (int i = 0; i < sizeof(tokenid) / sizeof(__tokendict); i++)
	{
		if (streq(word, tokenid[i].str)) return tokenid[i].id;
	}
	return t_idtf;
}

struct Token
{
	short id = 0;
	long fpos = 0; // position in (input) file
	char* value = 0x0;
};

Token get_token(FILE* file, char& current, long& fpos)
{
	Token token;
	token.fpos = fpos;
	
	short token_size = 8;
	
	// actual content pulled from file
	char* part = tcalloc(token_size + 1, char);
	part[token_size] = 0;
	
	// position in current token
	short idx = 0;
	
	bool name = false;
	
	bool  number = false;
	bool decimal = false;
	bool  binary = false;
	bool     hex = false;
	
	// special logic for the first character in a token
	// (identifiers/names cannot start with a number)
	// (numbers cannot start with a decimal)
	
	if (strhaschar(numeric + 1, current))
	{
		number = true;
		goto update;
	}
	if (strhaschar(alpha, current))
	{
		name = true;
		goto update;
	}
	
read:
	
	// token is a number
	if (number)
	{
		// binary and hex formatted numeric literals
		if (idx == 1)
		{
			if (current == 'b')
			{
				binary = true;
				goto update;
			}
			if (current == 'x')
			{
				hex = true;
				goto update;
			}
		}
		
		// decimal (floating point) numeric literals
		if (current == '.')
		{
			if (!decimal and !binary and !hex)
			{
				decimal = true;
				goto update;
			}
			else goto interrupt;
		}
		
		// no longer a number, end token
		if (!strhaschar(numeric, current))
		{
			if (hex and strhaschar(alpha + 41, current)) goto update;
			goto interrupt;
		}
		goto update;
	}
	
	// token is not a number
	// if its an alphabetical token and the current character is alphabetical, keep reading
	if (name)
	{
		if (strhaschar(alpha, current) or strhaschar(numeric + 1, current)) goto update;
		else goto interrupt;
	}
	
	if (current == '\n') printf("newline\n");
	
	goto finish;
	
update:
	
	// write current character into string
	part[idx] = current;
	
	idx++;
	// if the string needs more space, double its size
	// likelihood of resizing decreases quadratically with each resize
	if (idx == token_size - 1)
	{
		token_size *= 2;
		part = trealloc(part, token_size + 1, char);
		part[token_size] = 0;
	}
	
	fpos++;
	current = fgetc(file);
	goto read;
	
interrupt:
	
	// get rid of whitespace to avoid reading empty token next call
	while (current == ' ' or current == '\t' or current == '\r' or current == '\v' or current == '\f')
	{
		fpos++;
		current = fgetc(file);
	}
	
	token.value = part;
	
	if (number and !decimal and !hex and !binary) printf("number: %s\n", part);
	if (number and decimal) printf("float: %s\n", part);
	if (number and hex) printf("hex: %s\n", part);
	if (number and binary) printf("binary: %s\n", part);
	if (name) printf("alphabetical: %s\n", part);
	//else printf("thing: %s\n", part);
	
	return token;
	
finish:
	
	fpos++;
	current = fgetc(file);
	
	// get rid of whitespace to avoid reading empty token next call
	while (current == ' ' or current == '\t' or current == '\r' or current == '\v' or current == '\f')
	{
		fpos++;
		current = fgetc(file);
	}
	
	printf("_\n");
	return token;
	
}

void get_tokens(FILE* file, Array<Token>& tokens)
{
	long fpos = 0;
	char current = fgetc(file);
	
	while (current != EOF)
	{
		tokens.append(get_token(file, current, fpos));
	}
}