#include <stdio.h>
#include <stdlib.h>

#define ZALGO_EXT_IO

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
	
	while (get_token(file).id);
	
	return 0;
}

