TARGET = main
CC = gcc
CFLAG =-g
LDFLAG = -lpool
INC =-I./inc
OBJS = serfunction.o  servermain.o  server.o\

$(TARGET):$(OBJS)
		$(CC) $^ -o $@ $(LDFLAG)
	
%.o:%.c
		$(CC) -c $< -o $@ $(CFLAG) $(INC)

.PHONY:clean

clean:
	$(RM) -f *.o main 
