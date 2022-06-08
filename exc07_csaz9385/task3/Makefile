FEATURE_FLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700
DEFINES = 
CFLAGS = -Wall -Wextra -Wpedantic -pthread -O3 $(FEATURE_FLAGS) $(DEFINES)

.PHONY: all
all: task3

.PHONY: clean
clean:
	$(RM) -f task3

task3: task3.c
	$(CC) $(CFLAGS) -o $@ $^
