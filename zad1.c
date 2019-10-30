#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysmacros.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#define ERR(source) (perror(source),\
            fprintf(stderr,"%s:%d\n",__FILE__,__LINE__),\
            exit(EXIT_FAILURE))
#define MAX_PATH 1000

extern char *optarg;
extern int opterr, optind, optopt;

void usage(char* pname) {
    fprintf(stderr, "Usage: %s [OPTION]...\n"
                    "Listuje pliki i ich rozmiary w podanych katalogach.\n"
                    "  -o PLIK\t\tzapisuje wyjscie programu w PLIK, max. 1\n"
                    "  -p KATALOG\t\tkatalog do wylistowania\n",
                    pname);
    exit(EXIT_FAILURE);
}

void scanDir(char* dirName, FILE *out) {
    DIR *dir = opendir(dirName);
    if (dir == NULL)
        ERR("opendir");
    struct dirent *entry;
    struct stat entryStat;
    fprintf(out, "SCIEZKA:\n%s\nLISTA PLIKOW:\n", dirName);
    do {
        errno = 0;
        if ((entry = readdir(dir)) != NULL) {
            char fromCur[MAX_PATH];
            snprintf(fromCur, MAX_PATH-1, "%s/%s", dirName, entry->d_name);
            if (lstat(fromCur, &entryStat))
                ERR("stat");
            if (S_ISREG(entryStat.st_mode))
                fprintf(out, "%s %ld\n", entry->d_name, entryStat.st_size);
        }
    } while (entry != NULL);
    if (errno)
        ERR("readdir");
    if (closedir(dir))
        ERR("closedir");
}

int main(int argc, char** argv) {
    char c;
    FILE *out = stdout;
    // pierwsza petla do sprawdzenia poprawnosci argumentow
    while ((c = getopt(argc, argv, "p:o:")) != -1) {
        switch (c) {
        case 'o':
            if (out != stdout)
                usage(argv[0]);
            if ((out = fopen(optarg, "w")) == NULL)
                ERR("fopen");
            break;
        case 'p':
            break;
        case '?':
        default:
            usage(argv[0]);
        }
    }
    if (argc > optind) {
        usage(argv[0]);
    }
    // reset indeksu, zeby przejsc po argumentach jeszcze raz
    optind = 0;
    // druga petla do wlasciwego wykonania programu
    while ((c = getopt(argc, argv, "p:o:")) != -1) {
        switch (c) {
        case 'o':
            break;
        case 'p':
            scanDir(optarg, out);
            break;
        case '?':
        default:
            usage(argv[0]);
        }
    }
    if (fclose(out))
        ERR("fclose");
    if (argc > optind) {
        usage(argv[0]);
    }
	return EXIT_SUCCESS;
}
