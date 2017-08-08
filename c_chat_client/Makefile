TARGET = main
CC = gcc
CFLAG = -Wall -g 
LDFLAG = -lpthread
INC =-I./inc
OBJS = ./client_fun.o  ./client_show.o \
	   	   ./socket.o  ./main.o  \

$(TARGET):$(OBJS)
	$(CC) $^ -o $@ $(LDFLAG)

%.o:%.c
	$(CC) -c $< -o $@ $(CFLAG) $(INC)

.PHONY:clean
clean:
	$(RM) -f *.o main 

#$^:所有依赖
#$@:要生成的目标
#$<:第一个依赖
