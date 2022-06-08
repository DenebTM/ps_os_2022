DEFINES = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700
CFLAGS = -Wall -Wextra -Wpedantic -lrt -pthread $(DEFINES)

.PHONY: all
all: task2

.PHONY: clean
clean:
	$(RM) -f task2

task2: task2.c
	$(CC) $(CFLAGS) -o $@ $^
