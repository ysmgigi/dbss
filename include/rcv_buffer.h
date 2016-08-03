#ifndef RECEIVE_BUFFER_INCLUDE_H
#define RECEIVE_BUFFER_INCLUDE_H

#define BUFFER_MAX	1024

typedef struct rcv_buf_s {
	uint16_t len;
	char buf[BUFFER_MAX];
}rcv_buf_t;

#endif

