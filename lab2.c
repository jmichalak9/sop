#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#define ERR(source) (perror(source),\
            fprintf(stderr,"%s:%d\n",__FILE__,__LINE__),\
            exit(EXIT_FAILURE))
#define BUF_SIZE 500
#define MAXN 10
volatile sig_atomic_t usr1Count = 0, usr2Count = 0, al = 0;


void usage(char* pname) {
    fprintf(stderr, "Usage: %s [OPTION]...\n"
                    "Sprawdza, czy ciag jest ciagiem geometrycznym.\n"
                    "  -l LICZBA\t\tkolejny wyraz ciagu, min. 2, max. 10\n"
                    "  -k KATALOG\t\ttworzy KATALOG spelniajacy warunki zadania\n"
                    "  -p \t\ttworzy plik, tylko z flaga -k\n",
                    pname);
    exit(EXIT_FAILURE);
}
void sethandler( void (*f)(int), int sigNo) {
	struct sigaction act;
	memset(&act, 0, sizeof(struct sigaction));
	act.sa_handler = f;
	if (-1==sigaction(sigNo, &act, NULL)) ERR("sigaction");
}
void sig_handler(int sig) {
	//printf("[%d] received signal %d\n", getpid(), sig);
	if (SIGUSR1 == sig)
        usr1Count++;
    if (SIGUSR2 == sig)
        usr2Count++;
    if (SIGALRM == sig)
        al = 1;
}

void child_work(int k, int x) {

	for(int tt=k;tt>0;tt=sleep(tt));
    int dz = x/10, je = x%10;
    for (int i = 0; i < dz; i++) {
        if (kill(0, SIGUSR1)<0)ERR("kill");
        struct timespec t = {0, 20*1000*1000};
        nanosleep(&t,NULL);
    }
    for (int i = 0; i < je; i++) {
        if (kill(0, SIGUSR2)<0)ERR("kill");
        struct timespec t = {0, 20*1000*1000};
        nanosleep(&t,NULL);
    }
}

void create_children(int n, int PID[n][3]) {
	pid_t s;
	for (int k = 1; k <= n; k++) {
		if((s=fork())<0) ERR("Fork:");
		if(!s) {
        	sethandler(SIG_IGN,SIGUSR1);
        	sethandler(SIG_IGN,SIGUSR2);
            printf("child %d k=%d\n", getpid(), k);
            srand(time(NULL)*getpid());	
	        int x = rand()%49 + 1;
			child_work(k, x);
            printf("child %d x=%d after %d seconds\n", getpid(), x, k);
			exit(x);
		}
        PID[k-1][0] = s;
	}
}

int main(int argc, char** argv) {
	int n;
	if(argc!=2)  usage(argv[0]);
	n=atoi(argv[1]);
	if(n<=0)  usage(argv[0]);
    int PID[n][3];
    sethandler(sig_handler,SIGUSR1);
    sethandler(sig_handler,SIGUSR2);
    sethandler(sig_handler,SIGALRM);
    create_children(n, PID);
	for (int i = 1; i <= n; ) {
        
        pid_t pid;
        int status;
		pid=waitpid(PID[i-1][0], &status, 0);
		if(pid>0) {
            if ( WIFEXITED(status) ) {
                PID[i-1][1] = WEXITSTATUS(status);
            }

            PID[i-1][2] = usr1Count*10 + usr2Count;
            usr1Count = usr2Count = 0;
            i++;
        }
		if(0==pid) break;
		if(0>=pid) {
            if(EINTR==errno)
                continue;
            ERR("waitpid");
		}
    	printf("PARENT: %d processes remain\n",n-i+1);

    }
    printf("parent %d dies\n", getpid());
    for (int i = 1; i <= n; i++){
        printf("child %d, x sent %d, x recvd %d\n", PID[i-1][0], PID[i-1][1], PID[i-1][2]);
    }
	return EXIT_SUCCESS;
}