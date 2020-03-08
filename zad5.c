#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#define ERR(source) (fprintf(stderr,"%s:%d pid=%d\n",__FILE__,__LINE__, getpid()),\
                     perror(source),kill(0,SIGKILL),\
                     exit(EXIT_FAILURE))

void modify(char *s, int n) {
    int x = atoi(s);
    x += n;
    sprintf(s, "%d", x);
}

void childWork(int readfd, int writefd) {
    printf("child pid=%d pipe: %d %d\n", getpid(), readfd, writefd);
    srand(getpid());
    char sent[PIPE_BUF], recvd[PIPE_BUF];
    while (1) {
        if (TEMP_FAILURE_RETRY(read(readfd, recvd, PIPE_BUF)) < 1) ERR("read");
        if (atoi(recvd) == 0) {
            if (TEMP_FAILURE_RETRY(close(readfd))) ERR("close");
            if (TEMP_FAILURE_RETRY(close(writefd))) ERR("close");
            exit(EXIT_SUCCESS);
        }
        strncpy(sent, recvd, PIPE_BUF);
        modify(sent, rand()%21-10);
        printf("pid=%d received %s sent %s\n", getpid(), recvd, sent);
        if (TEMP_FAILURE_RETRY(write(writefd, sent, PIPE_BUF)) < 0) ERR("write");
    }
}

void parentWork(int readfd, int writefd) {
    printf("parent pid=%d pipe: %d %d\n", getpid(), readfd, writefd);
    srand(getpid());
    char sent[PIPE_BUF], recvd[PIPE_BUF];
    sent[0] = '1';
    sent[1] = 0;
    while (1) {
        if (TEMP_FAILURE_RETRY(write(writefd, sent, PIPE_BUF)) < 0) ERR("write");
        if (TEMP_FAILURE_RETRY(read(readfd, recvd, PIPE_BUF)) < 1) ERR("read");
        if (atoi(recvd) == 0) {
            if (TEMP_FAILURE_RETRY(close(readfd))) ERR("close");
            if (TEMP_FAILURE_RETRY(close(writefd))) ERR("close");
            exit(EXIT_SUCCESS);
        }
        strncpy(sent, recvd, PIPE_BUF);
        modify(sent, rand()%21-10);
        printf("pid=%d received %s sent %s\n", getpid(), recvd, sent);
    }

}

void makeChildren() {
    int ab[2], bc[2], ca[2];
    if (pipe(ab))
        ERR("pipe");
    if (pipe(bc))
        ERR("pipe");
    if (pipe(ca))
        ERR("pipe");
    // first child (B)
    switch(fork()) {
    case 0:
        if (TEMP_FAILURE_RETRY(close(ab[1]))) ERR("close");
        if (TEMP_FAILURE_RETRY(close(bc[0]))) ERR("close");
        if (TEMP_FAILURE_RETRY(close(ca[0]))) ERR("close");
        if (TEMP_FAILURE_RETRY(close(ca[1]))) ERR("close");
        childWork(ab[0], bc[1]);
        exit(EXIT_SUCCESS);
    case -1:
        ERR("fork");
    }

    // second child (C)
    switch(fork()) {
    case 0:
        if (TEMP_FAILURE_RETRY(close(ab[0]))) ERR("close");
        if (TEMP_FAILURE_RETRY(close(ab[1]))) ERR("close");
        if (TEMP_FAILURE_RETRY(close(ca[0]))) ERR("close");
        if (TEMP_FAILURE_RETRY(close(bc[1]))) ERR("close");
        childWork(bc[0], ca[1]);
        exit(EXIT_SUCCESS);
    case -1:
        ERR("fork");
    }
    if (TEMP_FAILURE_RETRY(close(ab[0]))) ERR("close");
    if (TEMP_FAILURE_RETRY(close(ca[1]))) ERR("close");
    if (TEMP_FAILURE_RETRY(close(bc[0]))) ERR("close");
    if (TEMP_FAILURE_RETRY(close(bc[1]))) ERR("close");
    parentWork(ca[0], ab[1]);
}

int main() {
    makeChildren();
    return EXIT_SUCCESS;
}
