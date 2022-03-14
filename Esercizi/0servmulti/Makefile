CC =  gcc
CFLAGS += -g
TARGETS = client.o server.o

normal: $(TARGETS)

server.o: server.c
	$(CC) $(CFLAGS) server.c -o server.o -lpthread

client.o: client.c
	$(CC) $(CFLAGS) client.c -o client.o

.PHONY: test1 test2 cleanall

server: server.o
		./server.o
client: client.o
		./client.o
		

cleanall: 
	-rm -f *.o *.a *.out
	-rm -r *.dSYM

.SUFFIXES: .c .h

all		:

clean		: 
	-rm -f $(TARGETS)