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
#define SOCKETNAME "./mysock"

typedef struct msg {
    int len;
    char *str;
}msg_t;


char* enlarge(char* enlargeme,int dim){
	char *new_bigger;
	new_bigger=calloc(dim*2,sizeof(char));
	strcpy(new_bigger,enlargeme);
	free(enlargeme);
	return new_bigger;
}


void func(int sockfd){
	
	int n,realdim=0;
    char *input=NULL;
    msg_t str;

	for (;;) {
        str.len=80;
        input = calloc(str.len, sizeof(char));
        if(!input){perror("fatal error during calloc");exit(EXIT_FAILURE);}
		printf("Inserisci la stringa : ");
		n = 0;
		while ((input[n++] = getchar()) != '\n'){
            if(n>=str.len){
                input = enlarge(input,str.len);
                str.len=2*str.len;
            }
        }
	
		//printf("Hai scritto questo: %s\n",input);
		str.len = strlen(input)+1;
		// char length[4];
		// sprintf(length,"%d",str.len);
		//printf("Mando la dimensione al server: %s\n",realdim_char);
		write(sockfd,&str.len,4);
		//strtok(input,"\n");
		//printf("Mando la stringa al server: %s\n");
		write(sockfd, input, str.len);

        if (strncmp("quit", input, 4) == 0) {
			close(sockfd);
			printf("ESCO per quit\n");
			break;
		}

		bzero(input, str.len);
		read(sockfd, input, str.len);

		printf("\nRisposta : %s\n\n", input);
	}
    if(!input) free(input);
	//printf("fine\n");
}

int main(){

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

	servaddr.sun_family = AF_UNIX;
	strncpy(servaddr.sun_path,SOCKETNAME,strlen(SOCKETNAME)+1);

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
