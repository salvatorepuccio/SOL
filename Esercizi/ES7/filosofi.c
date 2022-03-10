#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>




int main(int argc, char *argv[]){
    if(argc!=2){perror("argomenti != 2");exit(EXIT_FAILURE);}
    int N = atoi(argv[1]);

    pthread_mutex_t bacchette_mut[N];
    pthread_cond_t bacchette_cond[N];

    for(int i=0;i<N;i++) {
        bacchette_mut[i]=PTHREAD_MUTEX_INITIALIZER;
        bacchette_cond[i]=PTHREAD_COND_INITIALIZER;
    }

    


}