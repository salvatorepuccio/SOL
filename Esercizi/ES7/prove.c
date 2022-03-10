#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
static int x;

// static void* myfun (void* arg){
//     while(x<(int)arg){
//         printf("Secondo thread: x=%d\n",x++);
//         sleep(1);
//     }
//     pthread_exit((void*)17);
// }

static void* rand_num_gen(void* arg){
    srand(time(NULL));
    sleep((rand()%5)+1);
    int r = rand()%100;
    printf("sono il thread %d e ho generato %d\n",arg,r);
    //pthread_exit((void*)r);
    return 0;
}


int main(){
    pthread_t tid[10];
    int err,status,i=0;

    // if((err=pthread_create(&tid,NULL, &myfun,(void*)4))!=0){
    //     //gestisci errore
    //     perror("creazione trhread");exit(EXIT_FAILURE);
    // }
    // else{
    //     //creato correttamente
    //     while(x<4){
    //         printf("Primo thread: x=%d\n",x++);
    //         sleep(1);
    //     }
    //     pthread_join(tid,(void*) &status);
    //     printf("Thread 2 ends: %d status\n",status);
    // }
    
    while(true){
        if((err=pthread_create(&tid[i],NULL, &rand_num_gen,(void*)i))!=0){
            //gestisci errore
            perror("creazione thread");exit(EXIT_FAILURE);
        }
        else{
            //creato correttamente
            printf("[] creato thread %d\n",i);
            i++;
            sleep(3);
            // printf("i prima di join: %d\n",i);
            // pthread_join(tid,(void*) &status);
            // printf("i dopo di join: %d\n",i);
            //printf("Thread %f ha generato: %d\n",tid,status);
        }
        if(i==10) break;
    }

    for(int j=0;j<10;j++){ pthread_join(tid[j],(void*) &status);printf("terminato\n");}


    return 0;
}