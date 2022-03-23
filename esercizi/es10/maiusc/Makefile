CC =  gcc
CFLAGS += -g
TARGETS = client.o server.o 

normal: $(TARGETS)


server.o: my_lib.a
	$(CC) $(CFLAGS) server.c -L. -lmy_lib -o server.o -lpthread

my_lib.a: id.o
	ar rvs libmy_lib.a id.o	

id.o: 
	$(CC) -c id.c

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
	-rm -f mysocket

.SUFFIXES: .c .h

all	:

clean		: 
	-rm -f $(TARGETS)