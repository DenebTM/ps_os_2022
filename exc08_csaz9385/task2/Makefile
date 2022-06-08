FEATURE_FLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700
DEFINES = 
CFLAGS = -Wall -Wextra -Wpedantic -Wno-missing-braces -pthread -O3 $(FEATURE_FLAGS) $(DEFINES)
TARGETS = task2

.PHONY: all
all: $(TARGETS)

.PHONY: clean
clean:
	$(RM) -f $(TARGETS)

task2: task2.c
	$(CC) $(CFLAGS) -o $@ $^