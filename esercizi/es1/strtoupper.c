
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void strtoupper(const char* in, size_t len, char* out);


int main (int argc, char *argv[]){
    if (argc != 2) { printf("num argomenti inesatto (deve essere 1)\n"); return -1; }

    size_t size = strlen(argv[1]);
    char *upper=calloc(size,sizeof(int));
    //char *upper = malloc(sizeof(char)*size);
    strtoupper(argv[1],size,upper);
    printf("inserito: %s\nmaiuscolo: %s\n",argv[1],upper);
    free(upper);
    return 0;
}


void strtoupper(const char* in, size_t len, char* out){
    for(int i=0;i<len;i++){
        if(in[i]>96&&in[i]<123) 
            out[i]=in[i]-32; 
        else 
            out[i]=in[i];
    }
}
