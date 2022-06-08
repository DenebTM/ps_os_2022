#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#define CATCH(exit_cond, ...)               \
    do {                                    \
        if(exit_cond) {                     \
            fprintf(stderr, __VA_ARGS__);   \
            return EXIT_FAILURE;            \
        }                                   \
    } while(0)

typedef void (*fptr)(int*);

int main(int argc, const char *argv[]) {
    CATCH(argc < 3, "Usage: %s <number> <plugin 1> [<plugin 2> ...]\n", argv[0]);

    int number = atoi(argv[1]);

    int plugins_count = argc - 2;
    const char** plugin_name = argv + 2;
    while (plugins_count > 0) {
        void* plugin_handle = dlopen(*plugin_name, RTLD_LAZY);
        CATCH(plugin_handle == NULL, "Could not open shared library %s\n", *plugin_name);

        fptr map_int = (fptr)dlsym(plugin_handle, "map_int");
        map_int(&number);
        printf("%s: %d\n", *plugin_name, number);

        dlclose(plugin_handle);
        plugin_name++;
        plugins_count--;
    }
    
    return EXIT_SUCCESS;
}
