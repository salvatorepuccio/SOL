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
#include <sys/select.h>
#include <sys/un.h>
#include <errno.h>
#include "id.h"
#include <signal.h>
#include <sys/time.h>


#define SA struct sockaddr
#define SOCKETNAME "./mysocket"


Idgenerator_t *generator=NULL;

static volatile sig_atomic_t terminazione = 0;//se =1 dobbiamo terminare
static volatile sig_atomic_t sigpipe = 0;

//lista dei thread
	typedef struct Listath{
		pthread_t thread;
		int connfd;
		int status;
		struct Listath *next;
	}Listath_t;


static void sighandler (int sig) {
    if(sig == SIGPIPE){
		sigpipe = 1;
		printf("sigpipe\n");
	}
	else if(sig == SIGINT || sig == SIGHUP || sig == SIGQUIT || sig == SIGTERM){
		//gestisci graceful
		terminazione = 1;
		printf("sigint o hup o quit o term\n");
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

        if (strncmp("quit", buff, 4) == 0) {
			printf("Disconnetto client %d\n",my_id);
			free(buff);
			//free(size);
			break;
		}

		if (strncmp("my_id", buff, 5) == 0) {
			toread=17;
			//printf("toread = %d",toread);
			upper=malloc(toread);
			snprintf(upper, toread, "your id is: %d",my_id);
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
		free(buff);

		if(terminazione == 1){
			printf("[%d] break\n",my_id);
			break;
		}
	}

	return (void*)0;
}





int main(){

	//segnali
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
        perror("sigaction SIGHUP");
    if (sigaction(SIGQUIT, &sa, NULL) ==-1)
        perror ("sigaction SIGQUIT");
	if (sigaction(SIGTERM, &sa, NULL) ==-1)
        perror ("sigaction SIGTERM");
    if (system("clear")<0) 
        perror ("system");
    

	int accept_socket,connfd,len,err;
	struct sockaddr_un servaddr, cli;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sun_family = AF_UNIX;
	strcpy(servaddr.sun_path,SOCKETNAME);

	//SOCKET
	accept_socket = socket(AF_UNIX, SOCK_STREAM, 0);
	if (accept_socket == -1) {
		perror("creazione di accept socket\n");
		unlink(SOCKETNAME);
		exit(EXIT_FAILURE);
	}

	

	//BINDING
	if ((bind(accept_socket, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		perror("binding\n");
		unlink(SOCKETNAME);
		exit(EXIT_FAILURE);
	}

	generator=init_id();


	//lista dei thread che usero' (solo io)
	Listath_t *head=NULL,*current=head;

	for(;;){
		
		//LISTEN
		if ((listen(accept_socket, SOMAXCONN)) != 0) {
			perror("listening\n");
			unlink(SOCKETNAME);
			exit(EXIT_FAILURE);
		}
		else
			printf("Server in ascolto..\n");
			
		socklen_t len = sizeof(cli);

		//ACCEPT
		connfd = accept(accept_socket, (SA*)&cli, &len);//bloccante?
		if (connfd < 0) {
			if(terminazione == 1)
				break;
			else if(sigpipe == 1){
				//era un sigpipe, ignora
				sigpipe=0;
				continue;
			}	
			else{
				perror("accettazione di un nuovo client...\n");
				unlink(SOCKETNAME);
				exit(EXIT_FAILURE);
			}
		}
		else
			printf("Nuovo client accettato\n");


		//creare nuovo thread
		if(head==NULL){
			head = malloc(sizeof(Listath_t));
			printf("ho fatto una malloc\n");
			current = head;
		}
		else{
			printf("ho fatto una malloc\n");
			current->next = malloc(sizeof(Listath_t));
			current = current->next;
		}
		current->connfd=connfd;
		current->next=NULL;
		if((err=pthread_create(&current->thread,NULL,&client_holder,current->connfd))!=0){
				//gestisci errore
				perror("creazione thread per gestire client\n");
				unlink(SOCKETNAME);
				exit(EXIT_FAILURE);
		}
		else{
			printf("nuovo thread %d per client\n",connfd);
		}
	}


	if(terminazione == 1){
		if(head != NULL){
			Listath_t *tmp=NULL;
			int i=0;
			//se c'e' almeno un elemento
			printf("Elemento n: %d, connfd: %d\n",i,head->connfd);
			
			pthread_join(head->thread,(void*) &head->status);close(head->connfd);
			current=head;
			while(current->next != NULL){
				i++;
				tmp=current;
				current=current->next;
				free(tmp);
				printf("Elemento n: %d, connfd: %d\n",i,current->connfd);
				
				pthread_join(current->thread,(void*) &current->status);close(current->connfd);
			}
			free(current);
		}
		free(generator);
	}
	close(accept_socket);
	unlink(SOCKETNAME);
	return 0;
}