#ifndef CONFIG_INCLUDE_H
#define CONFIG_INCLUDE_H

#include <stdint.h>
#include <arpa/inet.h>
#include "general.h"

// 全局变量声明
#ifdef __cplusplus
extern "C" {
#endif

// 数据源
#define DATASOURCE_INVALID	0
#define DATASOURCE_SOCKET	1
#define DATASOURCE_FILE		2

// 数据输出
#define DATADEST_INVALID	0
#define DATADEST_DB			1
#define DATADEST_FILE		2

typedef struct {
	uint16_t port;
	char ip[INET_ADDRSTRLEN];
}client_info;

// 系统配置文件目录
extern char sysconf_dir[BYTE128];

// 表结构配置目录
extern char tblconf_dir[BYTE128];

// 数据源
extern int data_src;

// 数据输出目的
extern int data_dst;

// Oracle 数据库配置
extern char service[BYTE32];
extern char user[BYTE32];
extern char password[BYTE32];
extern int dpbfs;

// socket 接收缓存池节点数
extern uint32_t rcvps;

// 表列缓存池预分配节点数
extern uint32_t tblps;

// socket 监听端口
extern int lsn_port;

// client
extern int client_num;
extern client_info *clientinfo;

// 读系统配置文件
void read_sys_config();

// 读表结构配置文件
void read_table_config();

#ifdef __cplusplus
}
#endif

#endif

