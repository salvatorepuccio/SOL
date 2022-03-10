#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>

#define N 6

//pthread_mutex_t bacchette_mut[N];
pthread_cond_t bacchette_cond[N];
int bacchette[N];
pthread_mutex_t mtx=PTHREAD_MUTEX_INITIALIZER;


//forse il trucco e' nel cercare di acquisire prima la bacchetta sx e poi la dx
//e quando libero, liberare prima la bacchetta dx e poi la sx, al contrario 
//Questo perche' ci saranno sicuramente spesso filosofi che aspettano la dx per mangiare
//visto che tutti prendono prima la sx.



static void* routine_filo(void* arg){
    int pos = (int)arg;
    srand(time(NULL));
    int medita_tempo,mangia_tempo,dx,sx;
    

    dx = pos;
    sx = (pos+1)%N;

    for(int i=0;i<100;i++){
        medita_tempo = rand()%10;
        mangia_tempo = rand()%10;
        //meditare
        printf("[%d][%d] medito %d sec\n",pos,i,medita_tempo);
        sleep(medita_tempo);
        //acquisire bacchette
        pthread_mutex_lock(&mtx);
        printf("[%d][%d] Aspetto la bacchetta %d...",pos,i,sx);
        while(bacchette[sx]==1){//attendo la bacchetta a sx
            pthread_cond_wait(&bacchette_cond[sx],&mtx);
            fflush(stdout);
        }
        bacchette[sx]=1;//presa la bacchetta a sx
        printf("PRESA\n");
        printf("[%d][%d] Aspetto la bacchetta %d...",pos,i,dx);
        while(bacchette[dx]==1){//attendo la bacchetta a dx
            //bacchette[sx]=0;
            pthread_cond_wait(&bacchette_cond[dx],&mtx);
            fflush(stdout);
        }
        bacchette[dx]=1;//presa anche la bacchetta a dx
        printf("PRESA\n");
        printf("[%d][%d] Mangio per %d sec\n",pos,i,mangia_tempo);
        //mangiare
        sleep(mangia_tempo);
        bacchette[sx]=0;bacchette[dx]=0;
        printf("[%d][%d] Libero la bacchetta %d\n",pos,i,dx);
        pthread_cond_signal(&bacchette_cond[dx]);
        sleep(1);
        printf("[%d][%d] Libero la bacchetta %d\n",pos,i,sx);
        pthread_cond_signal(&bacchette_cond[sx]);  
        pthread_mutex_unlock(&mtx);  
    }
    return (void*)0;
}




int main(int argc, char *argv[]){
    if(argc!=1){perror("argomenti != 1");exit(EXIT_FAILURE);}
    pthread_t threads[N];
    int err,status[N];
    for(int i=0;i<N;i++) {
        //bacchette_mut[i]=PTHREAD_MUTEX_INITIALIZER;
        pthread_cond_init(&bacchette_cond[i],NULL);
        bacchette[i]=0;
    }
    
    for(int i=0;i<N;i++){
        if((err=pthread_create(&threads[i],NULL,&routine_filo,i))!=0){
            //gestisci errore
            perror("creazione filosofo");exit(EXIT_FAILURE);
        }
        else{
            sleep(1);
        }
    }

    for(int i=0;i<N;i++){
        pthread_join(threads[i],(void*) &status[i]);
    }

    return 0;
}