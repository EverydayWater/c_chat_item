#include "protocol.h"
#include <semaphore.h>
#include <sys/types.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
//pthread_rwlock_t rwlock;

void Usr_Register(int sock_fd)//注册
{
	char temp[20] = "";
	char temp_copy[20] = "";
	
	Register *p_Reg = malloc(sizeof(Register));
	sermsg *serbuf = malloc(sizeof(sermsg));
	

	do
	{	
		printf("请输入名字: ");
		scanf("%s", p_Reg->name);
		getchar();
		printf("请输入密码: ");
		scanf("%s", temp);
		getchar();
		printf("请重新输入密码: ");
		scanf("%s", temp_copy);
		getchar();
	}while(strcmp(temp, temp_copy));
	
	strcpy(p_Reg->password,temp);
	p_Reg->type = REGISTER;
	p_Reg->len = sizeof(Register);

	write(sock_fd,p_Reg,sizeof(Register));//向服务器发送注册申请信息	
	
	read(sock_fd,serbuf,sizeof(sermsg));//服务器返回注册帐号
	printf("亲，您获得的帐号为：");
	printf("%s\n",serbuf->buf);//服务器分配一个ID号
	sleep(2);
}

void Usr_Login(int sock_fd)//用户登录
{
	int id = 0;
	int count = 0;
	char sec[20] = "";
	
	log *log_in = malloc(sizeof(log));
	sermsg *serbuf = malloc(sizeof(sermsg));

	do
	{

		printf("请输入ID: ");
		scanf("%d",&id);
		getchar();
		printf("请输入密码：");
		gets(sec);

		strcpy(log_in->password, sec);
		log_in->id = id;
		log_in->type = LOGIN;
		log_in->len = sizeof(log);
		
		printf("before send....\n");
		write(sock_fd, log_in, sizeof(log));//发送数据

		read(sock_fd,serbuf,sizeof(sermsg));//接收数据
//		printf("flag = %d\n", serbuf->acc_flag);
//		printf("len = %d\n", serbuf->len);

		if(1 == serbuf->acc_flag)
		{
			printf("登录成功...\n");

			LoginSelect(sock_fd);

			break;
		}
		count++;
		printf("登录失败，请重新登录！\n");

	}while((0 == serbuf->acc_flag) && (count < 3));

}

void get_online(int sock_fd)//获取在线人数
{
	int ret;
	char buf[20] = "";

	chat *get_line = malloc(sizeof(chat));
	chat *chat_ser = malloc(sizeof(chat));
	
	get_line->type = ONLINE;
	get_line->len = sizeof(chat);

	write(sock_fd, get_line, sizeof(chat));

	while(1)
	{
		ret = read(sock_fd, chat_ser, sizeof(chat));
		if(-1 == ret)
		{
			perror("read error!\n");
			exit(-2);
		}
		if(ret > 0)
		{
			if(chat_ser->len == 0)
			{
				gets(buf);
				if(!strcmp(buf, "quit"))
				{
					return;
				}
			}
			printf("@id:%d\n", chat_ser->id);	
		}
		if(0 == ret)
		{
			gets(buf);
			if(!strcmp(buf, "quit"))
			{
				//printf("退出获取在线信息服务...\n");
				return;
			}
	//		printf("读取数据完毕！\n");
	//		break;
		}
	}

}

void * p_chat_recv(void *arg)//单聊接收
{
//	pthread_mutex_t mutex;
	chat *per_chat = malloc(sizeof(chat));
	int sock_fd = *(int *)arg;

	while (1)
	{
//		pthread_mutex_lock(&mutex);
		int ret = read(sock_fd, per_chat, sizeof(chat));
		if (-1 == ret)
		{
			perror("read failed!\n");
			exit(1);
		}

		if (0 == ret)
		{
			break;
		}
		printf("@:%d:%s\n", per_chat->id, per_chat->buf);
/*		if(!strcmp(per_chat->buf, "quit"))
		{
			printf("退出私聊程序...\n");
		//	break;
			return NULL;
	//		pthread_exit(NULL);
//		pthread_mutex_unlock(&mutex);
		}
*/	}

	return NULL;
}

void * p_chat_send(void *arg)//单聊发送
{

//	pthread_mutex_t mutex;
	typedef struct sock
	{
		pthread_t tid_kill;
		int sock_fd;
	}sock;//结构体传参数

	sock *buf = malloc(sizeof(sock));

	chat *per_chat = malloc(sizeof(chat));

	buf = (sock *)arg;

	int id;
	printf("请输入ID: ");

	scanf("%d",&id);
	getchar();

	while(1)
	{

		per_chat->id = id;
		per_chat->type = PERSON_CHAT;
		gets(per_chat->buf);
		per_chat->len = sizeof(chat);

		if(!strcmp(per_chat->buf, "quit"))
		{
			printf("退出私聊程序...\n");
			pthread_cancel(buf->tid_kill);
			pthread_exit(NULL);
		}

		int ret = write(buf->sock_fd,per_chat,sizeof(chat));
		if(-1 == ret)
		{
			perror("write error!\n");
			exit(-2);
		}

		if(0 == ret)
		{
			break;
		}

	}

	return NULL;
}

void person_chat(int sock_fd)//单聊
{	
	typedef struct sock
	{
		pthread_t tid_kill;
		int sock_fd;
	}sock;

	pthread_t tid1;
	pthread_t tid2;
	
	sock *buf = malloc(sizeof(sock));

//	tid1 = gettid();

	pthread_create(&tid1, NULL, p_chat_recv, (void *)&sock_fd);

	buf->tid_kill = tid1;
	buf->sock_fd = sock_fd;
	pthread_create(&tid2, NULL, p_chat_send, /*(void *)&sock_fd*/(void *)buf);

	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
}
void group_chat(int sock_fd)//群聊
{
	chat *group_chat = malloc(sizeof(chat));
	int ret;

	while(1)
	{
		group_chat->type = GROUP_CHAT;
		printf("请输入群发信息:");
		gets(group_chat->buf);
		group_chat->len = sizeof(chat);

		if(!strcmp(group_chat->buf, "quit"))
		{
			printf("退出群聊\n");
			break;
		}
	//	printf("%s\n",group_chat->buf);
		ret = write(sock_fd,group_chat,sizeof(chat));

		if(-1 == ret)
		{
			perror("write error!\n");
			exit(-2);
		}
		if(0 == ret)
		{
			;
		}

	}
}
void trans_file(int sock_fd)//传输文件
{

}
void chat_quit(int sock_fd)//登出
{
	log *log_out = malloc(sizeof(log));
	sermsg *serbuf = malloc(sizeof(sermsg));

	
	log_out->type = LOGOUT;
	log_out->len = sizeof(log);
	log_out->id = 111;

	write(sock_fd, log_out, sizeof(log));//发送数据

	read(sock_fd, serbuf, sizeof(sermsg));//接收数据


	if(0 == serbuf->acc_flag)
	{
		printf("成功退出...\n");
		exit(-1);
	}

}

