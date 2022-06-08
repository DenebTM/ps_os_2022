#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERR_ARG 2
#define ERR_NAN 3

int main(int argc, char* argv[]) {
    if (argc != 2) {
        return ERR_ARG;
    }

    char *end = NULL;
    long num = strtol(argv[1], &end, 10) & 1;
    if (end != argv[1] + strlen(argv[1])) {
        return ERR_NAN;
    }

    return num;
}
