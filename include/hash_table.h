#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <pthread.h>
#include "rte_atomic.h"

typedef struct {
	pthread_spinlock_t lock;
	void *data;
}slot_t;

typedef int (*cmpfunc_t)(void *key1, void *key2);
typedef uint32_t (*hashfunc_t)(const char *key, uint32_t len);

typedef struct {
	rte_atomic32_t size;	// size of hash table
	rte_atomic32_t nel;		// number of elements in hash table
	cmpfunc_t cmp;			// function for key compare
	hashfunc_t hash;		// function for hash
	slot_t *slots;
}ht_t;

void ht_create(ht_t *ht, uint32_t size, cmpfunc_t cmp, hashfunc_t hash);
void ht_destroy(ht_t *ht);
int32_t ht_find(ht_t *ht,  void *key,  int len);
int32_t ht_insert(ht_t *ht,  void *key,  int len,  void *e);

#endif

