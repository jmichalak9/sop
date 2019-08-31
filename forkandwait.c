#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/* Basic fork and wait syscalls usage. */
int main() {
    printf("Hello from: %d\n", getpid());
    int x = 42, child = fork();

    if (child < 0 ) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (child == 0 ) {
        // child
        printf("Hello from child: %d\n", getpid());
        x++;
        printf("x = %d\n", x);
    } else {
        // parent
        wait(NULL);
        printf("Hello from parent %d of child %d\n", getpid(), child);
        printf("x = %d\n", x);
    }
    return 0;
}
