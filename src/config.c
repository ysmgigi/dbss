#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "config.h"
#include "general.h"
#include "iniparser.h"

// 配置文件全局变量声明及相关函数
//
#ifdef __cplusplus
extern "C" {
#endif

// 系统配置文件目录
char sysconf_dir[BYTE128] = {0};

// 表结构配置目录
char tblconf_dir[BYTE128] = {0};

// 数据源
int data_src = DATASOURCE_INVALID;

// 数据目的
int data_dst = DATADEST_INVALID;

// Oracle 数据库配置
char service[BYTE32] = {0};
char user[BYTE32] = {0};
char password[BYTE32] = {0};
// 直接路径缓存大小
int dpbfs = 0;

// socket 接收缓存池节点数
uint32_t rcvps = 0;

// 表列缓存池预分配节点数
uint32_t tblps = 0;

// socket 监听端口
int lsn_port = 0;

// 配置文件中客户端信息
int client_num = 0;
client_info *clientinfo = NULL;

// 读取系统配置文件
void read_sys_config() {
	char *home = 0;
	char inifile[BYTE128] = {0};
	dictionary *dic = 0;
	home = getenv("HOME");
	if(home == NULL) {
		printf("Failed to get env HOME,%s, %s, %d\n", __FUNCTION__, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
	snprintf(inifile, BYTE128, "%s/conf/dbinfo.ini", home);
	if(access(inifile, F_OK) != 0) {
		printf("System config file does not exist, check it,%s, %s, %d\n", __FUNCTION__, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
	dic = iniparser_load(inifile);
	// 获取值
	char *p = NULL;
	// SystemConfigDir
	p = iniparser_getstring(dic, "SystemConfigDir:dir", NULL);
	p != NULL ? strncpy(sysconf_dir, p, BYTE128) : 0;
#ifdef DEBUG_STDOUT
	printf("[SysConfigDir]: %s\n", sysconf_dir);
#endif
	// TableDefineDir
	p = iniparser_getstring(dic, "TableConfigDir:dir", NULL);
	p != NULL ? strncpy(tblconf_dir, p, BYTE128) : 0;
#ifdef DEBUG_STDOUT
	printf("[TableConfigDir]: %s\n", tblconf_dir);
#endif
	// DataControl
	p = iniparser_getstring(dic, "DataControl:from", NULL);
	if(p != NULL) {
		if(strcmp(p, "socket") == 0)
			data_src = DATASOURCE_SOCKET;
		else if(strcmp(p, "file") == 0)
			data_src = DATASOURCE_FILE;
	}
#ifdef DEBUG_STDOUT
	printf("[DataSource]: %d\n", data_src);
#endif
	// DataDest
	p = iniparser_getstring(dic, "DataControl:to", NULL);
	if(p != NULL) {
		if(strcmp(p, "db") == 0)
			data_dst = DATADEST_DB;
		else if(strcmp(p, "file") == 0)
			data_dst = DATADEST_FILE;
	}
#ifdef DEBUG_STDOUT
	printf("[DataDest]: %d\n", data_dst);
#endif
	// Oracle
	p = iniparser_getstring(dic, "Oracle:service", NULL);
	p != NULL ? strncpy(service, p, BYTE32): 0;
	p = iniparser_getstring(dic, "Oracle:user", NULL);
	p != NULL ? strncpy(user, p, BYTE32): 0;
	p = iniparser_getstring(dic, "Oracle:password", NULL);
	p != NULL ? strncpy(password, p, BYTE32): 0;
#ifdef DEBUG_STDOUT
	printf("[service]: %s\n", service);
	printf("[user]: %s\n", user);
	printf("[password]: %s\n", password);
#endif
	dpbfs = iniparser_getint(dic, "DirectPath:buffer", 65536);
#ifdef DEBUG_STDOUT
	printf("[dpbfs]: %d\n", dpbfs);
#endif
	// ReceivePool
	rcvps = iniparser_getint(dic, "ReceivePool:num", 100000);
#ifdef DEBUG_STDOUT
	printf("[ReceivePoolNodeNum]: %d\n", rcvps);
#endif
	// TablePool
	tblps = iniparser_getint(dic, "TablePool:num", 100000);
#ifdef DEBUG_STDOUT
	printf("[TablePoolNodeNum]: %d\n", rcvps);
#endif
	// Server
	lsn_port = iniparser_getint(dic, "Server:port", 0);
#ifdef DEBUG_STDOUT
	printf("[ServerPort]: %d\n", lsn_port);
#endif
	// Client
	client_num = iniparser_getint(dic, "Client:num", 0);
#ifdef DEBUG_STDOUT
	printf("[ClientNum]: %d\n", client_num);
#endif
	clientinfo = (client_info*)calloc(client_num, sizeof(client_info));
	if(clientinfo == NULL) {
		printf("Failed to alloc clientinfo,%s, %s, %d\n", __FUNCTION__, __FILE__, __LINE__);
		iniparser_freedict(dic);
		exit(EXIT_FAILURE);
	}
	int i;
	for(i=0; i<client_num; ++i) {
		char cli[BYTE16] = {0};
		snprintf(cli, BYTE16, "Client:c%d", i+1);
		p = iniparser_getstring(dic, cli, NULL);
#ifdef DEBUG_STDOUT
		if(p != NULL)
			printf("c%d=%s\n", i+1, p);
#endif
		if(p != NULL) {
			int len = strlen(p);
			char *colon = memchr(p, ':', len);
			strncpy(clientinfo[i].ip, p, colon-p);
			char *port = colon+1;
			clientinfo[i].port = atoi(port);
			clientinfo[i].connected = 0;
#ifdef DEBUG_STDOUT
			printf("clientinfo[%d]: ip=%s, port=%d, connected=%d\n", i, clientinfo[i].ip, clientinfo[i].port, clientinfo.connected);
#endif
		}
	}

	iniparser_freedict(dic);
}

// 读取表结构配置文件
void read_table_config() {
}

#ifdef __cplusplus
}
#endif

