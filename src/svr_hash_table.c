#include "murmur_hash.h"
#include "hash_table.h"
#include "svr_hash_table.h"

ht_t svr_hash;

int svr_cmp(void *key1, void *data) {
	svr_key *k1 = (svr_key*)key1;
	svr_t *d_st = (svr_t*)data;
	return (k1->ip == d_st->ip && k1->port == d_st->port);
}

void init_svr_hash(uint32_t size) {
	ht_create(&svr_hash, size, &svr_cmp, &murmur);
}

