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

#define MAX 80
#define PORT 8080
#define SA struct sockaddr





//Function designed for chat between client and server.
void func(int connfd)
{
	char buff[MAX];
	int n;
	int fd_output;
	char *line=malloc(128);
    if( (fd_output = fopen("./output.txt","w+"))==NULL){
        perror("errore apertura file");exit(EXIT_FAILURE);
    }
	// infinite loop for chat
	for (;;) {
		bzero(buff, MAX);

		// read the message from client and copy it in buffer
		read(connfd, buff, sizeof(buff));
		// print buffer which contains the client contents
		printf("From client: %s\n ", buff);
	
		//controllare se e' arrivato 'quit'
		if (strncmp("quit", buff, 4) == 0) {
			printf("Chiudo connessione con questo client.\n");
			break;
		}
		else{
			//effettuo il calcolo
			printf("eseguo comando\n");
			buff[strcspn(buff, "\n")] = 0;
			char *command = malloc(sizeof(buff)+27);
			strcat(command,"echo \"");
			strcat(command,buff);
			strcat(command,"\" | bc > ./output.txt");
			printf("Sto per eseguire questo: %s\n",command);
			int res = system(command);
			//fprintf(fd_output,"\n");
			// if((line=gets(fd_output))==NULL){
			// 	perror("errore nella lettura del risultato dal file");
			// 	exit(EXIT_FAILURE);
			// }
			//ftruncate("./output.txt",0);
			//rispondere
			strcpy(line,"cazzzone\n");
			// and send that buffer to client
			printf("invio risposta\n");
			write(connfd, line, sizeof(line));
			bzero(line,sizeof(line));
			bzero(buff,MAX);
		}
		
	}
}

// Driver function
int main(){

	int accept_socket, connfd, len,err,status,c_id=0;
	struct sockaddr_in servaddr, cli;
	pthread_t t1;

	// socket create and verification
	accept_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (accept_socket == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	// Binding newly created socket to given IP and verification
	if ((bind(accept_socket, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

	for(;;){
		// Now server is ready to listen and verification
		if ((listen(accept_socket, 5)) != 0) {
			printf("Listen failed...\n");
			exit(0);
		}
		else
			printf("Server listening..\n");
		len = sizeof(cli);

		// Accept the data packet from client and verification
		connfd = accept(accept_socket, (SA*)&cli, &len);
		if (connfd < 0) {
			printf("server accept failed...\n");
			exit(0);
		}
		else
			printf("server accept the client...\n");

		// Function for chatting between client and server
		func(connfd);
	}
	// After chatting close the socket
	close(accept_socket);
}
