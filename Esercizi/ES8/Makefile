CC =  gcc
CFLAGS += -g
TARGETS = client.o server.o 

normal: $(TARGETS)

server.o: 
	$(CC) $(CFLAGS) server.c -o server.o -lpthread

client.o:
	$(CC) $(CFLAGS) client.c -o client.o

.PHONY: cleanall

server: server.o
		./server.o

client: client.o
		./client.o
		

cleanall: 
	-rm -f *.o *.a *.out *.txt
	-rm -r *.dSYM

.SUFFIXES: .c .h

all	:

clean		: 
	-rm -f $(TARGETS)