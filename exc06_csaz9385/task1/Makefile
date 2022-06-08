FEATURE_FLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700
DEFINES = 
CFLAGS = -Wall -Wextra -Wpedantic -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast -pthread -O3 $(FEATURE_FLAGS) $(DEFINES)

.PHONY: all
all: task1a task1b

.PHONY: clean
clean:
	$(RM) -f task1a task1b

task1a: task1a.c
	$(CC) $(CFLAGS) -o $@ $^

task1b: task1b.c
	$(CC) $(CFLAGS) -o $@ $^
