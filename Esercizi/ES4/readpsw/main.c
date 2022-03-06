//aprire il file delle password e leggere tutti i login name
// e scriverli in un fileX

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

//modifica fatta da T4302

int main(int argc, char **argv){
    FILE *ifp=NULL, *ofp=NULL;
    ifp = fopen("/etc/passwd","r");
    if(ifp==NULL){printf("Impossibile aprire il file /etc/passwd\n");return -1;}
    ofp = fopen("./passwd.txt","w+");
    if(ofp==NULL){printf("Impossibile aprire o creare il file ./passwd.txt\n");return -1;}
    char *line,*tok;
    line=malloc(300);

    //ciclo
    while(fgets(line,300,ifp)!=NULL){
        tok = strchr(line,':');
        //printf("line: %s",line);
        *tok = '\0';
        printf("line dopo: %s\n",line);
        strcat(line,"\n");
        fputs(line,ofp);
        
    }
//cambiamento fatto da RINGHIOO
    //free(tok);
    free(line);
    fclose(ifp);
    fclose(ofp);

    printf("FINE\n");

    return 0;
    
}