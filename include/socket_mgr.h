#ifndef SOCKET_INCLUDE_H
#define SOCKET_INCLUDE_H

#include <pthread.h>
#include <arpa/inet.h>

#define TYPE_CLIENT		1
#define TYPE_SERVER		2
#define TYPE_LISTEN		3
#define TYPE_SIGNAL		4

// socket socketection
typedef struct {
	uint8_t type;	// 1-Client  2-Server 3-Listen
	uint16_t port;
	int32_t fd;
	uint32_t ip;	// network order byte
}sock_info;

extern sock_info *sockinfo;

void init_server();
void init_client();
void init_sock_mgr();

#endif

