CC =  gcc
CFLAGS += -std=c99 -Wall -g
TARGETS = prova

HEADERS_DIR = ./includes/
OBJ_DIR = ./obj/
PROJ_DIR = ./test/
LIBS_DIR = ./myutils/

normal: $(TARGETS)


$(PROJ_DIR)/obj/test.o: my_lib.a
	$(CC) $(CFLAGS) server.c -L. -lmy_lib -o server.o -lpthread

my_lib.a: myutils.o
	ar rvs libmy_lib.a id.o	

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