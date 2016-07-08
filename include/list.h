// multi-thread safe link list
#ifndef LIST_INCLUDE_H
#define LIST_INCLUDE_H

#include <pthread.h>
#include "rte_atomic.h"

typedef struct node_s {
	void *data;
	struct node_s *prev;
	struct node_s *next;
}node_t;

typedef struct list_s {
	rte_atomic32_t num;	// number of elements stored in list
	node_t *head;
	node_t *tail;
	pthread_spinlock_t lock;
}list_t;

int list_create(list_t *list);

void list_destroy(list_t *list);

void list_push(list_t *list, node_t *n);

node_t *list_pop(list_t *list);

void list_split(list_t *src, list_t *dst);

#endif

