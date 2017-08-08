#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <error.h>
#include <errno.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pool.h>

#define REGISTER '1'
#define LOGIN '2'
#define LOGOUT '3'
#define PERSON_CHAT '4'
#define GROUP_CHAT '5'
#define SERMSG '6'
#define ONLINE '7'

#define FAUSE -1

#define LISTEN  20
#define SERVER_PORT 8000
#define SERVER_ADDR "192.168.1.38"
#define NAMELEN 20
#define PASSWORD 32
#define BUFSIZE (sizeof(char) * 1024)
#define IDLEN 32
#define IPLEN 32

//主链表节点
typedef struct node
{
	int fd;
	char name[NAMELEN];
	int id;
	char password[PASSWORD];
	int online;

	struct node * next;
	struct node * pre;
}node;

//服务器数据包
typedef struct servermsg
{
	int len;
	char type;
	int acc_flag;
	char buf[BUFSIZE];
}sermsg;

//记录每个节点的fd
typedef struct list
{
	int fd;
	node *p_node;
}list;

//注册
typedef struct Register
{
	int len;
	char type;
	char name[NAMELEN];
	char password[PASSWORD];
}Register;

//登录登出
typedef struct log
{
	int len;
	char type;
	int id;
	char password[PASSWORD];
}log;

//聊天
typedef struct chat
{
	int len;
	char type;
	int id;
	char buf[BUFSIZE];//通过此标记单聊和群聊
}chat;

//读
//失败返回-1，成功返回读入字符数
int my_read(int fd, char * buf, ssize_t size);

//写
//失败返回-1， 成功返回写入字符数
int my_write(int fd, char * buf, ssize_t size);


int Socket(int domain, int type, int protocol);//创建套接字
void set_server(struct sockaddr_in *s_info);//填充结构体
int Connect(int sockfd, struct sockaddr *serv_addr, int addrlen);//建立连接

int fun_select();
void ClientSelect(int sock_fd);//客户选择注册、登录、还是退出程序

int Login_date();
void LoginSelect(int sock_fd);//客户选择获取在线信息、单聊、群聊、传输文件、登出

//void * get_id(void * arg);
void Usr_Register(int sock_fd);//注册函数
void Usr_Login(int sock_fd);//登录函数

void get_online(int sock_fd);//获取在线人数
void person_chat(int sock_fd);//单聊
void group_chat(int sock_fd);//群聊
void trans_file(int sock_fd);//传输文件
void chat_quit(int sock_fd);//登出

//void * p_chat(void *arg);//单聊接收函数
void * p_chat_recv(void *arg);
void * p_chat_send(void *arg);

 #endif
