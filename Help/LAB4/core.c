#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <aio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#define BLOCKS 3
//volatile sig_atomic_t work;
void error(char* mesage)
{
	perror(mesage);
	exit(EXIT_FAILURE);
}

#define ERR(source) (fprintf(stderr,"%s:%d\n",__FILE__,__LINE__),\
                     perror(source),kill(0,SIGKILL),\
		     		     exit(EXIT_FAILURE))

void usage(char *name)
{
	fprintf(stderr, "USAGE: %s x x\n", name);
	fprintf(stderr, "x - \n");
	fprintf(stderr, "x - \n");
	fprintf(stderr, "\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]){
	int filedesc;
	string filename;
	if ((filedesc = TEMP_FAILURE_RETRY(open(filename, O_RDWR))) == -1) error("Cannot open file");
    /**/
	if (TEMP_FAILURE_RETRY(close(filedesc)) == -1) error("Cannot close file");
	return EXIT_SUCCESS;
}