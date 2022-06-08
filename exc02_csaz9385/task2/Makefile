CFLAGS = -std=c11 -Wall -Wextra -Wpedantic

.PHONY: all
all: signal_test

.PHONY: clean
clean:
	$(RM) -f signal_test

signal_test: signal_test.c
	$(CC) $(CFLAGS) -o $@ $^
