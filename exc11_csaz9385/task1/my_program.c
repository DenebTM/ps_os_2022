#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int x = atoi(argv[1]);
    printf("%d\n", x*x);

    return EXIT_SUCCESS;
}
