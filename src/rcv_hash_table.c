#include "rcv_hash_table.h"

#ifdef __cplusplus
extern "C" {
#endif

ht_t rcv_hash;

int rcv_cmp(void *key, void *data) {
	rcv_key_t *rcv_key = (rcv_key_t*)key;
	rcv_hash_data_t *rhd = (rcv_hash_data_t*)data;
	return (rcv_key->table_id == rhd->key.table_id);
}

void init_rcv_hash(ht_t *rcvhash, uint32_t size, cmpfunc_t cmp, hashfunc_t hash) {
	ht_create(rcvhash, size, cmp, hash);
}

#ifdef __cplusplus
}
#endif

