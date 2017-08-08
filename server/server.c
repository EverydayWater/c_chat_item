#include "protocol.h"
/*
 *函数名：GetSock_fd
 *参数：
 *返回值：int
 *说明：
 *
 */

int GetSock_fd()//获取sock_fd
{
	int sock_fd;

	sock_fd = socket(AF_INET,SOCK_STREAM,0);
	if(-1 == sock_fd)
	{
		perror("create socket faild");
		exit(-1);
	}

	return sock_fd;
}

void MyBind(int sock_fd,struct sockaddr_in *p_addr)//绑定IP和端口
{
	int ret;
	memset(p_addr,0,sizeof(struct sockaddr_in));

	p_addr->sin_family = AF_INET;
	p_addr->sin_addr.s_addr = INADDR_ANY;
	p_addr->sin_port = htons(SERVER_PORT);

	ret = bind(sock_fd,(struct sockaddr *)p_addr,sizeof(struct sockaddr_in));
	if(-1 == ret)
	{
		perror("bind error");
		exit(-2);
	}
}

void MyListen(int sock_fd)//服务器监听
{
	int ret;

	ret = listen(sock_fd,LISTEN);
	if(-1 == ret)
	{
		perror("listen error");
		exit(-3);
	}
}

int MyAccept(int sock_fd,struct sockaddr_in *p_addr)//接收客户请求
{
	int new_fd;
	int len = sizeof(struct sockaddr_in);

	new_fd = accept(sock_fd,(struct sockaddr *)p_addr,&len);

	return new_fd;
}











