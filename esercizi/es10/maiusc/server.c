//Realizzare un programma C che implementa un server che rimane sempre attivo in attesa di richieste da parte di 
//uno o piu' processi client su una socket di tipo AF_UNIX. Ogni client richiede al server la trasformazione di 
//tutti i caratteri di una stringa da minuscoli a maiuscoli (es. ciao –> CIAO). 
//Per ogni nuova connessione il server lancia un thread POSIX che gestisce tutte le richieste del client 
//(modello “un thread per connessione” – i thread sono spawnati in modalità detached) e quindi termina la sua 
//esecuzione quando il client chiude la connessione.
//Per testare il programma, lanciare piu' processi client ognuno dei quali invia una o piu' richieste al server multithreaded.


//MODIFICA: installare un signal handler, ignorare SIGPIPE e gestire SIGINT/QUIT/HUP/TERM con una terminazione graceful
//ovvero liberando la memoria allocata per non avere errori o memory leak
//testare tutto con valgrind --leak-cheack=full


#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
//nuovo
#include <sys/select.h>
#include <sys/un.h>
#include <errno.h>
#include "id.h"
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>

//COSA ABBIAMO CAPITO? che non è possibile riallocare dopo una read() da socket, e poi continuare la read
//infatti i dati non letti sono ormai andati persi, alla prima read() dobbiamo essere pronti sapendo quanti caratteri dovrò leggere

#define SA struct sockaddr
#define SOCKETNAME "./mysocket"

// typedef struct {
// 	int connfd;
// 	int id;
// }client;

Idgenerator_t *generator=NULL;

static volatile sig_atomic_t terminazione = 0;//se =1 dobbiamo terminare


static void sighandler (int sig) {
    if(sig == SIGPIPE){
		//non fare niente
	}
	else if(sig == SIGINT || sig == SIGHUP || sig == SIGQUIT || sig == SIGTERM){
		//gestisci graceful
		terminazione = 1;
		exit(EXIT_FAILURE);
	}
	else return;
}

void strtoupper(const char* in, size_t len, char* out){
    for(int i=0;i<len;i++){
        if(in[i]>96&&in[i]<123) 
            out[i]=in[i]-32; 
        else 
            out[i]=in[i];
    }
}


void* client_holder(void* arg){
    int n,toread,my_id;
	char size[4];
	//client *cli=arg;
	int connfd = (int)arg;
    char *upper=NULL,*buff=NULL;

	my_id=getid(generator);

	// infinite loop for chat
	for (;;) {
		
		// read the message from client and copy it in buffer
		read(connfd, size, 4);
		toread = atoi(size);
		//printf("Dimensione ricevuta: %d\n",toread);

		buff = malloc(toread);
		bzero(buff, toread);
        read(connfd, buff, toread);

		//printf("Stringa da client n %d: %s ",cli->id ,buff);

        if (strncmp("quit", buff, toread) == 0) {
			printf("Disconnetto client %d\n",my_id);
			//sleep(2);
			break;
		}

		if (strncmp("my_id", buff, toread) == 0) {
			printf("vuole sapere l'id\n");
			toread=sizeof(int);
			upper=malloc(toread);
			snprintf(upper, toread, "%d", my_id);
		}
		else{
			upper=malloc(toread);
        	strtoupper(buff,toread,upper);
		}

		//printf("\nMando a client n %d: %s ",cli->id ,upper);
		printf("Rispondo a %d\n",my_id);
		// and send that buffer to client
		write(connfd, upper, toread);
		free(upper);
	}
	return (void*)0;
}





int main(){

	//segnali
	sigset_t mask, oldmask;
    sigemptyset(&mask); // resetto tutti i bits
    sigaddset(&mask, SIGINT); // aggiunto SIGINT alla machera
    sigaddset(&mask, SIGHUP); // aggiunto SIGTSTP alla machera
	sigaddset(&mask, SIGQUIT);
	sigaddset(&mask, SIGTERM);

    // blocco i segnali SIGINT e SIGTSTP finche' non ho finito
    // l'installazione deli handler mi conservo la vecchia maschera
    if (sigprocmask(SIG_BLOCK, &mask, &oldmask) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }
    // installo un unico signal handler per tutti i segnali che mi inter
    struct sigaction sa;
    // resetto la struttura
    memset (&sa, 0, sizeof(sa));
    sa.sa_handler = sighandler;
    sigset_t handlermask;
    sigemptyset(&handlermask);
    sigaddset(&handlermask, SIGINT);
    sigaddset(&handlermask, SIGHUP);
    sigaddset(&handlermask, SIGQUIT);
	sigaddset(&handlermask, SIGTERM);
    sa.sa_mask = handlermask; 


    if (sigaction(SIGINT, &sa, NULL) ==-1)
        perror("sigaction SIGINT");
    if (sigaction(SIGHUP, &sa, NULL) ==-1)
        perror("sigaction SIGSTOP");
    if (sigaction(SIGQUIT, &sa, NULL) ==-1)
        perror ("sigaction SIGALRM");
	if (sigaction(SIGTERM, &sa, NULL) ==-1)
        perror ("sigaction SIGALRM");
    // if (system("clear")<0) 
    //     perror ("system");
    

	int accept_socket, connfd, len,err,status;
	struct sockaddr_un servaddr, cli;
	pthread_t t1;

	// socket create and verification
	accept_socket = socket(AF_UNIX, SOCK_STREAM, 0);
	if (accept_socket == -1) {
		perror("creazione di accept socket\n");
		unlink(SOCKETNAME);
		exit(EXIT_FAILURE);
	}

	bzero(&servaddr, sizeof(servaddr));

	servaddr.sun_family = AF_UNIX;
	strcpy(servaddr.sun_path,SOCKETNAME);

	// Binding newly created socket to given IP and verification
	if ((bind(accept_socket, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		perror("binding\n");
		unlink(SOCKETNAME);
		exit(EXIT_FAILURE);
	}

	generator=init_id();

	for(;;){
		if (sigsuspend(&oldmask) == -1 && errno != EINTR) {
            perror("sigsuspend");
            return (EXIT_FAILURE);
        }
		if(terminazione == 1){
			//terminare tutto
		}
		// Now server is ready to listen and verification
		if ((listen(accept_socket, 5)) != 0) {
			perror("listening\n");
			unlink(SOCKETNAME);
			exit(EXIT_FAILURE);
		}
		else
			printf("Server in ascolto..\n");
			
		socklen_t len = sizeof(cli);

		// Accept the data packet from client and verification
		connfd = accept(accept_socket, (SA*)&cli, &len);
		if (connfd < 0) {
			perror("accettazione di un nuovo client...\n");
			unlink(SOCKETNAME);
			exit(EXIT_FAILURE);
		}
		else
			printf("Nuovo client accettato\n");

		// Function for chatting between client and server
		//func(connfd);

		// client *c=malloc(sizeof(client*));
		// c->connfd=connfd;
		// c->id=c_id;

		if((err=pthread_create(&t1,NULL,&client_holder,connfd))!=0){
				//gestisci errore
				perror("creazione thread per gestire client\n");
				unlink(SOCKETNAME);
				exit(EXIT_FAILURE);
			}
		//c_id++;
	}
	// After chatting close the socket
	close(accept_socket);
	unlink(SOCKETNAME);
	return 0;
}