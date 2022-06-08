#include <stdio.h>
#include <stdlib.h>

int main() {
    char* env = getenv("MY_ENV_VAR");

    if (env) {
        printf("MY_ENV_VAR is set to '%s'\n", env);
        return EXIT_SUCCESS;
    }
    printf("MY_ENV_VAR is not set\n");
    return EXIT_FAILURE;
}
