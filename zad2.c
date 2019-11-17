/* Rozwiazanie zadania 2 z tutoriala. Dzieci jednak obsluguja o 1 SIGUSR1
 * za malo. Mozna sprobowac zrobic alarm w 1 miejscu.
 *
 */
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
#define BUFSIZE 1024
#define ERR(source) (fprintf(stderr,"%s:%d\n",__FILE__,__LINE__),\
                     perror(source),kill(0,SIGKILL),\
                     exit(EXIT_FAILURE))

/* sigCount zlicza odebrane SIGUSR1
 * lastSignal zapamietuje ostatni odebrany sygnal
 * alarmReceived ma informacje, czy wlaczyl sie juz alarm
 * blockCount zlicza ilosc zapisanych blokow do pliku */
volatile sig_atomic_t sigCount, lastSignal, alarmReceived, blockCount;

void sethandler( void (*f)(int), int sigNo) {
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    act.sa_handler = f;
    if (-1==sigaction(sigNo, &act, NULL)) ERR("sigaction");
}

void sigHandler(int sig) {
    if (SIGUSR1 == sig)
        sigCount++;
    if (SIGALRM == sig)
        alarmReceived = 1;
    lastSignal = sig;
}

ssize_t bulk_write(int fd, char *buf, size_t count){
    ssize_t c;
    ssize_t len=0;
    do{
        c=TEMP_FAILURE_RETRY(write(fd,buf,count));
        if(c<0) return c;
        buf+=c;
        len+=c;
        count-=c;
    }while(count>0);
    return len ;
}
void childWork(int n) {
    srand(getpid());
    int s = (rand() % 91 + 10) * 1024;

    // tworzenie pliku
    int out;
    char name[BUFSIZE];
    snprintf(name, BUFSIZE, "%d.txt", getpid());
    if ((out = TEMP_FAILURE_RETRY(open(name,O_WRONLY|O_CREAT|O_TRUNC|O_APPEND,0777))) < 0)
        ERR("open");

    // przygotowanie bloku
    char *block = malloc(sizeof(char) * s);
    for (int i = 0; i < s; i++) {
        block[i] = '0' + n;
    }

	sigset_t mask, oldmask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGUSR1);
	sigprocmask(SIG_BLOCK, &mask, &oldmask);
    /* sigsuspend nas interesuje tylko dla SIGUSR1, wiec nie ma sensu czekac
     * na alarm, tylko sprawdzamy czy juz byl, czyli nie ustawiamy dla niego
     * maski */
    alarm(1);
    while (1) {
        sigsuspend(&oldmask);
        if (SIGUSR1 == lastSignal) {
            // wypisywanie do pliku
            for (int i = blockCount; i < sigCount; i++) {
                int count = n;
                if ((count = bulk_write(out, block, s)) < 0)
                    ERR("write");
                blockCount++;
            }
        }
        if (alarmReceived)
            break;
    }
    sigprocmask(SIG_UNBLOCK, &mask, NULL);

    // zamkniecie pliku
    if(TEMP_FAILURE_RETRY(close(out)))
        ERR("close");
}

void createChildren(int n, int numbers[]) {
    for (int i = 0; i < n; i++) {
        switch (fork()) {
        case 0:
            printf("child %d is born\n", getpid());
            sethandler(sigHandler,SIGUSR1);
            childWork(numbers[i]);
            printf("child %d dies with %d USR1 received and %d blocks written\n", getpid(), sigCount, blockCount);
            exit(EXIT_SUCCESS);
        case -1:
            ERR("fork");
        }
    }
}

void usage(char* pname) {
    fprintf(stderr, "Usage: %s [args]...\n"
                    "Opis zadania\n",
                    pname);
    exit(EXIT_FAILURE);
}

int main(int argc, char** argv) {
    if (argc == 1)
        usage(argv[0]);
    int digits[argc-1];
    for (int i = 1; i < argc; i++) {
        digits[i-1] = atoi(argv[i]);
        if (digits[i-1] > 9 || digits[i-1] < 0)
            usage(argv[0]);
    }

    sethandler(SIG_IGN, SIGUSR1);
    sethandler(sigHandler, SIGALRM);
    createChildren(argc - 1, digits);

    /* licznik wyslanych SIGUSR1 - rodzic blokuje ten sygnal, zatem zmienna
     * sigCount nie bedzie inkrementowana */
    int sigUsr1Sent=0; 
    alarm(1);
    while (1) {
        if (kill(0, SIGUSR1) < 0)
            ERR("kill");
        sigUsr1Sent++;
        if (alarmReceived)
            break;
        struct timespec t = {0, 10*1000*1000};
        nanosleep(&t, NULL);
    }
    while (wait(NULL) > 0);
    printf("parent dies with %d SIGUSR1 sent\n", sigUsr1Sent);
    return EXIT_SUCCESS;
}
