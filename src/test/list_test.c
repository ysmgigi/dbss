#include <stdio.h>
#include <stdlib.h>
#include "list.h"

list_t list;

#define LIST_SIZE	10

int main() {
	int i;
	node_t *n = NULL;

	list_create(&list);

	printf("========================================\n");
	for(i=0; i<LIST_SIZE; ++i) {
		n = calloc(1, sizeof(node_t));
		list_push(&list, n);
		printf("%p <- %p -> %p\n", n->prev, n, n->next);
	}
	printf("========================================\n");
	n = NULL;
	while((n = list_pop(&list)) != NULL) {
		printf("%p <- %p -> %p\n", n->prev, n, n->next);
		free(n);
	}

	list_destroy(&list);

	return 0;
}

