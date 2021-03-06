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

typedef struct {
	int connfd;
	int id;
}client;


void* client_holder(void* arg){
	char buff[MAX];
	int n;
	client *cli=arg;
	// infinite loop for chat
	for (;;) {
		bzero(buff, MAX);

		// read the message from client and copy it in buffer
		read(cli->connfd, buff, sizeof(buff));
		// print buffer which contains the client contents
		printf("From client %d: %s\t To client %d: ",cli->id ,buff,cli->id);
		bzero(buff, MAX);
		n = 0;
		// copy server message in the buffer
		while ((buff[n++] = getchar()) != '\n')
			;

		// and send that buffer to client
		write(cli->connfd, buff, sizeof(buff));

		// if msg contains "Exit" then server exit and chat ended.
		if (strncmp("exit", buff, 4) == 0) {
			printf("Server Exit...\n");
			break;
		}
	}
}



//Function designed for chat between client and server.
// void func(int connfd)
// {
// 	char buff[MAX];
// 	int n;
// 	// infinite loop for chat
// 	for (;;) {
// 		bzero(buff, MAX);

// 		// read the message from client and copy it in buffer
// 		read(connfd, buff, sizeof(buff));
// 		// print buffer which contains the client contents
// 		printf("From client: %s\t To client : ", buff);
// 		bzero(buff, MAX);
// 		n = 0;
// 		// copy server message in the buffer
// 		while ((buff[n++] = getchar()) != '\n')
// 			;

// 		// and send that buffer to client
// 		write(connfd, buff, sizeof(buff));

// 		// if msg contains "Exit" then server exit and chat ended.
// 		if (strncmp("exit", buff, 4) == 0) {
// 			printf("Server Exit...\n");
// 			break;
// 		}
// 	}
// }

// Driver function
int main()
{
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
		//func(connfd);

		client *c=malloc(sizeof(client*));
		c->connfd=connfd;
		c->id=c_id;

		if((err=pthread_create(&t1,NULL,&client_holder,c))!=0){
				//gestisci errore
				perror("creazione filosofo");exit(EXIT_FAILURE);
			}
		c_id++;
	}
	// After chatting close the socket
	close(accept_socket);
}
