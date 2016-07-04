#include "hash_table.h"
#include <stdlib.h>

#ifdef DEBUG_STDOUT
#include <stdio.h>
#endif

// 
void ht_create(ht_t *ht,  uint32_t size,  cmpfunc_t cmp, hashfunc_t hash) {
	uint32_t i = 0;
	if(cmp == NULL || hash == NULL) {
#ifdef DEBUG_STDOUT
		printf("Key compare function or hash function is NULL, %s, %s, %d\n", __FUNCTION__, __FILE__, __LINE__);
#else
#endif
		exit(EXIT_FAILURE);
	}
	rte_atomic32_set(&ht->size, size);
	rte_atomic32_init(&ht->nel);
	ht->cmp = cmp;
	ht->hash = hash;
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
	ht->cmp = NULL;
	ht->hash = NULL;
}

// 
int32_t ht_find(ht_t *ht, void *key, int len) {
	uint32_t hv = ht->hash(key, len);
	uint32_t sz = rte_atomic32_read(&ht->size);
	int32_t pos = hv % sz;
	int32_t ori_pos = pos;
	slot_t *slot = &ht->slots[pos];
	while(1) {
		pthread_spin_lock(&slot->lock);
		if(slot->data == NULL) {
			pthread_spin_unlock(&slot->lock);
			return -1;
		}
		else if(ht->cmp(key, slot->data)) {
			pthread_spin_unlock(&slot->lock);
			return pos;
		}
		pthread_spin_unlock(&slot->lock);
		pos = (pos + 1) % sz;
		if(pos == ori_pos)
			return -1;
	}
}

// 
int32_t ht_insert(ht_t *ht, void *key, int len, void *e) {
	uint32_t hv = ht->hash(key, len);
	uint32_t sz = rte_atomic32_read(&ht->size);
	int32_t pos = hv % sz;
	int32_t ori_pos = pos;
	slot_t *slot = &ht->slots[pos];
	while(1) {
		pthread_spin_lock(&slot->lock);
		if(slot->data == NULL) {
			slot->data = e;
			pthread_spin_unlock(&slot->lock);
			return pos;
		}
		pthread_spin_unlock(&slot->lock);
		pos = (pos + 1) % sz;
		if(pos == ori_pos)
			return -1;
	}
}

