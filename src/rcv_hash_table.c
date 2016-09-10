#include "rcv_hash_table.h"

#ifdef __cplusplus
extern "C" {
#endif

ht_t rcv_hash;

int rcv_cmp(void *key, void *data) {
	rcv_key_t *rcvKey = (rcv_key_t*)key;
	rcv_hash_data_t *d_st = (rcv_hash_data_t*)data;
	return (rcvKey->table_id == d_st->table_id);
}

void init_rcv_hash(ht_t *rcvhash, uint32_t size, cmpfunc_t cmp, hashfunc_t hash) {
	ht_create(rcvhash, size, cmp, hash);
}

#ifdef __cplusplus
}
#endif

