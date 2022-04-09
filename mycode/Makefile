CC =  gcc
CFLAGS += -std=c99 -w -Wall -g -D_POSIX_SOURCE
INCLUDES = -I ./includes/
LIBS = -L$(OBJ_DIR) -lmy_lib  -lpthread
 
BUILD_DIR := ./build
OBJ_DIR := ./obj
LIBS_DIR := ./mylibs

SRC_DIR = ./srcmaiusc#<----modificami ma non lasciare spazi prima del #

.DEFAULT_GOAL := all

#TARGETS = $(OBJ_DIR)/server.o $(OBJ_DIR)/client.o

TARGETS = server client

#COMPILAZIONE--------------------------.c-------------------------------------------------
server.o: my_lib.a
	$(CC) $(CFLAGS) $(INCLUDES) -c $(SRC_DIR)/server.c $(LIBS)
	@mv server.o $(OBJ_DIR)/server.o
#	@mv server.o.dSYM $(OBJ_DIR)/server.o.dSYM

client.o: my_lib.a
	$(CC) $(CFLAGS) $(INCLUDES) -c $(SRC_DIR)/client.c $(LIBS)
	@mv client.o $(OBJ_DIR)/client.o
#	@mv client.o.dSYM $(OBJ_DIR)/client.o.dSYM
#COMPILAZIONE--------------------------.c-------------------------------------------------


#LINKING---------------------.o----------------------------
server: server.o client.o
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/server $(OBJ_DIR)/server.o $(LIBS)

client: client.o
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/client $(OBJ_DIR)/client.o $(LIBS)

#LINKING---------------------.o----------------------------



#LIBRERIA------------------------------.a---------------------------------------------
my_lib.a: stringsmanager.o id.o
	ar rvs libmy_lib.a stringsmanager.o id.o
	@mv libmy_lib.a $(OBJ_DIR)/libmy_lib.a
	@mv stringsmanager.o $(OBJ_DIR)/stringsmanager.o
	@mv id.o $(OBJ_DIR)/id.o
	rm -f *.o

stringsmanager.o:
	$(CC) -c $(LIBS_DIR)/stringsmanager.c

id.o:
	$(CC) -c $(LIBS_DIR)/id.c

#LIBRERIA-------------------------------.a--------------------------------------------





#ESECUZIONE----------------------------.o-----------------------------------------------
#run:
#	$(OBJ_DIR)/$(TARGET).o

runs:
	$(BUILD_DIR)/server

runc: 
	$(BUILD_DIR)/client 

test1: server client
#	valgrind -leak-check=full $(BUILD_DIR)/server
	@chmod +x scripts/script1.sh
	scripts/script1.sh

#ESECUZIONE----------------------------.o-----------------------------------------------
	

.PHONY: cleanall clean
.SUFFIXES: .c .h

clean:
	-rm -f ./obj/*

cleanall:
	-rm -f  $(OBJ_DIR)/*
	-rm -f $(BUILD_DIR)/*
	-rm -f mysocket


all: $(TARGETS)

