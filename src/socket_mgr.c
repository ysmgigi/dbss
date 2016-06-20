#include "config.h"
#include "sysctl.h"
#include "socket_mgr.h"
#include "epoll_mgr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

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
		printf("Failed to bind, %s, %s, %d\n", __FUNCTION__, __FILE__, __LINE__);
		close(fd);
		exit(EXIT_FAILURE);
	}
	if(listen(fd, DESCRIPTOR_MAX) < 0) {
		printf("Failed to listen, %s, %s, %d\n", __FUNCTION__, __FILE__, __LINE__);
		close(fd);
		exit(EXIT_FAILURE);
	}
	epoll_addfd(efd_server, fd, EPOLLIN);
}

void *connect_thread(void *arg) {
	pthread_detach(pthread_self());
	rte_atomic32_inc(&thread_num);
	int i;
	int remain = client_num;
	struct timespec req = {20, 0};
	while(rte_atomic32_read(&keep_running) && remain > 0) {
		for(i=0; i<client_num; ++i) {
			// not connect to this server yet
			if(clientinfo[i].connected == 0) {
				// create socket
				int fd = socket(AF_INET, SOCK_STREAM, 0);
				if(fd < 0) {
					printf("Failed to create socket for %s:%d, %s, %s, %d\n", clientinfo[i].ip, clientinfo[i].port, __FUNCTION__, __FILE__, __LINE__);
					continue;
				}
				if(fd >= DESCRIPTOR_MAX) {
					printf("Too many connections, max is %d, %s, %s, %d\n", DESCRIPTOR_MAX, __FUNCTION__, __FILE__, __LINE__);
					close(fd);
					//exit(EXIT_FAILURE);
					continue;
				}
				// connect to server
				struct sockaddr_in addr;
				memset(&addr, 0, sizeof addr);
				addr.sin_family = AF_INET;
				addr.sin_port = htons(clientinfo[i].port);
				inet_pton(AF_INET, clientinfo[i].ip, &addr.sin_addr);
				if(connect(fd, (struct sockaddr*)&addr, sizeof addr) < 0) {
					printf("Failed to connect to %s:%d, %s, %s, %d\n", clientinfo[i].ip, clientinfo[i].port, __FUNCTION__, __FILE__, __LINE__);
					close(fd);
					continue;
				}
				// make a flag that this server has been connected successfully
				clientinfo[i].connected = 1;
				// add to epoll control
				Socket *sp = &socketMgr[fd];
				pthread_spin_lock(&sp->lock);
				if(sp->fd == -1) {
					sp->fd = fd;
					sp->port = clientInfoMgr.clients[i].port;
					sp->type = TYPE_SERVER;
					strncpy(sp->ip, clientInfoMgr.clients[i].ip, INET_ADDRSTRLEN);
					--remain;
					EpollAddFd(epollMgr->efd, fd, EPOLLOUT | EPOLLRDHUP);
				}
				/*else { // impossible here
					if(sp->fd == fd) {
						printf("File descriptor %d has been added, %s, %s, %d\n", fd, __FUNCTION__, __FILE__, __LINE__);
						close(fd);
					}
					else {
						printf("File descriptor %d conflict with the old one %d, %s, %s, %d\n", fd, sp->fd, __FUNCTION__, __FILE__, __LINE__);
						close(fd);
					}
				}*/
				pthread_spin_unlock(&sp->lock);
			}
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
	if(pthread_create(&tid, NULL, connect_thread, NULL) != 0) {
		printf("Failed to create connect_thread, %s, %s, %d\n", __FUNCTION__, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
}

#ifdef __cplusplus
}
#endif

