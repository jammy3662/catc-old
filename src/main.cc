#include <stdio.h>
#include <stdlib.h>

#include "ext.h"
#include "token.h"

int main(int argc, char** argv)
{
	if (argc < 2) 
	{
		printf("Provide input file (first argument)\n");
		return 1;
	}
	
	FILE* file = fopen(argv[1], "r");
	Array<Token> tokens;
	tokens.allocate(200);
	get_tokens(file, tokens);
	return 0;
}

