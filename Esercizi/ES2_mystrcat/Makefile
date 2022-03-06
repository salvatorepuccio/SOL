CC =  gcc
CFLAGS += -g




func_set1.o: 
	$(CC) -c func_set1.c

my_lib.a: func_set1.o
	ar rvs libmy_lib.a func_set1.o

main.o: my_lib.a
	$(CC) $(CFLAGS) main.c -L. -lmy_lib -o main.o

.PHONY: test1 test2 cleanall

test1: main.o
		./main.o 1 2 3 4 5 6

test1_n:
		./main.o 1 2 3 4 5 6

test2: main.o
	./main.o Prima Seconda Terza Quarta Quinta Sesta

test2_n:
	./main.o Prima Seconda Terza Quarta Quinta Sesta


cleanall: 
	-rm -f *.o *.a *.out

.SUFFIXES: .c .h

all		:

clean		: 
	-rm -f $(TARGETS)