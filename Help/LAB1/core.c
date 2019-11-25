#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERR(source) (perror(source),\
		     fprintf(stderr,"%s:%d\n",__FILE__,__LINE__),\
		     exit(EXIT_FAILURE))

void usage(char* pname)
{
	fprintf(stderr,"USAGE:%s dsc how to use function\n",pname);
	exit(EXIT_FAILURE);
}// ./prog.c [-n NUMBER -o FILE_NAME] ...

int main(int argc, char** argv) 
{
	return EXIT_SUCCESS;
}