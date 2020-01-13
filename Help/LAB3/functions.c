typedef unsigned int UINT;
typedef struct threadBox {
    pthread_t tid;
	UINT seed;
    int val
	pthread_mutex_t mxval;
} threadBox_t;

threadBox_t box1;


void* thread_work(void *argsptr) {
	threadBox_t *args = argsptr;
	double* result;
	if(NULL==(result=malloc(sizeof(double)))) ERR("malloc");;
    /**/
    pthread_mutex_lock(&args.mxval);
    val = (int) rand_r(args.seed) ;
    pthread_mutex_unlock(&args.mxval);
    /**/
	return result;
}

void main_thread(int thread_nr)
{
    srand(time(NULL));
    threadBox_t* thread_box = (threadBox_t*) malloc(sizeof(threadBox_t) * thread_nr);
	if (thread_box == NULL) ERR("Malloc error for estimation arguments!");
    for(int i=0;i<thread_nr;i++)
    {
        thread_box[i].seed = (UINT) rand();
        thread_box[i].val = 0;
        thread_box[i].mxval = PTHREAD_MUTEX_INITIALIZER;
    }
    /*1#OPCJA*/
    pthread_attr_t threadAttr;
	if(pthread_attr_init(&threadAttr)) ERR("Couldn't create pthread_attr_t");
	if(pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED)) ERR("Couldn't setdetachsatate on pthread_attr_t");
    for(int i=0;i<thread_nr;i++)
    {
        if((err = pthread_create(&(thread_box[i].tid), &threadAttr, thread_work, &thread_box[i])) != 0) ERR("Couldn't create thread");
    }
    pthread_attr_destroy(&threadAttr);
    /*2#OPCJA*/
    for(int i=0;i<thread_nr;i++)
    {
        if((err = pthread_create(&(thread_box[i].tid), NULL, thread_work, &thread_box[i])) != 0) ERR("Couldn't create thread");
    }
    double *subresult;
    for (int i = 0; i < thread_nr; i++) {
		int err = pthread_join(thread_box[i].tid, (void*)&subresult);
		if (err != 0) ERR("Can't join with a thread");
		if(NULL!=subresult){
			//*subresult;
			free(subresult);
		}
	}
    /**/
    free(estimations);
}

pthread_mutex_t mxValue[100];
if(pthread_mutex_init(&mxValue[i], NULL)) ERR("Couldn't initialize mutex!");