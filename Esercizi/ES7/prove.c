#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

static int x;

static void* myfun (void* arg){
    while(true){
        printf("Secondo thread: x=%d\n",x++);
        sleep(1);
    }
}


int main(){
    pthread_t tid;
    int err;

    if(err=pthread_create(&tid,NULL, &myfun,NULL)!=0){
        //gestisci errore
        perror("creazione trhread");exit(EXIT_FAILURE);
    }
    else{
        //creato correttamente
        while(true){
            printf("Primo thread: x=%d\n",x++);
            sleep(1);
        }
    }
}