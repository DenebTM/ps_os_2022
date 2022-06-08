FEATURE_FLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700
DEFINES = 
CFLAGS = -Wall -Wextra -Wpedantic -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast -pthread -O3 $(FEATURE_FLAGS) $(DEFINES)

.PHONY: all
all: task2

.PHONY: clean
clean:
	$(RM) -f task2

task2: task2.c
	$(CC) $(CFLAGS) -o $@ $^
