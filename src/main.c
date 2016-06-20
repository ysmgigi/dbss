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

void init_signal() {
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
		printf("Failed to create signalfd, %s, %s, %d\n", __FUNCTION__, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
	epoll_addfd(efd, sigfd, EPOLLIN);
}

int main(int argc, char *argv[]) {
	rte_atomic32_init(&thread_num);
	rte_atomic32_set(&keep_running, 1);
	// 初始化epoll管理结构
	epoll_init();
	init_signal();
	// 读取系统配置文件
	read_sys_config();
	read_table_config();
	// 初始化socket管理结构,创建服务端socket,建立客户端socket连接
	init_server();
	init_client();

	epoll_event_loop();

	// close signal fd
	if(rte_atomic32_read(&keep_running) == 0) {
		close(sigfd);
	}
}

#ifdef __cplusplus
}
#endif

