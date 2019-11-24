//ROBIENIE DZIECI
void create_children(int n) 
{
	pid_t s;
    for(int i=0;i<n;i++)
    {
        switch ((s=fork())) 
        {
			case 0: 
            {
                //sethandler(sig_handler,SIGUSR1);
				//sethandler(sig_handler,SIGUSR2);
				//child_work();
				exit(EXIT_SUCCESS);
            }
			case -1:
            {
                ERR("Fork");
            } break;
		}
        printf("[%d]\n",s);
    }
	
}
//DZIAŁANIE DZIECKA
void child_work() 
{
	//srand(time(NULL)*getpid());	
	//int t=5+rand()%(10-5+1);
	//struct timespec time = {0, t*10000};
    //nanosleep(&time,NULL);
}
//ZABIJANIE DZIECI
void child_killer(int sig) //sigchld_handler
{
	pid_t pid;
    int stat_loc;
	for(;;){
		pid=waitpid(0, &stat_loc, WNOHANG);
        if(pid>0)
        {
            //if ( WIFEXITED(stat_loc) ) exit_status = WEXITSTATUS(stat_loc); //value returned by child                
        }
		if(pid==0) return; //
		if(pid<=0) //BŁĄD
        {
			if(errno==ECHILD) return;
			ERR("waitpid");
		}
	}
}
//DZIAŁANIE RODZICA
/*volatile sig_atomic_t alarm_signal = 0;
void alarm_handler(int sig) {alarm_signal = sig;}*/
void parent_work() 
{
	/*sethandler(alarm_handler,SIGALRM);
	alarm(l*10);
	while(last_signal!=SIGALRM) 
    {

	}*/
	//printf("[PARENT] Terminates \n");
}
//MĄDRE SPANIE
struct timespec time = {0, 0};
while(-1==nanosleep(&time,&time)) 
{
    //printf("Remaining time: %ld sec and %ld nanosec\n",time.tv_sec,time.tv_nsec);
}
//MĄDRE CZEKANIE NA SYGNAŁ OD DZIECI
void parent_work(sigset_t oldmask) 
{
	int count=0;
	while(1)
    {
		last_signal=0;
		while(last_signal!=SIGUSR2)
			sigsuspend(&oldmask);
		count++;
		//printf("[PARENT] received %d SIGUSR2\n", count);
	}
}
//#main
{
    igset_t mask, oldmask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGUSR1);
	sigaddset(&mask, SIGUSR2);
	sigprocmask(SIG_BLOCK, &mask, &oldmask);
    parent_work(oldmask);
	sigprocmask(SIG_UNBLOCK, &mask, NULL);
}
//MĄDRE CZYTANIE Z PLIKU
ssize_t bulk_read(int filedesc, char *buf, size_t count)
{
	ssize_t c;
	ssize_t len=0;
	do{
		c=TEMP_FAILURE_RETRY(read(filedesc,buf,count));
		if(c<0) return c;
		if(c==0) return len; //EOF
		buf+=c;
		len+=c;
		count-=c;
	}while(count>0);
	return len ;
}
//MADRE PISANIE DO PLIKU
ssize_t bulk_write(int filedesc, char *buf, size_t count)
{
	ssize_t c;
	ssize_t len=0;
	do{
		c=TEMP_FAILURE_RETRY(write(filedesc,buf,count));
		if(c<0) return c;
		buf+=c;
		len+=c;
		count-=c;
	}while(count>0);
	return len ;
}
// PRZEPISYWANIE Z JEDNEGO DO DRUGIEGO
{
    int size;//rozmiar bufora
    int b;//ilosc przepisywanych bloków romiazru size
    char *name;//nazwa tworzonego pliku
    int in,out;
	ssize_t count;
	char *buf=malloc(size);
	if(!buf) ERR("malloc");
	if((out=TEMP_FAILURE_RETRY(open(name,O_WRONLY|O_CREAT|O_TRUNC|O_APPEND,0777)))<0)ERR("open");
	if((in=TEMP_FAILURE_RETRY(open("/dev/urandom",O_RDONLY)))<0)ERR("open");
	for(int i=0; i<b;i++){
		if((count=bulk_read(in,buf,size))<0) ERR("read");
		if((count=bulk_write(out,buf,count))<0) ERR("read");
		if(TEMP_FAILURE_RETRY(fprintf(stderr,"Block of %ld bytes transfered. Signals RX:%d\n",count,sig_count))<0)ERR("fprintf");;
	}
	if(TEMP_FAILURE_RETRY(close(in)))ERR("close");
	if(TEMP_FAILURE_RETRY(close(out)))ERR("close");
	free(buf);
}
//PRZYDATNE OPERACJE
int* tab = mallock(sizeof(int) * rozmiar);
free(tab);
char s1[25] = "Ala ma kota";
char s2[25] = "Ala ma psa";
printf("s1: %s\n",s1);
printf("s2: %s\n",s2);
printf("Długości s1: %ld, s2: %ld\n",strlen(s1),strlen(s2));//długość
printf("Prówanie s1 i s2: %d\n",strcmp(s1,s2));//porównywanie
printf("Połączenie s1 i s2: %s\n",strcat(s1,s2));//łączenie
printf("s1: %s\n",s1);
printf("s2: %s\n",s2);
printf("Podciąg s2 w s1: %s\n",strstr(s1,s2));//szukanie wzorca
printf("s1: %s\n",s1);
printf("s2: %s\n",s2);
printf("Kopiowanie s2 do s1: %s\n",strcpy(s1,s2));//kopiowanie
printf("s1: %s\n",s1);
printf("s2: %s\n",s2);