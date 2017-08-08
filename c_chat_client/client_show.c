#include "protocol.h"
int fun_select()
{
	int SeekFun = 0;
	
	printf("\n\t\t***************\n\n");
	printf("\t\t1,注册\n");
	printf("\t\t2,登录\n");
	printf("\t\t0,退出\n");
	printf("\n\t\t***************\n\n");
	
	printf("Input choice SeekFun: ");
	scanf("%d", &SeekFun);
	while ('\n' != getchar())
	{
		;
	}

	return SeekFun;

} 


void ClientSelect(int sock_fd)
{	
	int select = 0;
	while (1)
	{
		select = fun_select();
		switch (select)
		{
			case 1:
				system("clear");
				printf("注册\n");	
				Usr_Register(sock_fd);
				break;
			case 2:
				system("clear");
				printf("登录\n");
			  	Usr_Login(sock_fd);
				break;
			case 0:
				system("clear");
				printf("退出\n");
				return;
			default:
				break;
		}
	}
}


int Login_date()
{
	int SeekLogin = 0;
	
	printf("\n\t\t***************\n\n");
	printf("\t\t1,获取在线人数\n");
	printf("\t\t2,私人聊天\n");
	printf("\t\t3,群聊\n");
//	printf("\t\t4,传输文件\n");
	printf("\t\t0,登出\n");
	printf("\n\t\t***************\n\n");
	
	printf("Input choice SeekLogin: ");
	scanf("%d", &SeekLogin);
	while ('\n' != getchar())
	{}
	return SeekLogin;
} 


void LoginSelect(int sock_fd)
{	
	int select = 0;
	
	while (1)
	{
		select = Login_date();
		switch (select)
		{
			case 1:
				system("clear");
				printf("获取在线人数\n");	
				get_online(sock_fd);
				break;
			case 2:
				system("clear");
				printf("私人聊天\n");
				person_chat(sock_fd);
				break;
			case 3:
				system("clear");
				printf("群聊\n");
				group_chat(sock_fd);
				break;
			case 4:
				system("clear");
				printf("传输文件\n");
				trans_file(sock_fd);
				break;
			case 0:
				system("clear");	
				printf("登出\n");
				chat_quit(sock_fd);
				break;
			default:
				break;
		}
	}
}
