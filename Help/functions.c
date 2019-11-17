//FGETS - pobieranie kolejnych lini
#define MAN_LINE 20
char name[MAX_LINE];
while(fgets(name,MAX_LINE+2,stdin)!=NULL)
{
    ;
}





//GETOPT - pobieranie opcji z wejscia
int option;
while ((option = getopt (argc, argv, "a:b:")) != -1)
    switch (option)
    {
        case 'a':
        {
            ;//sth;
        }
        break; 
        case 'b':
        {
            ;//sth
        }
        break;
        case '?':
        default:
            usage(argv[0]);
    }
if(argc>optind) usage(argv[0]);





//ENVIRON - zmienne srodowiskowe env
extern char **environ;
int index = -1;
while (environ[++index]) 
{
    ;
}
//pobieranie zmiennej
char *env = getenv("NAZWA_ZMIENNEJ"); 
//dodawanie zmiennej
if(putenv("EXAMPLE=exampletext")!=0) 
{
    fprintf(stderr,"putenv failed");
    return EXIT_FAILURE;
}
//wypisanie zmiennej sposrod env
if(system("env|grep RESULT")!=0) 
		return EXIT_FAILURE;





//READDIR - czytanie aktualnego katalogu
DIR *dirp;
struct dirent *dp;
struct stat filestat;
if (NULL == (dirp = opendir("."))) ERR("opendir");
do {
    errno = 0;
    if ((dp = readdir(dirp)) != NULL) 
    {
        if (lstat(dp->d_name, &filestat)) ERR("lstat");
        if (S_ISDIR(filestat.st_mode)) ;//katalog
        else if (S_ISREG(filestat.st_mode)) ;//file
        else if (S_ISLNK(filestat.st_mode)) ;//link
        else ;//other
    }
} while (dp != NULL);

if (errno != 0) ERR("readdir");
if(closedir(dirp)) ERR("closedir");
//https://linux.die.net/man/2/stat





//CHDIR - chodzenie po katalogach programem
char path[MAX_PATH];
if(getcwd(path, MAX_PATH)==NULL) ERR("getcwd");
//...//
if(chdir(argv[i]))ERR("chdir");
// działania w danym katalogu można odwolac sie opendir(".")
if(chdir(path))ERR("chdir");//powrot do rodzimego katalogu






//NFTW - rekurencyjne przechodzenie po drzewku katalogow zaczynajac od path_name
#define MAXFD 20
int search(const char *name, const struct stat *s, int type, struct FTW *f)
{
	switch (type){
		case FTW_DNR:
		case FTW_D: {} break;
		case FTW_F: {} break;
		case FTW_SL: {} break;
		default : {}; 
	}
	return 0;
}
//#main
char* path_name;
if(nftw(path_name,search,MAXFD,FTW_PHYS)==0) ;//sth
else printf("%s: brak dostepu\n",argv[i]);





//STRTOL 
mode_t perms = strtol(param/*optarg for examp*/, (char **)NULL, 8);//usemkowy





//FOPEN - tworzenie pliku jako wyjscia
FILE* make_file(char *name)
{
	FILE* new_file;
	//umask(~perms&0777); //jakas maska perms - ósemkowo
	if((new_file=fopen(name,"w+"))==NULL)ERR("fopen");
	return new_file;
	
}
//#main
FILE* output = stdout;
if(true/*jakies tam warunek dac*/) output = make_file("file_name");
for(int i=0;i<100;i++)
{
    // jakies dzialanie np fprintf(output,"%d\n",i);
    //if(fseek(output,polozenie_kursora,SEEK_SET)) ERR("fseek");
}
if(fclose(output))ERR("fclose");