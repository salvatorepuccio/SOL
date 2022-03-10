#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>

int X=0;

pthread_mutex_t mtx=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ho_prodotto=PTHREAD_COND_INITIALIZER;
pthread_cond_t ho_consumato=PTHREAD_COND_INITIALIZER;

static void* my_prod(void* arg){
    srand(time(NULL));
    int n_prodotti=0;
    while(true) {
        int x = rand()%100;
        if(x<1)x=x+x+1;

        pthread_mutex_lock(&mtx);
        while(X!=0){
            pthread_cond_wait(&ho_consumato, &mtx);
            fflush(stdout);
        }
        printf("[P] Ho spazio? produco %d\n",x);
        X=x;
        pthread_cond_signal(&ho_prodotto);
        pthread_mutex_unlock(&mtx);
        n_prodotti++;
        if(n_prodotti==atoi(arg))break;
        sleep(rand()%5);
    }
    return (void*)0;
}

static void* my_cons(void* arg){
    int n_consumati=0;
    while(true) {
        pthread_mutex_lock(&mtx);
        while (X == 0){
            pthread_cond_wait(&ho_prodotto, &mtx);
            fflush(stdout);
        }
        printf("[C] C'e' prodotto? Consumo %d\n",X); 
        X=0;
        pthread_cond_signal(&ho_consumato);
        pthread_mutex_unlock(&mtx);
        n_consumati++;
        if(n_consumati==atoi(arg))break;
        sleep(rand()%5);
    }
        return (void*)0;
}

int main(int argc, char *argv[]){
    if(argc!=2){perror("inserire quanti elementi produrre");exit(EXIT_FAILURE);}
    pthread_t cons,prod;
    int err1,err2,status1,status2;

    if((err1=pthread_create(&prod,NULL,&my_prod,argv[1]))!=0){
        //gestisci errore
        perror("creazione produttore");exit(EXIT_FAILURE);
    }
    else{
        //creato correttamente
        printf("[] creato produttore\n");
    }

    //
    if((err2=pthread_create(&cons,NULL,&my_cons,argv[1]))!=0){
        //gestisci errore
        perror("creazione consumatore");exit(EXIT_FAILURE);
    }
    else{
        //creato correttamente
        printf("[] creato consumatore\n");
    }
    //sleep(10);
    pthread_join(prod,(void*) &status1);
    pthread_join(cons,(void*) &status2);
    return 0;


}

