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


int mystrcat(char** target, char* source, int oversize,int log){
	if(source==NULL) return -1;

	int len=-1,targetL=-1,sourceL=-1;
	char *target_original;
	sourceL = strlen(source);

	if(*target == NULL){
		target_original = malloc(5);
		strcpy(target_original,"null");
		targetL=0;
		len = strlen(source)+1+oversize;
		*target = malloc(sizeof(char)*len);
	}
	else{
		target_original=malloc(strlen(*target));
		strcpy(target_original,*target);
		targetL = strlen(*target);
		len = targetL+sourceL+oversize;
		enlarge(target,len);
	}

	strncat(*target,source,sourceL);

	if(log==1) printf("\n[*************]\n[*]\tYou called mystrcat(target,source,%d) w/: \
	\n[*]\ttarget: '%s' [%d] \
	\n[*]\tsource: '%s'  [%d] \
	\n[*]\ttarget (new): '%s' [%d]\n",oversize,target_original,targetL,source,sourceL,*target,len);
	
	return len;
}