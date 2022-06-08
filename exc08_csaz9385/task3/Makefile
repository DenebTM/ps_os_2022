FEATURE_FLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700
DEFINES = 
CFLAGS = -Wall -Wextra -Wpedantic -Wno-missing-braces -Wno-unused-function -pthread -O3 $(FEATURE_FLAGS) $(DEFINES)
TARGETS = task3

.PHONY: all
all: $(TARGETS)

.PHONY: clean
clean:
	$(RM) -f $(TARGETS)

task3: task3.c thread_pool.h
	$(CC) $(CFLAGS) -o $@ $^