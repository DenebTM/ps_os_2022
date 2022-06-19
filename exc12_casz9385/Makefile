CC = gcc
FEATURE_FLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700
CFLAGS = -Wall -Wextra -Wpedantic -pthread -std=c11 $(FEATURE_FLAGS)

TARGETS = restaurant

.PHONY: all
all: clean $(TARGETS)

.PHONY: clean
clean:
	$(RM) -f $(TARGETS)

restaurant: restaurant.c
	$(CC) $(CFLAGS) -o $@ $^ -g
