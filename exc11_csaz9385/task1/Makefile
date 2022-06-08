CC = gcc
FEATURE_FLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700
CFLAGS = -Wall -Wextra -Wpedantic -pthread -std=c11 $(FEATURE_FLAGS)

TARGETS = some_library.so my_program

.PHONY: all
all: clean $(TARGETS)

.PHONY: clean
clean:
	$(RM) -f $(TARGETS)

some_library.so: some_library.c
	$(CC) $(CFLAGS) -shared -fPIC -o $@ $^ -g

my_program: my_program.c
	$(CC) $(CFLAGS) -static -o $@ $^ -g
