CC = gcc
FEATURE_FLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700
CFLAGS = -Wall -Wextra -Wpedantic -pthread -std=c11 $(FEATURE_FLAGS)

TASKNUM := $(shell basename $$(pwd))
TARGETS = $(TASKNUM)_test $(TASKNUM)_benchmark
SOURCES = $(wildcard *.c) ../allocator_tests.c ../membench.c

.PHONY: all
all: clean $(TARGETS)

.PHONY: clean
clean:
	$(RM) -f $(TARGETS)

$(TASKNUM)_test: $(SOURCES)
	$(CC) $(CFLAGS) -DTEST -o $@ $^ -g

$(TASKNUM)_benchmark: $(SOURCES)
	$(CC) $(CFLAGS) -DBENCHMARK -o $@ $^ -O3
