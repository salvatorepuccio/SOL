#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "protos1.h"
 
const int REALLOC_INC=16;
 
// void RIALLOCA(char** buf, size_t newsize) {

//     if(*buf==NULL){
//         *buf = (char*)malloc(sizeof(char)*newsize);
//     }
//     else{
//         //char *nuovo = malloc(sizeof(char)*(size+REALLOC_INC));
//         *buf=(char*)realloc(*buf,sizeof(char)*newsize);
//     }
// }
 
int main(int argc, char *argv[]) {
    if (argc < 7) { printf("troppi pochi argomenti\n"); return -1; }
    char *buffer=NULL;
    RIALLOCA(&buffer, REALLOC_INC);  // macro che effettua l'allocazione del 'buffer'
    buffer[0]='\0'; // mi assicuro che il buffer contenga una stringa
    buffer = mystrcat(buffer, REALLOC_INC, argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], NULL);
    printf("%s\n", buffer);     
    //printf("%s\n", mystrcat(buffer, 16, "prima stringa", "seconda", "terza molto molto molto lunga", "quarta", "quinta lunga", "ultima!",NULL));
    free(buffer);
    return 0;
}




 