#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
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
int flag=0;

static void* exit_prompt(void* arg){
    printf("Tra 10 secondi il programma si chiudera\'\n");
    alarm(10);
    return (void*)0;
}

//handler di sigtstp ctrl-z
static void handler (int code) {
    //printf("code: %d\n",code);
    if(flag==0){
        switch(code){
            case SIGINT:
                count_sigint++;
                break;
            case SIGTSTP:
                printf("\n[%d]SIGINT fino ad ora: %d\n",getpid(),count_sigint);
                count_sigtstp++;
                break;
            default:
                break;
        }//switch
    }
    
} 

int main (void) {
    sigset_t mask, oldmask;
    //creo una mask che evidenzia SIGINT e SIGTSTP
    // sigemptyset(&mask);
    // sigaddset(&mask, SIGINT); 
    // sigaddset (&mask, SIGTSTP);
    //cambia la maschera attuale (che non cponosco) con la mia
    //quindi bloccandd ogni SIGINT e SIGTSTP
    //in oldmask mi tengo la maschera attuale
    //sigprocmask(SIG_BLOCK, &mask, &oldmask);

    //creo una maschera che evidenzia questi 3 SIG
    sigset_t handlermask;
    sigemptyset(&handlermask);
    sigaddset(&handlermask, SIGINT);
    sigaddset(&handlermask, SIGTSTP);
    //sigaddset(&handlermask, SIGALRM);
    
    //imposta la maschera dell'handler, cosi reagira' a questi 3 SIG
    struct sigaction sa;
    sa.sa_mask = handlermask;

    //imposto gestore di SIGINT
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler=handler; 

    //non ho capito a che serve
    sigaction(SIGINT,&sa,NULL);
    sigaction(SIGTSTP,&sa,NULL);
    //sigaction(SIGALRM,&sa,NULL);

    printf("Ciclo infinito...\n");
    for (;;){ 
        //sigsuspend(&oldmask);//imposta la maschera vecchia
        if(count_sigtstp>=3){
            flag=1;
            char c;
            printf("Chiudo tra 10 secondi, premi invio per evitarlo\n");
            alarm(10);
            scanf("%c",&c);
            alarm(0);
            system("clear");
            printf("[%d]continuiamo..\n",getpid());
            count_sigint=0;
            count_sigtstp=0;
            flag=0;
        }      
    }
        
    
    return 0;
}