#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <error.h>
#include <signal.h>
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

#define MAX_THREAD 15
#define LISTEN  20
#define SERVER_PORT 8000
#define NAMELEN 20
#define PASSWORD 32
#define BUFSIZE (sizeof(char) * 1024)
#define IDLEN 32
#define IPLEN 32

//pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;

//主链表节点
typedef struct node
{
	int fd;
	char name[NAMELEN];
	int id;
	char password[PASSWORD];
	int online;

	struct node * next;
}node;

typedef struct servermsg
{
	int len;
	char type;
	int acc_flag;
	char buf[BUFSIZE];
}sermsg;

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
	char buf[BUFSIZE];
}chat;


void *client_fun(void *arg);

void insert_node(node *head_node, Register *regbuf,int id);

int login(node *head_node,int id,char*password,int fd);

void logout(int fd,node *head_node,log *logbuf,sermsg *serbuf);

void usr_Register(sermsg *serbuf,int id);

void regis(int len,int fd,node *head_node);

void person_login(int fd,node *head_node,int len);

void person_logout(int fd,int len,node *head_node);

void single_chat(int fd,int len,node *head_node);

void GroupChat(int fd,int len,node *head_node);

void online(int fd,int len,node *head_node);

int GetSock_fd();

void MyBind(int sock_fd,struct sockaddr_in *p_addr);

void MyListen(int sock_fd);

int MyAccept(int sock_fd,struct sockaddr_in *p_addr);

void W_List(node *head_node);

void R_List(node *head_node);

int my_read(int fd, char * buf, ssize_t size);

int my_write(int fd, char * buf, ssize_t size);

int person_chat(int fd,node *head_node,chat *chatbuf);

int group_chat(node *head_node,chat *chatbuf,int fd);

void online_person(int fd,node *head_node,chat *chatbuf);

void destroy_list(node *head_node);

void fun(void *arg);
//读
//失败返回-1，成功返回读入字符数
int my_read(int fd, char * buf, ssize_t size);

//写
//失败返回-1， 成功返回写入字符数
int my_write(int fd, char * buf, ssize_t size);
