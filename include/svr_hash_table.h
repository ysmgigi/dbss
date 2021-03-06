#ifndef SERVER_HASH_TABLE_H
#define SERVER_HASH_TABLE_H

#include "list.h"
#include "hash_table.h"
#include <stdint.h>

typedef struct {
	uint16_t port;
	uint32_t ip;
}svr_key;

typedef struct {
	uint16_t connected;
	uint16_t port;
	uint32_t ip;
	list_t queue;
}svr_t;

extern ht_t svr_hash;
int svr_cmp(void *key1,  void *data);
void init_svr_hash();

#endif

