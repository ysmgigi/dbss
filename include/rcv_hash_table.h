#ifndef RECEIVE_HASH_TABLE_H
#define RECEIVE_HASH_TABLE_H

#include "hash_table.h"
#include "table_info.h"
#include "list.h"

typedef struct rcv_key_s {
	uint16_t table_id;
}rcv_key_t;

typedef struct rcv_hash_data_s {
	uint16_t table_id;
	table_info_t table_info;
	list_t queue;
}rcv_hash_data_t;

extern ht_t rcv_hash;

void init_rcv_hash(ht_t *rcvhash, uint32_t size, cmpfunc_t cmp, hashfunc_t hash);

#endif

