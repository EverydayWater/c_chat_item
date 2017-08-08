#include "protocol.h"

/*
 * 函数名：main
 * 返回值：int
 * 参数：void
 * 说明：主函数
 */
int main(void)
{
	int sock_fd;

	struct sockaddr_in addr1; //服务器结构体
	struct sockaddr_in addr2; //客户结构体

	pool_t *pool = pool_create(MAX_THREAD); //创建线程池

	node *head_node = malloc(sizeof(node));//创建头节点
	memset(head_node,0,sizeof(node));
	head_node->next = NULL;

	R_List(head_node); //从文件中读出链表

	sock_fd = GetSock_fd();
	MyBind(sock_fd,&addr1); //绑定
	MyListen(sock_fd); //监听

	
	while(1)
	{
		head_node->fd = MyAccept(sock_fd,&addr2); //接收
		if(-1 == head_node->fd)
		{
			perror("accept error");
			continue;
		}
		
		pool_add(pool,client_fun,head_node); //添加任务
	}

	return 0;
}
