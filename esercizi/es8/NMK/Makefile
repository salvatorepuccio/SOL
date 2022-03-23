CC =  gcc
CFLAGS += -g
TARGETS = main.o


main.o: my_lib.a
	$(CC) $(CFLAGS) main.c -L. -lmy_lib -o main.o -lpthread

my_lib.a: queue.o
	ar rvs libmy_lib.a queue.o	

queue.o: 
	$(CC) -c queue.c



.PHONY: test1 cleanall

test1: main.o
		./main.o -p 3 -c 2 -n 100

test1_n:
		./main.o -p 3 -c 2 -n 100



cleanall: 
	-rm -f *.o *.a *.out
	-rm -r *.dSYM

.SUFFIXES: .c .h

all		:

clean		: 
	-rm -f $(TARGETS)