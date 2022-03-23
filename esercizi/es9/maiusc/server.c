//Realizzare un programma C che implementa un server che rimane sempre attivo in attesa di richieste da parte di 
//uno o piu' processi client su una socket di tipo AF_UNIX. Ogni client richiede al server la trasformazione di 
//tutti i caratteri di una stringa da minuscoli a maiuscoli (es. ciao –> CIAO). 
//Per ogni nuova connessione il server lancia un thread POSIX che gestisce tutte le richieste del client 
//(modello “un thread per connessione” – i thread sono spawnati in modalità detached) e quindi termina la sua 
//esecuzione quando il client chiude la connessione.
//Per testare il programma, lanciare piu' processi client ognuno dei quali invia una o piu' richieste al server multithreaded.

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

//COSA ABBIAMO CAPITO? che non è possibile riallocare dopo una read() da socket, e poi continuare la read
//infatti i dati non letti sono ormai andati persi, alla prima read() dobbiamo essere pronti sapendo quanti caratteri dovrò leggere

#define SA struct sockaddr
#define SOCKETNAME "./mysocket"

typedef struct {
	int connfd;
	int id;
}client;



void strtoupper(const char* in, size_t len, char* out){
    for(int i=0;i<len;i++){
        if(in[i]>96&&in[i]<123) 
            out[i]=in[i]-32; 
        else 
            out[i]=in[i];
    }
}


void* client_holder(void* arg){
    int n,toread;
	char size[4];
	client *cli=arg;
    char *upper=NULL,*buff=NULL;
	// infinite loop for chat
	for (;;) {
		
		// read the message from client and copy it in buffer
		read(cli->connfd, size, 4);
		toread = atoi(size);
		//printf("Dimensione ricevuta: %d\n",toread);

		buff = malloc(toread);
		bzero(buff, toread);
        read(cli->connfd, buff, toread);

		//printf("Stringa da client n %d: %s ",cli->id ,buff);

        if (strncmp("quit", buff, 4) == 0) {
			printf("Disconnetto client %d\n",cli->id);
			//sleep(2);
			break;
		}

        upper=malloc(toread);
        strtoupper(buff,toread,upper);
		//printf("\nMando a client n %d: %s ",cli->id ,upper);
		printf("Rispondo..\n");

		// and send that buffer to client
		write(cli->connfd, upper, toread);
	}
	return (void*)0;
}





int main(){

	int accept_socket, connfd, len,err,status,c_id=0;
	struct sockaddr_un servaddr, cli;
	pthread_t t1;

	// socket create and verification
	accept_socket = socket(AF_UNIX, SOCK_STREAM, 0);
	if (accept_socket == -1) {
		perror("creazNASOCKETNAMEi;one di accetp socket\n");
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

	for(;;){
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
			printf("Nuovo client accettato %d\n",c_id);

		// Function for chatting between client and server
		//func(connfd);

		client *c=malloc(sizeof(client*));
		c->connfd=connfd;
		c->id=c_id;

		if((err=pthread_create(&t1,NULL,&client_holder,c))!=0){
				//gestisci errore
				perror("creazione thread");
				unlink(SOCKETNAME);
				exit(EXIT_FAILURE);
			}
		c_id++;
	}
	// After chatting close the socket
	close(accept_socket);
	unlink(SOCKETNAME);
	return 0;
}