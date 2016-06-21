#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/signalfd.h>
#include "config.h"
#include "socket_mgr.h"
#include "epoll_mgr.h"
#include "sysctl.h"
#include "general.h"

#ifdef __cplusplus
extern "C" {
#endif

int sigfd = 0;

// signal init
void signal_init() {
	// block signals
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask, SIGQUIT);
	//sigaddset(&mask, SIGKILL);
	sigaddset(&mask, SIGTERM);
	sigprocmask(SIG_BLOCK, &mask, NULL);
	// create signal file descriptor and add to epoll control
	if((sigfd = signalfd(-1, &mask, 0)) < 0) {
#ifdef DEBUG_STDOUT
		printf("Failed to create signalfd, %s, %s, %d\n", __FUNCTION__, __FILE__, __LINE__);
#else
#endif
		exit(EXIT_FAILURE);
	}
	sockinfo[sigfd].fd = sigfd;
	sockinfo[sigfd].type = TYPE_SIGNAL;
	epoll_addfd(efd, sigfd, EPOLLIN);
}

// signal cleanup
void signal_destroy() {
	close(sigfd);
}

// 初始化系统资源
void init() {
	rte_atomic32_init(&thread_num);
	rte_atomic32_set(&keep_running, 1);
	// 初始化连接管理结构
	init_sock_mgr();
	// 初始化epoll管理结构
	epoll_init();
	signal_init();
	// 读取系统配置文件
	read_sys_config();
	read_table_config();
	// 创建服务端socket
	init_server();
	// 建立客户端socket,创建重连线程
	init_client();
}

int main(int argc, char *argv[]) {
	// init system resource
	init();
	// epoll events loop
	epoll_event_loop();
	// signal cleanup
	if(rte_atomic32_read(&keep_running) == 0) {
		signal_destroy();
	}
}

#ifdef __cplusplus
}
#endif

