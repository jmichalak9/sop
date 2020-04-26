#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <mqueue.h>

#define ERR(source) (fprintf(stderr,"%s:%d\n",__FILE__,__LINE__),\
                     perror(source),kill(0,SIGKILL),\
                                     exit(EXIT_FAILURE))
#define NAMELEN 64

struct threadarg {
    mqd_t* mqs;
    int i;
    int n;
};

mqd_t* create_queues(int n) {
    mqd_t* mqs = (mqd_t*)malloc(sizeof(mqd_t*) * n);
    if (mqs == NULL)
        ERR("malloc");
    for (int i = 0; i < n; i++) {
        char name[NAMELEN];
        snprintf(name, NAMELEN, "/q%d", i+1);
        struct mq_attr attr;
        attr.mq_msgsize = sizeof(int);
        attr.mq_maxmsg = 10;
        mqs[i] = mq_open(name, O_RDWR | O_CREAT | O_EXCL | O_NONBLOCK, 0600, &attr);
        if (mqs[i] == (mqd_t)-1)
            ERR("mq_open");
        printf("Created %s message queue.\n", name);
    }
    return mqs;
}

void delete_queues(mqd_t* mqs, int n) {
    for (int i = 0; i < n; i++) {
        mq_close(mqs[i]);
        char name[NAMELEN];
        snprintf(name, NAMELEN, "/q%d", i+1);
        mq_unlink(name);
    }
    free(mqs);
}

void thread_func(union sigval sv);

void notify_setup(struct threadarg *ta) {
    struct sigevent sev;
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = thread_func;
    sev.sigev_notify_attributes = NULL;
    sev.sigev_value.sival_ptr = ta;
    if (mq_notify(ta->mqs[ta->i], &sev) == -1)
        ERR("mq_notify");
}
void thread_func(union sigval sv) {
    struct threadarg *ta;
    ta = sv.sival_ptr;
    notify_setup(ta);

    ssize_t numread;
    int buf;
    while ((numread = mq_receive(ta->mqs[ta->i], (char*)&buf, sizeof(int), NULL)) >= 0) {
        printf ("MQ %d read %d\n", ta->i+1, buf);
        if (ta->i+1 == ta->n)
            printf("niepodzielna przez [1..%d]\n", ta->n);
        else if (buf % (ta->i+2) != 0) {
            if (mq_send(ta->mqs[ta->i+1], (char*)&buf, sizeof(int), 0) == -1)
                ERR("mq_send");
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    srand(time(NULL));
    int n = atoi(argv[1]);
    if (argc != 2 || n < 1 || n > 100)
        ERR("invalid args");

    mqd_t* mqs = create_queues(n);
    struct threadarg *ta = malloc(n * sizeof(struct threadarg));
    for (int i = 0; i < n; i++) {
        ta[i].mqs = mqs;
        ta[i].i = i;
        ta[i].n = n;
        notify_setup(&ta[i]);
    }
    char buf[NAMELEN];
    while (scanf("%s", buf) != EOF) {
        int x = atoi(buf);
        if (x < 1)
            continue;
        if (mq_send(mqs[0], (const char*)&x, sizeof(int), 0) == -1)
            ERR("mq_send");
    }
    sleep(2);
    delete_queues(mqs, n);
    free(ta);
    return EXIT_SUCCESS;
}
