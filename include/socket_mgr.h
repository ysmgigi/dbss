#ifndef SOCKET_INCLUDE_H
#define SOCKET_INCLUDE_H

#include <pthread.h>
#include <arpa/inet.h>
#include "general.h"

#define TYPE_CLIENT		1
#define TYPE_SERVER		2
#define TYPE_LISTEN		3

// socket socketection
typedef struct {
	int fd;
	uint16_t port;
	uint8_t type;	// 1-客户端  2-服务端 3-Listen
	pthread_spinlock_t lock;
	char ip[INET_ADDRSTRLEN];
	int32_t len;
	char *data;
}Socket;

// socket管理
extern Socket *socketMgr;

void InitSocket(Socket *s, int len);
void InitSocketMgr();
void DestroySocketMgr();
void InitServerEnd();
void InitClientEnd();

#endif

