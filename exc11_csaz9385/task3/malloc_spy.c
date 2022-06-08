#include <stdio.h>
#include <stddef.h>
#include <unistd.h>
#include <dlfcn.h>

static void print_number(size_t number) {
	if(number > 9) {
		print_number(number / 10);
	}
	const char digit = '0' + number % 10;
	write(STDOUT_FILENO, &digit, 1);
}

void* malloc(size_t size) {
    write(STDOUT_FILENO, "allocating ", 11);
    print_number(size);
    write(STDOUT_FILENO, " bytes\n", 7);

    void* (*libc_malloc)(size_t) = dlsym(RTLD_NEXT, "malloc");
    return libc_malloc(size);
}
