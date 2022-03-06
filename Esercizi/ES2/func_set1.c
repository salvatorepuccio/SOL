

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

 
void RIALLOCA(char** buf, size_t newsize) {

    if(*buf==NULL){
        *buf = (char*)malloc(sizeof(char)*newsize);
    }
    else{
        //char *nuovo = malloc(sizeof(char)*(size+REALLOC_INC));
        *buf=(char*)realloc(*buf,sizeof(char)*newsize);
    }
}

char* mystrcat(char *buf, size_t sz, char *first, ...) {

    char* curr=NULL;
    int slots_array=sz;
    va_list ap;
    va_start(ap,first);
    curr=first;
    while(curr!=NULL){
        while((slots_array - strlen(buf)) < strlen(curr)){
            slots_array = slots_array + sz;
            RIALLOCA(&buf,slots_array);
        }
        strcat(buf,curr);
        curr=va_arg(ap,char*); 
    }va_end(ap);
    return buf;
} 