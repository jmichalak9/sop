# sop
W tym repozytorium znajdują się m.in. porady na laby z sopów.
To repo nie zastępuje tutoriala, materiałów z dysku i dokumentacji.
Zachęcam do tworzenia issues i pull requestów.

Pliki:
- `zad*.c` - Rozwiązania zadań z tutoriala.
- `lab*.c` - Rozwiązania zadań z laboratoriów.
- `szablon*.c` - Szablony

## Ogólne porady programistyczne
Można je traktować jako checklistę przed wysłaniem rozwiązania.

- Pisz `if (1 == x)` zamiast `if (x == 1)`
- Do wypisywania błędów używaj makro
```c
#define ERR(source) (perror(source),\
            fprintf(stderr,"%s:%d\n",__FILE__,__LINE__),\
            exit(EXIT_FAILURE))
```
- Zawsze sprawdzaj zwracane wartości funkcji systemowych, np.
```c
if (closedir(dir))
    ERR("closedir")
```
- Niektóre funkcje informują o błędzie, ustawiając zmienną `errno`.
- Kończ `fprintf` i `printf` znakiem nowej linii `\n`.
- Inicjalizuj zmienne. Nie jesteśmy w latach 70. i to dużo nie kosztuje.
- Nie używaj tzw. magic numbers. Zamiast pisać `malloc(4)` pisz `malloc(sizeof(int))`
- Zwalniaj zasoby, jak katalogi, pliki, deskryptory.

## Korzystanie z IDE, mana itp.
### Visual Studio Code
- wciśnięcie `<C-/>` powoduje (za|od)komentowanie zaznaczonych linii.

### vim
ale + coc + ccls

### manpage
- `d` i `u` szybkie poruszanie w dół i w górę
- `/str` wyszukiwanie wzorca `str` w tekście
- `n` i `N` przejście do następnego i poprzedniego wyniku wyszukiwania
- Wywołanie z parametrem `-k arg` przeszukuje manpage dotyczące `arg`. Super przydatne, gdy nie wiemy nawet gdzie szukać.
- Flaga `-a`, gdy nie wiemy, która sekcja.
- Korzystaj ze stron POSIX-owych: `man 3p printf` zamiast `man 3 printf`
- Warto zwrócić uwagę na sekcje `EXAMPLES` i `SEE ALSO`
- Więcej sztuczek, gdy wciśniemy `h` i oczywiście w komendzie `man man` :)

### debugger
Polecam [gdbgui](https://www.gdbgui.com/).

### strace
Pozwala śledzić wywołania funkcji systemowych i sygnały. Więcej w `man strace`.

## pipe/FIFO
- odczyt i zapis na zasadzie first-in-first-out
- łącze jest jednokierunkowe
- FIFO jest widoczne w systemie plików, a pipe nie
- `PIPE_BUF` oznacza maksymalną wielkość bufora łącza
- `fork` oczywiście przekazuje do dziecka łącze utworzone przez rodzica

## pipe
`#include <unistd.h>`
### `int pipe(int fildes[2])`
- tworzy jednokierunkowe nienazwane łącze
- czytamy z `fildes[0]` zwykle funkcją `read`
- piszemy do `fildes[1]` zwykle funkcją `write`
- zwraca `0`, jeśli się powiedzie
- zwraca `-1` i ustawia `errno` w przeciwnym wypadku

### `int close(int fildes)`
- zamyka łącze

## FIFO
`#include <sys/stat.h>`
### `int mkfifo(const char *path, mode_t mode)`
- tworzy FIFO w systemie plików
- `const char *path` to ścieżka
- `mode_t` to prawa dostępu
- zwraca `0`, jeśli się powiedzie
- zwraca `-1` i ustawia `errno` w przeciwnym wypadku

### `int open(const char *path, int oflag, ...)`
- otwiera kolejkę `path` p w trybie `oflag`
- domyślnie operacje otwarcia są **blokujące**
- wywołanie z flagą `O_NONBLOCK` jest nieblokujące

### `int unlink(const char *path)`
- usuwa FIFO z systemu

## Inne źródła
- [glibc](https://www.gnu.org/software/libc/manual/)
- [POSIX](https://pubs.opengroup.org/onlinepubs/9699919799/)
- Michael Kerrisk - "Linux Programming Interface" łatwo dostępna książka, w której wszystko jest szczegółowo opisane, są tam przykładowe programy i ich uruchomienia
