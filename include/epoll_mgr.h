#ifndef EPOLL_INCLUDE_H
#define EPOLL_INCLUDE_H

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include "general.h"

extern int efd;
extern struct epoll_event *enents;

void set_reuse_addr(int fd);
int set_nonblock(int fd);
void epoll_addfd(int epollfd, int fd, int ev);
void epoll_delfd(int epollfd, int fd);

void epoll_init();
void epoll_destroy();

void epoll_event_loop();

#endif

