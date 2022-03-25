#include <stdio.h>
#include<stdlib.h>
#include <strings.h>

int main(){
    FILE *bin,*txt;
    bin = fopen("./text.txt","w+");
    if(bin==NULL){printf("Impossibile aprire il file\n");return -1;}
    

    int length = sizeof("https://static7.hentai-cosplays.com/upload/20220228/291/297183/1.jpg");
    length = length+10;
    printf("length: %d\n",length);
    
    char *b = malloc(length);
    char num[4];

    int i=1;
    do{
        printf("azzero b\n");
        bzero(b,length);
        printf("copio il link\n");
        strcpy(b,"https://static7.hentai-cosplays.com/upload/20220228/291/297183/");
        printf("preparo num\n");
        snprintf(num,4,"%d",i);
        printf("strcatto num\n");
        strcat(b,num);
        printf("strcatto .jpg\n");
        strcat(b,".jpg\n");
        printf("scrivo '%s'\n",b);
        fprintf(bin,"%s",b);
        i++;
    }while(i<26);
    fclose(bin);
    return 0;
}