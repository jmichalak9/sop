#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

//volatile sig_atomic_t last_signal = 0;

#define ERR(source) (fprintf(stderr,"%s:%d\n",__FILE__,__LINE__),\
                     perror(source),kill(0,SIGKILL),\
		     		     exit(EXIT_FAILURE))


void usage(char *name)
{
	fprintf(stderr,"USAGE: %s x x x \n",name);
	fprintf(stderr,"x - \n");
	fprintf(stderr,"x - \n");
	fprintf(stderr,"x - \n");
	fprintf(stderr," \n");
	exit(EXIT_FAILURE);
}

void sethandler( void (*function)(int), int sig_name) 
{
	struct sigaction action;
	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = function;
	if (-1==sigaction(sig_name, &action, NULL)) ERR("sigaction");
}


int main(int argc, char** argv) 
{
    if(argc != 2) usage(argv[0]);

	//while(wait(NULL)>0) {};
	return EXIT_SUCCESS;
}