#include "protocol.h"


int main(int argc, char * argv[])
{
	struct sockaddr_in s_info;
    int sock_fd;

    //创建套接字
    sock_fd = Socket(AF_INET, SOCK_STREAM, 0);

    //填充服务器
    set_server(&s_info);
    //向服务器发起连接请求
   	Connect(sock_fd, (struct sockaddr *)&s_info, sizeof(s_info));

	//进行数据通信
	ClientSelect(sock_fd);	

	//关闭套接字
    close(sock_fd);

    return 0;
}
