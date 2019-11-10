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
	fprintf(stderr,"Usage: %s [OPTION]...\n"
                    "Sprawdza, czy ciag jest ciagiem arytmetycznym.\n"
                    "  -l LICZBA\t\tkolejny wyraz ciagu\n"
                    "  -k KATALOG\t\ttworzy KATALOG spelniajacy warunki zadania\n"
                    "  -p \t\ttworzy plik, tylko z flaga -k\n",
                    pname);
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
    DIR *dirp;
    if(NULL !=(dirp = opendir(name)))
    {
        if(nftw(name, unlink_cb, 64, FTW_DEPTH | FTW_PHYS)!=0) ERR("nftw");
        if(closedir(dirp)) ERR("closedir");
    }

    if(mkdir(name,0700)) ERR("mkdir");
    if(chdir(name))ERR("chdir");

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
    if(chdir(".."))ERR("chdir");
    
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

    int r = tab[1]-tab[0];
    for(int i=2;i<ind;i++)
    {
        if(tab[i]-tab[i-1]!=r)
            return EXIT_FAILURE;
    }
    printf("Ciag jest arytmetyczny!\n");

    if(strcmp(name,"")!=0)
    {
        srand(time(NULL));
        makedir(name,tab,ind);
    }

    if(optp==1 && strcmp(name,"")!=0)
    {
        mfile(name,tab,ind);
    }
    else
    {
        usage(argv[0]);
    }

    char a;
    if(putenv("SUM=NieZapisanoSumy")!=0) ERR("putenv");
    while(scanf("%c",&a) == 1)
    {
        int sum=0;
        char buff[MAX_LINE];
        switch(a)
        {
            case 's':
                if(ind<num) usage(argv[0]);
                sum = (tab[0]+tab[num-1])*(num)/2;
                snprintf(buff,MAX_LINE,"SUM=%d",sum);
                if(putenv(buff)!=0) ERR("putenv");
                break;
            case 'x':
                printf("%s\n",getenv("SUM"));
                break;
            case 'n':
                scanf("%d",&num);
                break;
        }
    }

    return EXIT_SUCCESS;
}