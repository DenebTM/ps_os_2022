FEATURE_FLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700
CFLAGS = -Wall -Wextra -Wpedantic -pthread -std=c11 $(FEATURE_FLAGS)

TASKNUM := $(shell basename $$(pwd))
TARGETS = $(TASKNUM)_free_list_test $(TASKNUM)_free_list_benchmark \
		  $(TASKNUM)_best_fit_test $(TASKNUM)_best_fit_benchmark
COMMON_SRC = main.c ../allocator_tests.c ../membench.c
FREE_LIST_SRC = $(COMMON_SRC) free_list_allocator.c
BEST_FIT_SRC = $(COMMON_SRC) best_fit_allocator.c

.PHONY: all
all: clean $(TARGETS)

.PHONY: clean
clean:
	$(RM) -f $(TARGETS)

$(TASKNUM)_free_list_test: $(FREE_LIST_SRC) 
	$(CC) $(CFLAGS) -DFREE_LIST -DTEST -o $@ $^ -g

$(TASKNUM)_free_list_benchmark: $(FREE_LIST_SRC)
	$(CC) $(CFLAGS) -DFREE_LIST -DBENCHMARK -o $@ $^ -O3

$(TASKNUM)_best_fit_test: $(BEST_FIT_SRC) 
	$(CC) $(CFLAGS) -DBEST_FIT -DTEST -o $@ $^ -g

$(TASKNUM)_best_fit_benchmark: $(BEST_FIT_SRC)
	$(CC) $(CFLAGS) -DBEST_FIT -DBENCHMARK -o $@ $^ -O3

