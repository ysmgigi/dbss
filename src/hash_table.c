#include "hash_table.h"
#include <stdlib.h>

// 
void ht_create(ht_t *ht,  uint32_t size,  func_t fnc) {
	int i = 0;
	if(fnc == NULL) {
#ifdef DEBUG_STDOUT
		printf("Key compare function is NULL, %s, %s, %d\n", __FUNCTION__, __FILE__, __LINE__);
#else
#endif
		exit(EXIT_FAILURE);
	}
	rte_atomic32_set(&ht->size, size);
	rte_atomic32_init(&ht->nel);
	ht->fnc = fnc;
	ht->slots = calloc(size, sizeof(slot_t));
	if(ht->slots == NULL) {
#ifdef DEBUG_STDOUT
		printf("Failed to calloc for hash table, %s, %s, %d\n", __FUNCTION__, __FILE__, __LINE__);
#else
#endif
		exit(EXIT_FAILURE);
	}
	for(i=0; i<size; ++i) {
		pthread_spin_init(&ht->slots[i].lock, PTHREAD_PROCESS_PRIVATE);
		ht->slots[i].data = NULL;
	}
}

// 
void ht_destroy(ht_t *ht) {
	int i = 0;
	for(i=0; i<rte_atomic32_read(&ht->size); ++i) {
		pthread_spin_destroy(&ht->slots[i].lock);
	}
	free(ht->slots);
	ht->slots = NULL;
}

// 
int32_t ht_find(void *key, int len) {
}

// 
int32_t ht_insert(void *key, int len, void *e) {
}

