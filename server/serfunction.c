#include "protocol.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;//初始化互斥锁

void *client_fun(void *arg)//客户功能实现
{
	node *head_node = arg;//传参为head_node 
	int fd = head_node->fd;
	int len;
	int ret;
	char msg_type;

	while(1)
	{
		ret = read(fd,&len,sizeof(int));
		if(ret == 0)
		{
			break;
		}
		ret = read(fd,&msg_type,sizeof(char));
		if(ret > 0)
		{
				switch(msg_type)
				{
							case REGISTER:
									regis(len,fd,head_node);  //注册
									break;
							case LOGIN:
									person_login(fd,head_node,len);  //登录
									break;
							case LOGOUT:
									person_logout(fd,len,head_node);  //退出登录
									break;
							case PERSON_CHAT:
									single_chat(fd,len,head_node);    //私聊
									break;
							case GROUP_CHAT:
									GroupChat(fd,len,head_node);     //群聊
									break;
							case ONLINE:
									online(fd,len,head_node);       //查看在线人员
									break;
							default :
									break;
				}
		}
	}
							  
}

//插入链表
void insert_node(node *head_node, Register *regbuf,int id)
{
	node *new_node = malloc(sizeof(node));
	strcpy(new_node->name,regbuf->name);
	strcpy(new_node->password,regbuf->password);
	new_node->fd = -1;
	new_node->id = id;
	node *temp = head_node;

	while(temp->next != NULL)
	{
		temp = temp->next;
	}
	temp->next = new_node;
}

//查找登录人的id和密码
int login(node *head_node,int id,char*password,int fd)
{
	node *temp = head_node->next;

	while(temp != NULL)
	{
		if((temp->id == id) && (!strcmp(temp->password,password)))
		{			
			temp->online = 1;
			temp->fd = fd;
			return 1;
		}
		temp = temp->next;
	}
	return 0;
}

//退出登录
void logout(int fd,node *head_node,log *logbuf,sermsg *serbuf)
{
	serbuf->acc_flag = 1;
	node *temp = head_node->next;
	while(temp != NULL)
	{
		if(temp->fd == fd)
		{
			logbuf->id = temp->id;
		}
		temp = temp->next;
	}

	temp = head_node->next;
	while(temp != NULL)
	{
		if(temp->id == logbuf->id)
		{
			head_node->online = 0;
			close(temp->fd);
			serbuf->acc_flag = 0;
		}
		temp = temp->next;
	}
	serbuf->len = sizeof(sermsg);
	serbuf->type = LOGOUT;
}


int person_chat(int fd,node *head_node,chat *chatbuf)
{
		chat *chat_node = malloc(sizeof(chat));
		node *temp = head_node->next;
		while(temp != NULL)
		{
			if(temp->fd == fd)
			{
				chat_node->id = temp->id;
			}
			temp = temp->next;
		}

		temp = head_node->next;
		while(temp != NULL)
		{
			if(temp->id == chatbuf->id && temp->online == 1)
			{
				chat_node->type = PERSON_CHAT;
				chat_node->len = sizeof(chat);
				strcpy(chat_node->buf,chatbuf->buf);
				write(temp->fd,chat_node,sizeof(chat));
				free(chat_node);
			}
			temp = temp->next;
		}
		return 0;
}

int group_chat(node *head_node,chat *chatbuf,int fd)
{
		chat *chat_node = malloc(sizeof(chat));
		node *temp = head_node->next;
		while(temp != NULL)
		{
			if(temp->fd == fd)
			{
				chat_node->id = temp->id;
			}
			temp = temp->next;
		}
		temp = head_node->next;
		while(temp != NULL)
		{
			if(temp->online == 1 && temp->fd != fd)
			{
				chat_node->len = sizeof(chat);
				chat_node->type = GROUP_CHAT;
				strcpy(chat_node->buf,chatbuf->buf);
				write(temp->fd,chat_node,sizeof(chat));
			}
			temp = temp->next;
		}
		free(chat_node);
		return 0;
}

void online_person(int fd,node *head_node,chat *chatbuf)
{
	node *temp = head_node->next;

	while(temp != NULL)
	{
		if(temp->online == 1 && temp->fd != fd)
		{
			chatbuf->len = sizeof(chat);
			chatbuf->type = ONLINE;
			chatbuf->id = temp->id;
			write(fd,chatbuf,sizeof(chat));
		}
		temp = temp->next;
	}
	chatbuf->len = 0;
	write(fd,chatbuf,sizeof(chat));
}

//将链表写入文件
void W_List(node *head_node)
{
	FILE *fp;
	node *p = NULL;
	p = head_node->next;

	if((fp = fopen("list.txt", "wb")) == NULL)
	{
		printf("无法打开list.txt!\n");
		return ;
	}

	while(p != NULL)
	{
		fwrite(p,sizeof(node),1,fp);
		p = p->next;
	}

	fclose(fp);
}


//注册
void regis(int len,int fd,node *head_node)
{
	node *temp = head_node;
	int id = 0;
	int ret = 0;
	Register *regbuf = malloc(sizeof(Register));
	sermsg *serbuf = malloc(sizeof(sermsg));
    memset(serbuf,0,sizeof(sermsg));
	memset(regbuf,0,sizeof(Register));
	ret = read(fd,((char *)regbuf)+sizeof(int)+sizeof(char),len-sizeof(int)-sizeof(char));
	pthread_mutex_lock(&mutex);
	if(temp->next == NULL)
	{
		id = 100000;
	}                                           
	else
	{
		while(temp->next != NULL)
		{
			temp = temp->next;
		}
		id = temp->id +1;
	}
	insert_node(head_node,regbuf,id);
	pthread_mutex_unlock(&mutex);
	W_List(head_node);
	sprintf(serbuf->buf,"%d",id);
	serbuf->len = sizeof(sermsg);
	serbuf->type = REGISTER;
	serbuf->acc_flag = 1;
	ret = write(fd,serbuf,sizeof(sermsg));
	free(regbuf);
	free(serbuf);
}

//个人登录
void person_login(int fd,node *head_node,int len)
{
	int ret = 0;
	log *logbuf = malloc(sizeof(log));
	sermsg *serbuf = malloc(sizeof(sermsg));
	memset(serbuf,0,sizeof(sermsg));
	memset(logbuf,0,sizeof(log));
	ret = read(fd,((char *)logbuf)+sizeof(int)+sizeof(char),len-sizeof(int)-sizeof(char));
	ret = login(head_node, logbuf->id, logbuf->password,fd);
	if(ret == 1)
	{
		serbuf->acc_flag = 1;
		serbuf->len = sizeof(sermsg);
		serbuf->type = LOGIN;
	}
	if(ret == 0)
	{
		serbuf->acc_flag = 0;
		serbuf->len = sizeof(sermsg);
		serbuf->type = LOGIN;
	}
	ret = write(fd,serbuf,sizeof(sermsg));
	free(logbuf);
	free(serbuf);
}


//退出个人登录
void person_logout(int fd,int len,node *head_node)
{
	int ret;
	log *logbuf = malloc(sizeof(log));
	sermsg *serbuf = malloc(sizeof(sermsg));
	memset(serbuf,0,sizeof(sermsg));
    memset(logbuf,0,sizeof(log));
	ret = read(fd,((char *)logbuf)+sizeof(int)+sizeof(char),len-sizeof(int)-sizeof(char));
	logout(fd,head_node,logbuf,serbuf);
	write(fd,serbuf,sizeof(sermsg));
	free(logbuf);
	free(serbuf);
}

//私聊
void single_chat(int fd,int len,node *head_node)
{
	int ret;
	chat *chatbuf = malloc(sizeof(chat));
	sermsg *serbuf = malloc(sizeof(sermsg));
	memset(serbuf,0,sizeof(sermsg));
	memset(chatbuf,0,sizeof(chat));
    ret = read(fd,((char *)chatbuf)+sizeof(int)+sizeof(char),len-sizeof(int)-sizeof(char));
	person_chat(fd,head_node,chatbuf);
	free(chatbuf);
	free(serbuf);
}

//群聊
void GroupChat(int fd,int len,node *head_node)
{
	int ret;
	chat *chatbuf = malloc(sizeof(chat));
	sermsg *serbuf = malloc(sizeof(sermsg));
    memset(serbuf,0,sizeof(sermsg));
	memset(chatbuf,0,sizeof(chat));
	ret = read(fd,((char *)chatbuf)+sizeof(int)+sizeof(char),len-sizeof(int)-sizeof(char));       
	group_chat(head_node,chatbuf,fd);
	free(chatbuf);
	free(serbuf);
}

//发送在线人数
void online(int fd,int len,node *head_node)
{
	chat *chatbuf = malloc(sizeof(chat));
	memset(chatbuf,0,sizeof(chat));
	read(fd,((char *)chatbuf)+sizeof(int)+sizeof(char),len-sizeof(int)+sizeof(char));
	online_person(fd,head_node,chatbuf);
	free(chatbuf);
}

//从文件中把数据读入链表
void R_List(node *head_node)
{
	FILE *fp = NULL;
	if((fp = fopen("list.txt", "rb")) == NULL)
	{
		printf("无法读取list.txt!\n");
		return ;
	}
	node *new_node = NULL;
	node *temp = head_node;
	while(!feof(fp))
	{
		new_node = (node *)malloc(sizeof(node));
		fread(new_node,sizeof(node),1,fp);
		if(feof(fp))                   
		{
			break;
		}
		new_node->next=NULL;
		while(temp->next != NULL)
		{
			temp = temp->next;
		}
		temp->next = new_node;

	}
	fclose(fp);
}

//摧毁链表
void destroy_list(node *head_node)
{

	node *p = NULL;

	p = head_node;
	while(head_node != NULL)
	{
		p = head_node;
		head_node = head_node->next;
		free(p);
	}
}


