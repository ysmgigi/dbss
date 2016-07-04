#include "murmur_hash.h"
#include "hash_table.h"
#include "svr_hash_table.h"
#include "config.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

ht_t svr_hash;

int svr_cmp(void *key1, void *data) {
	svr_key *k1 = (svr_key*)key1;
	svr_t *d_st = (svr_t*)data;
	return (k1->ip == d_st->ip && k1->port == d_st->port);
}

void init_svr_hash() {
	svr_key key;
	svr_t *svr = NULL;
	int32_t i, pos;
	struct in_addr addr;
	memset(&key, 0, sizeof key);
	ht_create(&svr_hash, client_num, &svr_cmp, &murmur);
	for(i=0; i<client_num; ++i) {
		memset(&addr, 0, sizeof addr);
		inet_pton(AF_INET, clientinfo[i].ip, &addr);
		svr = calloc(1, sizeof(svr_t));
		if(svr == NULL) {
#ifdef DEBUG_STDOUT
			printf("Failed to calloc for svr_t %s, %s, %d\n", __FUNCTION__, __FILE__, __LINE__);
#else
#endif
			exit(EXIT_FAILURE);
		}
		svr->ip = addr.s_addr;
		svr->port = clientinfo[i].port;
		svr->connected = 0;
		pos = ht_insert(&svr_hash, &key, sizeof key, svr);
		if(pos < 0) {
#ifdef DEBUG_STDOUT
			printf("Failed insert into svr_hash %s, %s, %d\n", __FUNCTION__, __FILE__, __LINE__);
#else
#endif
			free(svr);
			exit(EXIT_FAILURE);
		}
		else {
#ifdef DEBUG_STDOUT
			printf("svr[%d]: ipos=%d, p=%s, port=%d\n", i, pos, clientinfo[i].ip, clientinfo[i].port);
#endif
		}
	}
}

