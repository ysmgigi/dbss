#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "rte_atomic.h"

typedef struct {
	pthread_spinlock_t lock;
	void *data;
}slot_t;

typedef struct {
	rte_atomic32_t size;					// size of hash table
	rte_atomic32_t nel;						// number of elements in hash table
	int (*key_cmp)(void *key1, void *key2); // key compare function
	slot_t *slots;
}ht_t;

void ht_create(ht_t *ht, uint32_t size);
void ht_destroy(ht_t *ht);
int32_t ht_find(const char *key, int len);
int32_t ht_insert(const char *key, int len, void *e);

#endif

