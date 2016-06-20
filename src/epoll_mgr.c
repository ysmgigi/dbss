#include <stdio.h>
#include <stdlib.h>
#include "epoll_mgr.h"

#ifdef __cplusplus
extern "C" {
#endif

int efd = -1;
struct epoll_event *enents = NULL;

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

void epoll_event_loop() {
	while(rte_atomic32_read(&keep_running)) {
	}
}

#ifdef __cplusplus
}
#endif

