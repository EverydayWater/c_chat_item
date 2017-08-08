#include "protocol.h"

int Socket(int domain, int type, int protocol)
{
    int sock_fd;

    sock_fd = socket(domain, type, protocol);
    if(sock_fd < 0)
    {
		perror("socket");
		exit(1);
    }

    return sock_fd;
}

void set_server(struct sockaddr_in *s_info)
{
    s_info->sin_family = AF_INET;
    s_info->sin_port = htons(SERVER_PORT);
    s_info->sin_addr.s_addr = inet_addr(SERVER_ADDR);
}

int Connect(int sockfd, struct sockaddr *serv_addr, int addrlen)
{
    int ret;
    
    ret = connect(sockfd,  serv_addr, addrlen);
    if(ret < 0)
    {
		perror("connect");
		exit(1);
    }

	return 0;
}

