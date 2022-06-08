DEFINES = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700
CFLAGS = -Wall -Wextra -Wpedantic -lrt $(DEFINES)

.PHONY: all
all: task1

.PHONY: clean
clean:
	$(RM) -f task1

task1: task1.c
	$(CC) $(CFLAGS) -o $@ $^
