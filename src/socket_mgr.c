#include "config.h"
#include "sysctl.h"
#include "socket_mgr.h"
#include "epoll_mgr.h"
#include "hash_table.h"
#include "svr_hash_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

sock_info *sockinfo = NULL;

void init_sock_mgr() {
	sockinfo = calloc(DESCRIPTOR_MAX, sizeof(sock_info));
	if(sockinfo == NULL) {
#ifdef DEBUG_STDOUT
		printf("Failed to calloc sockinfo, %s, %s, %d\n", __FUNCTION__, __FILE__, __LINE__);
#else
#endif
		exit(EXIT_FAILURE);
	}
}

// dbss作为服务端
// 初始化服务器端socket
void init_server() {
	int fd;
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof addr);
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd < 0) {
		printf("Failed to init server socket, %s, %s, %d\n", __FUNCTION__, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
	set_reuse_addr(fd);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(lsn_port);
	inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr);
	if(bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
#ifdef DEBUG_STDOUT
		printf("Failed to bind, %s, %s, %d\n", __FUNCTION__, __FILE__, __LINE__);
#else
#endif
		close(fd);
		exit(EXIT_FAILURE);
	}
	if(listen(fd, DESCRIPTOR_MAX) < 0) {
#ifdef DEBUG_STDOUT
		printf("Failed to listen, %s, %s, %d\n", __FUNCTION__, __FILE__, __LINE__);
#else
#endif
		close(fd);
		exit(EXIT_FAILURE);
	}
	if(sockinfo[fd].fd == 0) {
		sockinfo[fd].fd = fd;
		sockinfo[fd].type = TYPE_LISTEN;
	}
	epoll_addfd(efd, fd, EPOLLIN);
}

void *reconnect_thread(void *arg) {
	int i;
	pthread_detach(pthread_self());
	rte_atomic32_inc(&thread_num);
	char ip[INET_ADDRSTRLEN] = {0};
	struct timespec req = {20, 0};
	struct in_addr addr4 = {0};
	while(rte_atomic32_read(&keep_running)) {
		for(i=0; i<client_num; ++i) {
			slot_t *slot = &svr_hash.slots[i];
			svr_t *svr = (svr_t*)slot->data;
			pthread_spin_lock(&slot->lock);
			if(svr != NULL && svr->connected == 0) {
				// get server ip string from uint32_t
				addr4.s_addr = svr->ip;
				inet_ntop(AF_INET, &addr4, ip, INET_ADDRSTRLEN);
				// create socket
				int fd = socket(AF_INET, SOCK_STREAM, 0);
				if(fd < 0) {
#ifdef DEBUG_STDOUT
					printf("Failed to create socket for %s:%d, %s, %s, %d\n", ip, svr->port, __FUNCTION__, __FILE__, __LINE__);
#else
#endif
					pthread_spin_unlock(&slot->lock);
					continue;
				}
				if(fd >= DESCRIPTOR_MAX) {
#ifdef DEBUG_STDOUT
					printf("Too many connections %d/%d, %s, %s, %d\n", fd, DESCRIPTOR_MAX, __FUNCTION__, __FILE__, __LINE__);
#else
#endif
					close(fd);
					pthread_spin_unlock(&slot->lock);
					exit(EXIT_FAILURE);
				}
				// connect to server
				struct sockaddr_in addr;
				memset(&addr, 0, sizeof addr);
				addr.sin_family = AF_INET;
				addr.sin_port = htons(svr->port);
				addr.sin_addr.s_addr = svr->ip;
				if(connect(fd, (struct sockaddr*)&addr, sizeof addr) < 0) {
					if(errno != EINPROGRESS) {
#ifdef DEBUG_STDOUT
						printf("Failed to connect to %s:%d, %s, %s, %d\n", ip, svr->port, __FUNCTION__, __FILE__, __LINE__);
#endif
						close(fd);
						pthread_spin_unlock(&slot->lock);
						continue;
					}
				}
				svr->connected = 1;
				// add to fd manager
				sockinfo[fd].fd = fd;
				sockinfo[fd].ip = svr->ip;
				sockinfo[fd].type = TYPE_SERVER;
			}
			pthread_spin_unlock(&slot->lock);
		}
		nanosleep(&req, NULL);
	}
	rte_atomic32_dec(&thread_num);
	return NULL;
}

// dbss作为客户端
// 初始化客户端socket
// 开启一个线程去连接服务端:若连上，则将描述符添加到epoll控制
void init_client() {
	pthread_t tid;
	if(pthread_create(&tid, NULL, reconnect_thread, NULL) != 0) {
#ifdef DEBUG_STDOUT
		printf("Failed to create connect_thread, %s, %s, %d\n", __FUNCTION__, __FILE__, __LINE__);
#else
#endif
		exit(EXIT_FAILURE);
	}
}

#ifdef __cplusplus
}
#endif

