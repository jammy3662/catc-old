#pragma once

#include <stdlib.h>
#include <stdio.h>

typedef char* string;
typedef char byte;

string word_terminators = " \t\n+,.<>/|\\!@#$%^&*(){}[]:;\'\"`~=";
string numeric = "0123456789";

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
typedef struct {string str; short id;} __tokendict;
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

short process_word(string word)
{
	if (*word != '\n') printf("'%s'\n", word);
	else printf("'\\n'\n");
	
	if (*word == '\n') return t_endcmd;
	if (strhas(numeric, *word) > -1) return t_
	
	for (int i = 0; i < sizeof(tokenid) / sizeof(__tokendict); i++)
	{
		if (streq(word, tokenid[i].str)) return tokenid[i].id;
	}
	return t_idtf;
}

struct Token
{
	short id;
	long fpos; // position in (input) file
};

// assumes a valid file descriptor seeked to beginning
Array<Token> tokenize(FILE* file)
{
	Array<Token> tokens;
	
	long fpos = 1;
	
	new_word:
	
	Token curtok;
	curtok.fpos = fpos;
	
	bool number = false;
	int bufln = 10; // length of string buffer
	int bufpos = 0; // index position in current buffer
	int bufidx = 0; // number of extra buffers allocated for word
	string word = (string) malloc(sizeof(char) * bufln);
	
	char curc = 0; // previous character read from file, current in memory
	
	// null-terminated buffer for non-whitespace word terminators
	// flushed every full_word
	char terminal[2] = {0,0};
	
	read_word:
	
	for (bufpos = 0; bufpos < bufln; bufpos++)
	{
		curc = fgetc(file);
		fpos++;
		
		if (curc == EOF) goto finalize;
		
		if (curc == '-' && bufpos == 0) // distinguish unary subtraction operator
		{
			*terminal = '-';
			goto full_word;
		}
		
		if (strhas(numeric, curc) && bufpos == 0) numeric = true;
		
		int tpos;
		if (!numeric) tpos = strhas(word_terminators, curc);
		else          tpos = !strhas(numeric);
		if (tpos)
		{
			if (tpos > 2 && curc != '.' && !number) *terminal = curc;
			else *terminal = 0;
			goto full_word;
		}
		*(word + (bufidx * bufln) + bufpos) = curc;
	}
	
	bufidx++;
	word = (string) realloc(word, sizeof(char) * (bufln * bufidx + 1));
	goto read_word; // continue reading until a word terminator is found
	
	full_word:
	
	*(word + (bufidx * bufln) + bufpos) = 0; // null terminate string
	
	if (*word != 0)  // ignore empty strings caused by consecutive word terminators
	{
		short id = process_word(word);
		printf("%i\n", id);
		curtok.id = id;
		tokens.append(curtok);
	}
	
	free(word);
	
	// flush word terminator buffer by processing as word (like it should be)
	if (*terminal != 0) 
	{
		short id = process_word(terminal);
		printf("%i\n", id);
		curtok.fpos = fpos - 1;
		tokens.append(curtok);
		
		*terminal = 0;
	}
	
	goto new_word;
	
	finalize:
	
	return tokens;
}