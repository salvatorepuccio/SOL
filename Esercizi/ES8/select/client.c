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

#define SOCKNAME "./mysock"
#define UNIX_PATH_MAX 108
#define PORT 9999
#define SA struct sockaddr

void func(int sockfd){
	
	int n,dim=128,realdim=0;
    char *buff=malloc(dim),*temp=NULL,*buff2=NULL;

	for (;;) {
		bzero(buff, dim);
		printf("Inserisci la stringa : ");
		n = 0;
		while ((buff[n++] = getchar()) != '\n'){
            // if(n>=dim){
            //     //allargo
			// 	//printf("allargo\n");
            //     buff2=malloc(dim*2);
            //     strcpy(buff2,buff);
            //     temp = buff;
            //     buff = buff2;
            //     free(temp);temp=NULL;
            //     dim = dim*2;
            // }
			;
        }

		printf("Mando: %s\n",buff);


		// realdim = strlen(buff);
		// char realdim_char[4];
		// sprintf(realdim_char,"%d",realdim);
		// //printf("Mando la dimensione al server: %s\n",realdim_char);
		// write(sockfd,realdim_char,4);

		//printf("Mando la stringa al server: %s\n");
		//write(sockfd, buff, realdim);
		write(sockfd, buff, dim);

        if (strncmp("quit", buff, 4) == 0) {
			printf("ESCO.\n");
			//exit(EXIT_FAILURE);
			break;
		}

		bzero(buff, dim);
		//read(sockfd, buff, dim);
		//printf("\nRisposta : %s\n", buff);
	}
}

int main(){


	int sockfd, connfd;
	struct sockaddr_un servaddr;
	
	// socket create and verification
	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sun_family = AF_UNIX;
	strcpy(servaddr.sun_path,SOCKNAME);
	//servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	//servaddr.sin_port = htons(PORT);


	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("connection with the server failed...\n");
		exit(0);
	}
	else
		printf("connected to the server..\n");

	func(sockfd);
	close(sockfd);

	return 0;
}

