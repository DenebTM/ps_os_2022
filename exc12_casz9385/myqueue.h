// from PS exercise 7, task 2

#ifndef MYQUEUE_H_
#define MYQUEUE_H_

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/queue.h>

#include "common.h"

typedef struct myqueue_entry {
	order_t* value;
	STAILQ_ENTRY(myqueue_entry) entries;
} myqueue_entry;

STAILQ_HEAD(myqueue_head, myqueue_entry);

typedef struct myqueue_head myqueue;

static void myqueue_init(myqueue* q) {
	STAILQ_INIT(q);
}

static bool myqueue_is_empty(myqueue* q) {
	return STAILQ_EMPTY(q);
}

static void myqueue_push(myqueue* q, order_t* value) {
	struct myqueue_entry* entry = (myqueue_entry*)malloc(sizeof(struct myqueue_entry));
	entry->value = value;
	STAILQ_INSERT_TAIL(q, entry, entries);
}

static order_t* myqueue_pop(myqueue* q) {
	assert(!myqueue_is_empty(q));
	struct myqueue_entry* entry = STAILQ_FIRST(q);
	order_t* value = entry->value;
	STAILQ_REMOVE_HEAD(q, entries);
	free(entry);
	return value;
}

#endif
