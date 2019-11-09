#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ftw.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>

#define MAX_ELEMS 1000
#define BUFF 20
#define SUM 100
#define MAX_LINE 20
#define ERR(source) (perror(source),\
		     fprintf(stderr,"%s:%d\n",__FILE__,__LINE__),\
		     exit(EXIT_FAILURE))

void usage(char* pname){
	fprintf(stderr,"USAGE:%s name times>0\n",pname);
	exit(EXIT_FAILURE);
}

extern int optind;
extern char* optarg;


int my_cmp (const void * a, const void * b)
{
    int _a = *(int*)a;
    int _b = *(int*)b;
    if(_a < _b) return 1;
    else if(_a == _b) return 0;
    else return -1;
}

int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    if(remove(fpath))ERR("unlink");
    return 0;
}

void makedir(char* name,int* tab,int ind)
{
    char buff[MAX_LINE]="./";
    DIR *dirp;
    strcat(buff,name);
    if(NULL !=(dirp = opendir(buff)))
    {
        if(nftw(buff, unlink_cb, 64, FTW_DEPTH | FTW_PHYS)!=0) ERR("nftw");
        if(closedir(dirp)) ERR("closedir");
    }

    if(mkdir(buff,0700)) ERR("mkdir");
    if(chdir(buff))ERR("chdir");

    FILE* s1;
    for(int i=0;i<ind;i++)
    {
        int rnd = (rand()%5) + 1;
        char tmp[MAX_LINE];
        if(sprintf(tmp, "%d", tab[i])<0) ERR("sprintf");
        if(rnd <= 2)
        {
            if((s1=fopen(tmp,"w+"))==NULL)ERR("fopen");
            if(fclose(s1))ERR("fclose");
        }
        else
        {
            if(symlink(".",tmp)!=0) ERR("symlink");
        }
    }
    
}

void mfile(char *name,int* tab,int ind){
        FILE* s1;
        strcat(name,"f");
        if((s1=fopen(name,"w+"))==NULL)ERR("fopen");
        for(int i=0;i<ind;i++)
        {
            fprintf(s1,"%d ",tab[i]);
        }
        if(fclose(s1))ERR("fclose");
        name[strlen(name)-1] = '\0';
}

int main(int argc, char** argv)
{
    int ind=0;
    int tab[MAX_ELEMS];
    int c;
    int x;
    int num = SUM;
    int optp = 0;
    char name[BUFF] = "";
	while ((c = getopt (argc, argv, "l:k:p")) != -1)
		switch (c){
			case 'l':
				x=strtol(optarg, (char **)NULL, 8);
                if (x==-1) usage(argv[0]);
                tab[ind++]=x;
				break;
            case 'k':
                strcpy(name,optarg);
                break;
            case 'p':
                optp=1;
                break;
			case '?':
			default: usage(argv[0]);
		}
	if(argc>optind) usage(argv[0]);

    qsort(tab,ind,sizeof(int),my_cmp);

    if(optp==1 && strcmp(name,"")!=0)
    {
        mfile(name,tab,ind);
    }
    else
    {
        usage(argv[0]);
    }

    int r = tab[1]-tab[0];
    for(int i=2;i<ind;i++)
    {
        if(tab[i]-tab[i-1]!=r)
            return EXIT_FAILURE;
    }
    printf("Ciag jest arytmetyczny!\n");

    char buff[MAX_LINE+2];
    while(fgets(buff,MAX_LINE+2,stdin)!=NULL)
	{
        //printf("%c\n",buff[0]);
        if(buff[0] == 's')
        {
            if(ind<=num)
                usage(argv[0]);
            int sum = (tab[0]+tab[num])*(num+1)/2;
            char tmp[MAX_LINE] ="";
            char tmp2[MAX_LINE]="SUM=";
            if(sprintf(tmp, "%d", sum)<0) ERR("sprintf");
            strcat(tmp2,tmp);
            if(putenv(tmp2)!=0) ERR("putenv");
        }
        if(buff[0] == 'x')
        {
            if(getenv("SUM") == NULL)
                {
                    printf("Nie zapisano sumy\n");
                    break;
                }
            printf("%s\n",getenv("SUM"));
            break;
        }
        if(buff[0] == 'n')
        {
            char tmp[MAX_LINE]="";
            strncpy(tmp,buff+2,MAX_LINE-1);
            int tmp2 = strtol(tmp, (char **)NULL, 8);
            if (tmp2==-1) usage(argv[0]);
            num=tmp2;
            //printf("%d\n",tmp2);
        }
    }
    
    if(strcmp(name,"")!=0)
    {
        //printf("dsadasdas\n");
        srand(time(NULL));
        makedir(name,tab,ind);
    }
/*
    for(int i=0;i<ind;i++)
    {
        printf("%d\n",tab[i]);
    }
*/

    return EXIT_SUCCESS;
}