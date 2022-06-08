FEATURE_FLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700
DEFINES = 
CFLAGS = -Wall -Wextra -Wpedantic -pthread -O3 $(FEATURE_FLAGS) $(DEFINES)
TARGETS = task1 task1_pool

.PHONY: all
all: $(TARGETS)

.PHONY: clean
clean:
	$(RM) -f $(TARGETS)

task1: task1.c
	$(CC) $(CFLAGS) -o $@ $^

task1_pool: task1.c thread_pool.h
	$(CC) $(CFLAGS) -DUSE_THREADPOOL -o $@ $^