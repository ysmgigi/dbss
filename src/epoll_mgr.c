#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/signalfd.h>
#include "general.h"
#include "sysctl.h"
#include "epoll_mgr.h"
#include "rte_atomic.h"
#include "socket_mgr.h"

#ifdef __cplusplus
extern "C" {
#endif

int efd = -1;
struct epoll_event *events = NULL;

// ============ file descriptor & epoll 操作 ===============
//
// 设置地址重用
void set_reuse_addr(int fd) {
	int reuse = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, 1);
}

// 设置非阻塞模式
int set_nonblock(int fd) {
	int oldflag;
	oldflag = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, oldflag | O_NONBLOCK);
	return oldflag;
}

// 添加描述符
void epoll_addfd(int epollfd, int fd, int ev){
	struct epoll_event event;
	event.events = ev;
	event.data.fd = fd;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
	set_nonblock(fd);
}

// 从epoll中删除描述符
void epoll_delfd(int epollfd, int fd) {
	epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
}

// 
void epoll_init() {
	efd = epoll_create(DESCRIPTOR_MAX);
	if(efd < 0) {
#ifdef DEBUG_STDOUT
		printf("Failed to create epoll, %s, %s, %d\n", __FUNCTION__, __FILE__, __LINE__);
#else
#endif
		exit(EXIT_FAILURE);
	}
	events = calloc(DESCRIPTOR_MAX, sizeof(struct epoll_event));
	if(events == NULL) {
#ifdef DEBUG_STDOUT
		printf("Failed to alloc epoll events, %s, %s, %d\n", __FUNCTION__, __FILE__, __LINE__);
#else
#endif
		close(efd);
		efd = -1;
		exit(EXIT_FAILURE);
	}
}

// 销毁epollMgr
void epoll_destroy() {
	if(efd != -1)
		close(efd);
	if(events != NULL)
		free(events);
}

// handle signals
void signal_handle(int fd) {
	struct signalfd_siginfo si;
	struct timespec req = {0, 10000};
	int sl = sizeof si;
	memset(&si, 0, sl);
	if(read(fd, &si, sl) < 0) {
#ifdef DEBUG_STDOUT
		printf("Failed to read sigfd, %s, %s, %d\n", __FUNCTION__, __FILE__, __LINE__);
#else
#endif
		exit(EXIT_FAILURE);
	}
#ifdef DEBUG_STDOUT
		printf("Signal %d happens, will terminate system\n", si.ssi_signo);
#else
#endif
		rte_atomic32_set(&keep_running, 0);
		// wait until all threads  exit, clean up system resource.
		while(rte_atomic32_read(&thread_num) > 0) {
			nanosleep(&req, NULL);
		}
		// cleanup
		// system_clean_up();
}

void accept_client(int lsnfd) {
	int cfd = 0;
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof addr;
	cfd = accept(lsnfd, (struct sockaddr*)&addr, &addrlen);
	if(cfd < 0) {
#ifdef DEBUG_STDOUT
		printf("Failed to accept new client, %s, %s, %d\n", __FUNCTION__, __FILE__, __LINE__);
#else
#endif
		exit(EXIT_FAILURE);
	}
	// too many connections
	if(cfd >= DESCRIPTOR_MAX) {
#ifdef DEBUG_STDOUT
		printf("File exceeded the max %d, %s, %s, %d\n", DESCRIPTOR_MAX, __FUNCTION__, __FILE__, __LINE__);
#else
#endif
		close(cfd);
		return;
	}
	if(sockinfo[cfd].fd == 0) {
		sockinfo[cfd].fd = cfd;
		sockinfo[cfd].type = TYPE_CLIENT;
		epoll_addfd(efd, cfd, EPOLLIN | EPOLLRDHUP);
	}
}

// clean up sockinfo and close client fd
void close_client(int fd) {
	sockinfo[fd].fd = 0;
	sockinfo[fd].type = 0;
	sockinfo[fd].ip = 0;
	sockinfo[fd].port = 0;
	close(fd);
}

// 
void epoll_event_loop() {
	int num = 0;
	int fd = -1;
	uint32_t event = 0;
	while(rte_atomic32_read(&keep_running)) {
		num = epoll_wait(efd, events, DESCRIPTOR_MAX, -1);
		if(num < 0) {
#ifdef DEBUG_STDOUT
			printf("Failed to epoll_wait, %s, %s, %d\n", __FUNCTION__, __FILE__, __LINE__);
#else
#endif
			rte_atomic32_set(&keep_running, 0);
			exit(EXIT_FAILURE);
		}
		int i;
		for(i=0; i<num; ++i) {
			fd = events[i].data.fd;
			event = events[i].events;
			// new client comes, accept it and add to epoll event loop
			if(sockinfo[fd].type == TYPE_LISTEN && (event & EPOLLIN)) {
				accept_client(fd);
			}
			// data from client, read data and put data into receive/send buffer.
			else if(sockinfo[fd].type == TYPE_CLIENT && (event & EPOLLIN)) {
			}
			// client closed, close local socket and clean up sockinfo
			else if(sockinfo[fd].type == TYPE_CLIENT && (event & EPOLLRDHUP)) {
				close_client(fd);
			}
			// server can wirte, get data from its queue and send.
			else if(sockinfo[fd].type == TYPE_SERVER && (event & EPOLLOUT)) {
			}
			// server closed
			else if(sockinfo[fd].type == TYPE_SERVER && (event & EPOLLRDHUP)) {
			}
			// signal occured
			else if(sockinfo[fd].type == TYPE_SIGNAL && (event & EPOLLIN)) {
				signal_handle(fd);
			}
		}
	}
}

#ifdef __cplusplus
}
#endif

