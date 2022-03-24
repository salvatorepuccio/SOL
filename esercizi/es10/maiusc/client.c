#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//nuovo
#include <sys/select.h>
#include <sys/un.h>
#include <errno.h>
#include <signal.h>

#define SA struct sockaddr
#define SOCKETNAME "./mysocket"

static volatile sig_atomic_t sigint = 0;

static void sighandler (int sig) {
    if(sig == SIGINT){
		//printf("SIGINT\n");
		sigint=1;
	}
	else{
		printf("[SH] Segnale non gestito: %s\n",sig);
	}
}



char* enlarge(char* enlargeme,int dim){
	char *new_bigger;
	new_bigger=malloc(dim*2);
	strcpy(new_bigger,enlargeme);
	free(enlargeme);
	return new_bigger;
}


void func(int sockfd){
	
	int n,dim=80,realdim=0;
    char *buff=malloc(dim),*temp=NULL,*buff2=NULL;

	for (;;) {
		bzero(buff, dim);
		printf("Inserisci la stringa : ");
		n = 0;
		while ((buff[n++] = getchar()) != '\n'){
            if(n>=dim){
                //allargo
                buff = enlarge(buff,dim);
				dim=dim*2;
            }
        }
		if(n==1){
			//ha soltanto premuto invio senza scrivere nulla
			//non mandare "\n" al server
			continue;
		}

		//printf("n: %d\n",n);

		printf("Hai scritto questo: %s\n",buff);

		realdim = strlen(buff);
		char realdim_char[4];
		sprintf(realdim_char,"%d",realdim);
		//printf("Mando la dimensione al server: %s\n",realdim_char);
		//printf("AU\n");
		write(sockfd,realdim_char,4);
		strtok(buff,"\n");
		//printf("Mando la stringa al server: %s\n");
		write(sockfd, buff, realdim);

        if (strncmp("quit", buff, 4) == 0) {
			close(sockfd);
			printf("ESCO per quit\n");
			free(buff);
			break;
		}

		bzero(buff, dim);
		read(sockfd, buff, dim);
		printf("\nRisposta : %s\n\n", buff);
	}
	//printf("fine\n");
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
    sa.sa_mask = handlermask; 

    if (sigaction(SIGINT, &sa, NULL) ==-1)
        perror("sigaction SIGINT");
   


	int client_socket;
	struct sockaddr_un servaddr, cli;

	system("clear");
	
	// socket create and verification
	client_socket = socket(AF_UNIX, SOCK_STREAM, 0);
	if (client_socket == -1) {
		printf("socket creation failed...\n");
		exit(EXIT_FAILURE);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sun_family = AF_UNIX;
	strcpy(servaddr.sun_path,SOCKETNAME);

	// connect the client socket to server socket
	if (connect(client_socket, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		perror("connessione con il server\n");
		unlink(SOCKETNAME);
		exit(EXIT_FAILURE);
	}
	else
		printf("Connesso al server..\n");

		
	// function for chat
	func(client_socket);

	// close the socket
	close(client_socket);
	return 0;
}
