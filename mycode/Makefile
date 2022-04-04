CC =  gcc
CFLAGS += -std=c99 -Wall -g -D_POSIX_SOURCE
INCLUDES = -I ./includes/
 

OBJ_DIR = ./obj
LIBS_DIR = ./mylibs

SRC_DIR = ./srcmaiusc#<----------modificami ma non lasciare spazi prima del #

TARGETS = $(OBJ_DIR)/server.o $(OBJ_DIR)/client.o


normal: $(TARGETS)

#COMPILAZIONE--------------------------.c-------------------------------------------------
#$(OBJ_DIR)/$(TARGET).o: $(LIBS_DIR)/my_lib.a
#	$(CC) $(CFLAGS) $(INCLUDES) $(SRC_DIR)/$(TARGET).c -L$(OBJ_DIR) -lmy_lib -o $(TARGET).o
#	@mv $(TARGET).o $(OBJ_DIR)/$(TARGET).o
#	@mv $(TARGET).o.dSYM $(OBJ_DIR)/$(TARGET).o.dSYM

$(OBJ_DIR)/server.o: $(LIBS_DIR)/my_lib.a
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC_DIR)/server.c -L$(OBJ_DIR) -lmy_lib -o server.o -lpthread
	@mv server.o $(OBJ_DIR)/server.o
	@mv server.o.dSYM $(OBJ_DIR)/server.o.dSYM

$(OBJ_DIR)/client.o: $(LIBS_DIR)/my_lib.a
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC_DIR)/client.c -L$(OBJ_DIR) -lmy_lib -o client.o -lpthread
	@mv client.o $(OBJ_DIR)/client.o
	@mv client.o.dSYM $(OBJ_DIR)/client.o.dSYM
#COMPILAZIONE--------------------------.c-------------------------------------------------





#LIBRERIA------------------------------.a---------------------------------------------
$(LIBS_DIR)/my_lib.a: stringsmanager.o id.o
	ar rvs libmy_lib.a stringsmanager.o id.o
	@mv libmy_lib.a $(OBJ_DIR)/libmy_lib.a
#	@mv stringsmanager.o $(OBJ_DIR)/stringsmanager.o
	rm -f *.o

stringsmanager.o:
	$(CC) -c $(LIBS_DIR)/stringsmanager.c

id.o:
	$(CC) -c $(LIBS_DIR)/id.c

#LIBRERIA-------------------------------.a--------------------------------------------





#ESECUZIONE----------------------------.o-----------------------------------------------
run:
	$(OBJ_DIR)/$(TARGET).o

s:
	$(OBJ_DIR)/server.o

c: 
	$(OBJ_DIR)/client.o

#ESECUZIONE----------------------------.o-----------------------------------------------
	

.PHONY: cleanall clean
.SUFFIXES: .c .h

clean:
	-rm -f  $(OBJ_DIR)/*.o
	-rm -r $(OBJ_DIR)/*.dSYM
	-rm -f $(OBJ_DIR)/*.a
	-rm -f *.a *.o *.txt *.out mysocket
	-rm -r *.dSYM

cleanall: 
	-rm -f *.o *.a *.out *.txt mysocket
	-rm -r *.dSYM
	-rm -f *.a



