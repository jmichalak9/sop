
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#define ERR(source) (perror(source),\
            fprintf(stderr,"%s:%d\n",__FILE__,__LINE__),\
            exit(EXIT_FAILURE))
#define MAX_NUMS 10
#define BUF_SIZE 500

extern char *optarg;
extern int optind;
// globalne, zeby dalo sie odczytac przy funkcji makedir z atexit
char dir[BUF_SIZE];
int a[MAX_NUMS], nums;

void usage(char* pname) {
    fprintf(stderr, "Usage: %s [OPTION]...\n"
                    "Sprawdza, czy ciag jest ciagiem geometrycznym.\n"
                    "  -l LICZBA\t\tkolejny wyraz ciagu, min. 2, max. 10\n"
                    "  -k KATALOG\t\ttworzy KATALOG spelniajacy warunki zadania\n"
                    "  -p \t\ttworzy plik, tylko z flaga -k\n",
                    pname);
    exit(EXIT_FAILURE);
}

int cmp(const void * a, const void * b)
{
    int _a = *(int*)a;
    int _b = *(int*)b;
    if(_a < _b) return -1;
    else if(_a == _b) return 0;
    else return 1;
}

void niegeom() {
    printf("Nie jest geometryczny.\n");
    exit(EXIT_SUCCESS);
}

float sum(int a, float q, int n) {
    float s = a * (1-pow(q, n)) / (1-q);
    return s;
}

void makedir() {
    struct stat st;
    if(stat(dir,&st) == 0) {
        printf("%s is present, removing...\n", dir);
        if (rmdir(dir))
            ERR("rmdir");
    }
    if (mkdir(dir, 0700))
        ERR("mkdir");
    if (chdir(dir))
        ERR("chdir");
    srand(time(NULL));
    for (int i = 0; i < nums; i++) {
        int x = rand() % 4;
        char buf[BUF_SIZE];
        snprintf(buf, BUF_SIZE, "%d", a[i]);
        if (x == 0) {
            FILE* f;
            if((f=fopen(buf,"w+")) == NULL)
                ERR("fopen");
            if (fclose(f))
                ERR("fclose");
        } else {
            if (symlink(".", buf))
                ERR("symlink");
        }
       
    }
}


void makefile() {
    FILE* f;
    if((f=fopen("plik","w+")) == NULL)
            ERR("fopen");
    for (int i = 0; i < nums; i++) {
        fprintf(f, "%d ", a[i]); 
       
    }
    if (fclose(f))
        ERR("fclose");

}

int main(int argc, char** argv) {
	int c = 0, elemsToSum = 100, dirSet = 0;
	while ((c = getopt (argc, argv, "l:k:p")) != -1) {
		switch (c)
		{
			case 'l':
                if (nums >= MAX_NUMS)
                    usage(argv[0]);
				a[nums]=atoi(optarg);
                nums++;
				break;
			case 'k':
                dirSet = 1;
                strncpy(dir, optarg, BUF_SIZE);
                atexit(makedir);
                break;
            case 'p':
                if (dirSet == 0)
                    usage(argv[0]);
                atexit(makefile);
			case '?':
			default:
				usage(argv[0]);
		}
    }
    if(argc>optind) usage(argv[0]);

    if (nums < 2)
        usage(argv[0]);
    qsort(a, nums, sizeof(int), cmp);

    // sprawdzanie czy geometryczny
    if (a[0] == 0)
        niegeom();

    float ratio = a[1] / a[0];
    for (int i = 1; i < nums; i++) {
        if (a[i-1] == 0)
            niegeom();
        if ((float)a[i]/a[i-1] != ratio)
            niegeom();
    }

    printf("Jest geometryczny.\n");

    // DEBUG
    char ch;
    while(scanf("%c", &ch) == 1) {
        char buf[BUF_SIZE];
        switch(ch) {
        case 'z':
            snprintf(buf, BUF_SIZE, "SUM=%f", sum(a[0], ratio, elemsToSum) );
            if(putenv(buf)!=0) {
		        ERR("putenv");
	        }
            break;
        case 'k':
            
            printf("%s\n", getenv("SUM"));
            exit(EXIT_SUCCESS);
            break;
        case 'l':
            scanf("%d", &elemsToSum);
            break;
        }
    }

	return EXIT_SUCCESS;
}