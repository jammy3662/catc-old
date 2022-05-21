#include <stdio.h>
#include <stdlib.h>

#include "ext.h"
#include "token.h"

int main(int argc, char** argv)
{
	FILE* file = fopen(argv[1], "r");
	Array<Token> tokens = tokenize(file);
	return 0;
}

