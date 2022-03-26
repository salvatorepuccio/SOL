#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <assert.h>
#include <sys/select.h>

typedef struct msg {
int len;
    char *str;
}msg_t;

// converte tutti i carattere minuscoli in maiuscoli
void toup(msg_t str){
    for(int i=0;i<str.len;i++){
        if(str.str[i]>96&&str.str[i]<123) 
            str.str[i]=str.str[i]-32; 
    }
}

//funzione eseguita dal Worker thread del pool
//gestisce una intera connessione di un client
 void threadF(void *arg) {
    assert(arg);
    long* args = (long*)arg;
    long connfd = args[0];
    long* termina = (long*)(args[1]);
    free(arg);
    fd_set set, tmpset;
    FD_ZERO(&set);
    FD_SET (connfd, &set);
    do {
        tmpset=set;

        int r;
        // ogni tanto controllo se devo terminare
        struct timeval timeout={0, 100000}; // 100 milliseconds
        if ((r=select (connfd+1, &tmpset, NULL, NULL, &timeout)) < 0) {
            perror("select");
            break;
        }
        if (r==0) {
            if (*termina) break;
            continue;
        }
        msg_t str;
        int n;
        if ((n=readn (connfd, &str. len, sizeof (int))) ==-1){
            perror("read1");
            break;
        }

        if (n==0) break;
        str.str = calloc((str. len), sizeof (char));
        if (!str. str) {
            perror("calloc");
            fprintf(stderr, "Memoria esaurita....\n");
            break;
        }
        if ((n=readn(connfd, str. str, str. len * sizeof (char))) ==-1) {
            perror("read2");
            free(str.str);
            break;
        }
        toup(str);
        if ((n=writen (connfd, &str. len, sizeof (int))) ==-1) {

            perror("write1");
            free(str.str);
            break;
        }
        if ((n=writen (connfd, str. str, str. len*sizeof (char))) ==-1){
            perror("write2");
            free(str.str);
            break;
        }
        free(str.str);
    }while(*termina==0);
    close(connfd);
}