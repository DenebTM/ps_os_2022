FEATURE_FLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700
DEFINES = 
CFLAGS = -Wall -Wextra -Wpedantic -Wno-unused-parameter -pthread -O2 $(FEATURE_FLAGS) $(DEFINES)

.PHONY: all
all: task2_pt task2_my

.PHONY: clean
clean:
	$(RM) -f task2_pt task2_my

task2_pt: task2.c
	$(CC) $(CFLAGS) -o $@ $^

task2_my: task2.c
	$(CC) $(CFLAGS) -DUSE_MY_MUTEX -o $@ $^
