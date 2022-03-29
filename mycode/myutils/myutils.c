#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//nuovo
#include <sys/select.h>
#include <sys/un.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>


int enlarge(char** enlargeme,int dim){
	//printf("enlarge '%s'\n with size: %ld new size: %d",*enlargeme,strlen(*enlargeme),dim);
    if(dim<=strlen(*enlargeme))return -1;
	char *new_bigger=malloc(sizeof(char)*dim);
	strncpy(new_bigger,*enlargeme,strlen(*enlargeme));
	free(enlargeme);
	*enlargeme = new_bigger;
}


int mystrcat(char** paste, char* copy, int oversize){
	int len;

	if(*paste == NULL){
		len = strlen(copy)+oversize;
		*paste = malloc(sizeof(char)*len);
	}
	else{
		len = strlen(*paste)+strlen(copy)+oversize;
		enlarge(paste,len);
	}
	strncat(*paste,copy,strlen(copy));
	return len;
}