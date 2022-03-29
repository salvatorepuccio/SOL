CC =  gcc
CFLAGS += -std=c99 -Wall -g
TARGET = prova


HEADERS_DIR = ./includes/
OBJ_DIR = ./obj/
PROJ_DIR = ./test/
LIBS_DIR = ./myutils/

normal: $(TARGET)


$(PROJ_DIR)/$(OBJ_DIR)/$(TARGET).o: my_lib.a
	$(CC) $(CFLAGS) $(TARGET).c -L. -lmy_lib -o $(TARGET).o -lpthread
	@mv $(TARGET).o $(OBJ_DIR)/$(TARGET).o

$(LIBS_DIR)/my_lib.a: myutils.o
	ar rvs libmy_lib.a myutils.o
	@mv $(TARGET).o $(OBJ_DIR)/my_lib.a

utils.o: 
	$(CC) -c utils.c

client.o:
	$(CC) $(CFLAGS) client.c -o client.o

.PHONY: cleanall

s: server.o
		./server.o

c: client.o
		./client.o
		

cl: 
	-rm -f *.o *.a *.out *.txt
	-rm -r *.dSYM
	-rm -f mysocket

.SUFFIXES: .c .h

all	:

clean		: 
	-rm -f $(TARGETS)