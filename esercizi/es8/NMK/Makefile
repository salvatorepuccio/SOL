CC =  gcc
CFLAGS += -g




queue.o: 
	$(CC) -c queue.c

my_lib.a: queue.o
	ar rvs libmy_lib.a queue.o

main.o: my_lib.a
	$(CC) $(CFLAGS) main.c -L. -lmy_lib -o main.o -lpthread

.PHONY: test1 test2 cleanall

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