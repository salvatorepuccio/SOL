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

#define SA struct sockaddr
#define SOCKETNAME "./mysocket"



void func(int sockfd){
	
	int n,dim=128,realdim=0;
    char *buff=malloc(dim),*temp=NULL,*buff2=NULL;

	for (;;) {
		bzero(buff, dim);
		printf("Inserisci la stringa : ");
		n = 0;
		while ((buff[n++] = getchar()) != '\n'){
            if(n>=dim){
                //allargo
				//printf("allargo\n");
                buff2=malloc(dim*2);
                strcpy(buff2,buff);
                temp = buff;
                buff = buff2;
                free(temp);temp=NULL;
                dim = dim*2;
            }
        }

		//printf("Hai scritto questo: %s\n",buff);


		realdim = strlen(buff);
		char realdim_char[4];
		sprintf(realdim_char,"%d",realdim);
		//printf("Mando la dimensione al server: %s\n",realdim_char);
		write(sockfd,realdim_char,4);

		//printf("Mando la stringa al server: %s\n");
		write(sockfd, buff, realdim);

        if (strncmp("quit", buff, 4) == 0) {
			printf("ESCO.\n");
			//exit(EXIT_FAILURE);
			break;
		}

		bzero(buff, dim);
		read(sockfd, buff, dim);
		printf("\nRisposta : %s\n", buff);
	}
}

int main()
{
	int client_socket;
	struct sockaddr_un servaddr, cli;

	system("clear");
	
	// socket create and verification
	client_socket = socket(AF_UNIX, SOCK_STREAM, 0);
	if (client_socket == -1) {
		printf("socket creation failed...\n");
		exit(0);
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
