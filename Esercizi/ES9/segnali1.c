#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <unistd.h>
//Scrivere un programma C che conta il numero di volte in cui l'utente invia il segnale 
//SIGINT (Ctl-C) al processo. Quando il processo riceve il segnale SIGTSTP (Ctl-Z), 
//il numero di SIGINT ricevuti viene stampato sullo standard output. Al terzo segnale SIGTSTP, 
//il processo chiede all'utente se deve terminare o no attendendo una risposta per 10 secondi dallo 
//standard input. Se l'utente non risponde entro 10 secondi un segnale SIGALRM termina il processo.

//i segnali sono diversi su macos rispetto a linux
//attenzione

//glob vars
int count_sigint=0;
int count_sigtstp=0;

static void* exit_prompt(void* arg){
    printf("Tra 10 secondi il programma si chiudera\'\n");
    alarm(10);
    return (void*)0;
}


//handler di sigtstp ctrl-z
static void handler (int code) {
    //printf("code: %d\n",code);

    switch(code){

        case SIGINT:
            count_sigint++;
            break;

        case SIGTSTP:
            printf("\nSIGINT fino ad ora: %d\n",count_sigint);
            count_sigtstp++;
            if(count_sigtstp>=3){
                //chiedi se terminare
                int err,status;
                pthread_t tid;
                if((err=pthread_create(&tid,NULL,&exit_prompt,NULL))!=0){
                    //gestisci errore
                    perror("creazione exit prompt thread");exit(EXIT_FAILURE);
                }
                char risposta;
                printf("Premi invio per continuare a usare\n");
                scanf("%c",&risposta);
                //se sono qui ha premuto invio sicuro, scanf bloccante
                printf("OK\n");
                alarm(0);            
            }
            break;

        default:
            break;

    }//switch
    
    
} 

int main (void) {
    struct sigaction action; 
    

    //imposto gestore di SIGINT
    memset(&action, 0, sizeof(action));
    action.sa_handler=handler; 

    
    sigaction(SIGINT,&action,NULL);
    sigaction(SIGTSTP,&action,NULL);

    printf("Ciclo infinito...\n");
    for (;;) 
        ;
    
    return 0;
}