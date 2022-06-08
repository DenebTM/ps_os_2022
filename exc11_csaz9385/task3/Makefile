CC = gcc
FEATURE_FLAGS = -D_GNU_SOURCE -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700
CFLAGS = -Wall -Wextra -pthread -std=c11 $(FEATURE_FLAGS)

TARGETS = malloc_spy.so

.PHONY: all
all: clean $(TARGETS)

.PHONY: clean
clean:
	$(RM) -f $(TARGETS)

malloc_spy.so: malloc_spy.c
	$(CC) $(CFLAGS) -shared -ldl -fPIC -o $@ $^ -g
	chmod -x $@
