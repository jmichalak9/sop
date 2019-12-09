/* Rozwiazanie zadania 3 z tutoriala.
 *
 */

#include <stdlib.h>
#include <stddef.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <signal.h>

#define ERR(source) (perror(source),\
		     fprintf(stderr,"%s:%d\n",__FILE__,__LINE__),\
		     exit(EXIT_FAILURE))

typedef unsigned int UINT;
typedef struct args {
	pthread_t tid;
	UINT seed;
    pthread_mutex_t *mxChecked;
    int Checked;
    int* Counter;
    pthread_mutex_t *mxCounter;
    pthread_mutex_t *mxL;
    int *L;
    int* Quit;
    pthread_mutex_t *mxQuit;
} args_t;

typedef struct argsSignalHandler {
        pthread_t tid;
        sigset_t *pMask;
        int *Quit;
        pthread_mutex_t *mxQuit;
} argsSignalHandler_t;

void* thread_work(void *voidPtr) {
    args_t* args = voidPtr;
    int M = rand_r(&args->seed) % 99 + 2;
    while(1)
    {
        pthread_mutex_lock(args->mxChecked);
        if(args->Checked == 0)
        {
            (*args->Counter)++;
            args->Checked = 1;
        }
        else
        {
            pthread_mutex_unlock(args->mxChecked);
            continue;
        }
        pthread_mutex_unlock(args->mxChecked);
        
        pthread_mutex_lock(args->mxL);
        if(*(args->L) % M == 0) printf("L[%d] jest podzielne przez M[%d]\n",*args->L,M);
        pthread_mutex_unlock(args->mxL);

        pthread_mutex_lock(args->mxQuit);
        if(*(args->Quit) == 1)
        {
            pthread_mutex_unlock(args->mxQuit);
            break;
        }
        pthread_mutex_unlock(args->mxQuit);
    }
	return NULL;
}

void* signal_handling(void* voidArgs) {
        argsSignalHandler_t* args = voidArgs;
        int signo;
        for (;;) {
                if(sigwait(args->pMask, &signo)) ERR("sigwait failed.");
                switch (signo) {
                        case SIGINT:
                                pthread_mutex_lock(args->mxQuit);
                                *args->Quit = 1; 
                                pthread_mutex_unlock(args->mxQuit);
                                return NULL;
                        default:
                                printf("unexpected signal %d\n", signo);
                                exit(1);
                }
        }
        return NULL;
}

int main(int argc, char** argv) {
    int n = atoi(argv[1]);
    int L = 1;
    int Counter = 0;
    int Quit = 0;

    sigset_t oldMask, newMask;
    sigemptyset(&newMask);
    sigaddset(&newMask, SIGINT);
    if (pthread_sigmask(SIG_BLOCK, &newMask, &oldMask)) ERR("SIG_BLOCK error");

    args_t* attr = (args_t*) malloc(n * sizeof(args_t));
    argsSignalHandler_t handler_args;

    pthread_mutex_t mxChecked = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t mxL = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t mxCounter = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t mxQuit = PTHREAD_MUTEX_INITIALIZER;
    
    handler_args.Quit=&Quit;
    handler_args.mxQuit=&mxQuit;
    handler_args.pMask = &newMask;
    if(pthread_create(&handler_args.tid, NULL, signal_handling, &handler_args))ERR("Couldn't create signal handling thread!");


    srand(time(NULL));
    for(int i=0;i<n;i++)
    {
        attr[i].mxQuit = &mxQuit;
        attr[i].Quit = &Quit;
        attr[i].mxCounter = &mxCounter;
        attr[i].Counter = &Counter;
        attr[i].mxL = &mxL;
        attr[i].Checked = 0;
        attr[i].seed = rand();
        attr[i].L = &L;
        attr[i].mxChecked = &mxChecked;
    }

    for(int i=0;i<n;i++)
    {
        int err = pthread_create(&(attr[i].tid), NULL, thread_work,&attr[i]);
        if (err != 0) ERR("Couldn't create thread");
    }

    struct timespec tk = {0,0.1 * 1000000000};
    while(1)
    {
        pthread_mutex_lock(&mxChecked);
        if(Counter != n)
        {
            pthread_mutex_unlock(&mxChecked);
            continue;
        }
        else
        {
            Counter =0;
        }
        pthread_mutex_unlock(&mxChecked);
        
        nanosleep(&tk,NULL);
        pthread_mutex_lock(&mxL);
        L++;
        pthread_mutex_unlock(&mxL);

        pthread_mutex_lock(&mxChecked);
        for(int i=0;i<n;i++)
        {
            attr[i].Checked = 0;
        }
        pthread_mutex_unlock(&mxChecked);
        
        pthread_mutex_lock(&mxQuit);
        if(Quit == 1)
        {
            pthread_mutex_unlock(&mxQuit);
            break;
        }
        pthread_mutex_unlock(&mxQuit);
        
    }

    if(pthread_join(handler_args.tid, NULL)) ERR("Can't join with 'signal handling' thread");


    for (int i = 0; i < n; i++)
    {
        int err = pthread_join(attr[i].tid, NULL);
        if (err != 0) ERR("Can't join with a thread");
    }

    free(attr);
    if (pthread_sigmask(SIG_UNBLOCK, &newMask, &oldMask)) ERR("SIG_BLOCK error");
    return EXIT_SUCCESS;
}


