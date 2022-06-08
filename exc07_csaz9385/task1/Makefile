FEATURE_FLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700
DEFINES = 
CFLAGS = -Wall -Wextra -Wpedantic -Wno-unused-parameter -pthread -O3 $(FEATURE_FLAGS) $(DEFINES)

.PHONY: all
all: task1_mutex task1_atomic

.PHONY: clean
clean:
	$(RM) -f task1_mutex task1_atomic

task1_mutex: task1.c
	$(CC) $(CFLAGS) -o $@ $^

task1_atomic: task1.c
	$(CC) $(CFLAGS) -DUSE_ATOMICS -o $@ $^
