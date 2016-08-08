#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"
#include "list.h"
#include "rcv_pool.h"
#include <stdlib.h>

list_t rcv_pool;

int init_rcv_pool() {
	unsigned int i;
	int nsz = sizeof(node_t);
	int bsz = sizeof(rcv_buf_t);
	list_create(&rcv_pool);
	node_t *nd = calloc(rcvps, nsz);
	if(nd == NULL) {
		// no memory space
		return -1;
	}
	rcv_buf_t *rbf = calloc(rcvps, bsz);
	if(nd == NULL) {
		// no memory space
		return -1;
	}
	for(i=0; i<rcvps; ++i) {
		nd->data = rbf;
		list_push(&rcv_pool, nd);
		nd += nsz;
		rbf += bsz;
	}
	return 0;
}

#ifdef __cplusplus
}
#endif

