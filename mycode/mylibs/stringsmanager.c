#include <stdlib.h>
#include <string.h>
#include <stdio.h>
//


int enlarge(char** enlargeme,int dim){
	//printf("enlarge '%s'\n with size: %ld new size: %d",*enlargeme,strlen(*enlargeme),dim);
    if(dim<=strlen(*enlargeme))return -1;
	char *new_bigger=malloc(sizeof(char)*dim);
	strncpy(new_bigger,*enlargeme,strlen(*enlargeme));
	free(*enlargeme);
	*enlargeme = new_bigger;
	return 0;
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