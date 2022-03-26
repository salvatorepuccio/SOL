#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

typedef struct threadPayload{
    int myid;
    int *mypipe;
}threadPayload_t;


static volatile sig_atomic_t terminazione = 0;//se =1 dobbiamo terminare
static void sighandler (int sig) {
	if(sig == SIGINT){
        printf("[***SIGNAL HANDLER***] ctrl-c intercettato\n");
		terminazione = 1;
	}
}





void *func(void* arg){
    int id = ((threadPayload_t*)arg)->myid;
    int *pipe = ((threadPayload_t*)arg)->mypipe;
    
    char *buffer=malloc(sizeof(char)*80);
    strcat(buffer,"Ciao sono tuo figlio.");
    int len = strlen(buffer);
    //sleep(3);
    printf("[%d]Scrivo su mypipe[1] '%s' , len: %d\n",getpid(),buffer,len);
    write(pipe[1],buffer,len);

    bzero(buffer,NULL);
    strcat(buffer,"Seconda stringa.");
    int len = strlen(buffer);

    printf("[%d]Scrivo su mypipe[1] '%s' , len: %d\n",getpid(),buffer,len);
    write(pipe[1],buffer,len);

    printf("[%d]FATTO.\n\n",getpid());
    return (void*)0;
}


int main(){

    sigset_t mask,oldmask;
    sigemptyset(&mask);
    sigaddset(&mask,SIGINT);
    sigaddset(&mask, SIGQUIT);
    sigaddset(&mask, SIGTERM);
    sigaddset(&mask, SIGHUP);

    if (pthread_sigmask(SIG_BLOCK, &mask, &oldmask) != 0) {//ignora i segnali sopra
        fprintf(stderr, "FATAL ERROR, pthread_sigmask\n");
        return EXIT_FAILURE;
    }

    // installo il signal handler per tutti i segnali che mi interessano
    struct sigaction sa;
    // resetto la struttura
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sighandler;
    if (sigaction(SIGINT, &sa, NULL) ==-1){
        perror("sigaction SIGINT");
        exit(EXIT_FAILURE);
    }

    if(pthread_sigmask(SIG_SETMASK, &oldmask, NULL) != 0) {//NON ignorare piu i segnali sopra
        fprintf (stderr, "FATAL ERROR\n");
        return EXIT_FAILURE;
    }

    int *mypipe=malloc(sizeof(int)*2);
    pthread_t myThread;
    char *buffer=NULL;
    if(pipe(mypipe)<0){
        perror("[MAIN]Creazione pipe");
        exit(EXIT_FAILURE);
    }

    threadPayload_t payload = {0,mypipe};
    
    printf("[MAIN]Creo un thread\n");
    if(pthread_create(&myThread,NULL,&func,&payload)!=0){
        perror("creazione thread\n");
        exit(EXIT_FAILURE);
    }
    buffer = malloc(sizeof(char)*10);
    sleep(3);
    printf("[MAIN]In attesa su pipe[0]\n");
    read(mypipe[0],buffer,80);
    printf("[MAIN]Ho letto da mypipe[0]: '%s'\n",buffer);

    bzero(buffer,NULL);

    read(mypipe[0],buffer,80);
    printf("[MAIN]Ho letto da mypipe[0]: '%s'\n",buffer);

    pthread_join(myThread,NULL);

    return 0;

}