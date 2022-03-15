//esempio dalle slide di server multi connessione con select

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


#define SOCKNAME "./mysock"
#define N 100
#define UNIX_PATH_MAX 108

struct sockaddr_un {
	sa_family_t sun_family;
	char sun_path[UNIX_PATH_MAX];
};

// #define PORT 8080
// #define SA struct sockaddr

// typedef struct {
// 	int connfd;
// 	int id;
// }client;


static void run_server(struct sockaddr * psa) {
	int fd_sk, fd_c, fd_num=0, fd;
	char buf[N]; 
	fd_set set, rdset; 
	int nread;
	fd_sk=socket(AF_UNIX,SOCK_STREAM,0);
	bind(fd_sk,(struct sockaddr *)psa,sizeof(*psa));
	listen(fd_sk,SOMAXCONN);
	/* mantengo il massimo indice di descrittore
	attivo in fd_num */
	if (fd_sk > fd_num) fd_num = fd_sk;
	FD_ZERO(&set);
	FD_SET(fd_sk,&set); 
	while (true) {
		rdset=set;
		if (select(fd_num+1,&rdset,NULL,NULL,NULL)==-1) {/* gest errore */ }
		else { /* select OK */
			for (fd = 0; fd<=fd_num;fd++) {
				if (FD_ISSET(fd,&rdset)) {
					if (fd== fd_sk) {/* sock connect pronto */
						fd_c=accept(fd_sk,NULL,0);
						FD_SET(fd_c, &set);
						if (fd_c > fd_num) fd_num = fd_c; 
					}
					else {/* sock I/0 pronto */
						nread=read(fd,buf,N);
						if (nread==0) {/* EOF client finito */
							FD_CLR(fd,&set);
							fd_num=aggiorna(&set);
							close(fd); 
						}
						else { /* nread !=0 */
							printf("Server got: %s\n",buf) ;
							write(fd,"Bye!",5);
						}
					} 
				} 
			} 
		} 
	} 
}




static void run_client(struct sockaddr * psa) {
	if (fork()==0) { /* figlio, client */
		int fd_skt; char buf[N];
		fd_skt=socket(AF_UNIX,SOCK_STREAM,0);
		while (connect(fd_skt,(struct sockaddr*)psa,sizeof(*psa)) == -1 ) {
			if (errno == ENOENT) sleep(1);
			else exit(EXIT_FAILURE); 
		}
		write(fd_skt,"Hallo!",7);
		read(fd_skt,buf,N);
		printf("Client got: %s\n",buf);
		close(fd_skt);
		exit(EXIT_SUCCESS);
	}
} /* figlio terminato */



int main (void){
	int i; 
	struct sockaddr_un sa;
	sa.sun_family=AF_UNIX;
	for(i=0; i< 4; i++)
	run_client(&sa); /* attiv client*/
	run_server (&sa); /* attiv server */
	return 0;
}

// void strtoupper(const char* in, size_t len, char* out){
//     for(int i=0;i<len;i++){
//         if(in[i]>96&&in[i]<123) 
//             out[i]=in[i]-32; 
//         else 
//             out[i]=in[i];
//     }
// }


// void* client_holder(void* arg){
//     int n,toread;
// 	char size[4];
// 	client *cli=arg;
//     char *upper=NULL,*buff=NULL;
// 	// infinite loop for chat
// 	for (;;) {
		
// 		// read the message from client and copy it in buffer
// 		read(cli->connfd, size, 4);
// 		toread = atoi(size);
// 		//printf("Dimensione ricevuta: %d\n",toread);

// 		buff = malloc(toread);
// 		bzero(buff, toread);
//         read(cli->connfd, buff, toread);

// 		//printf("Stringa da client n %d: %s ",cli->id ,buff);

//         if (strncmp("quit", buff, 4) == 0) {
// 			printf("Disconnetto client %d\n",cli->id);
// 			//sleep(2);
// 			break;
// 		}

//         upper=malloc(toread);
//         strtoupper(buff,toread,upper);
// 		//printf("\nMando a client n %d: %s ",cli->id ,upper);
// 		printf("Rispondo..\n");

// 		// and send that buffer to client
// 		write(cli->connfd, upper, toread);
// 	}
// 	return (void*)0;
// }





// int main()
// {
// 	int accept_socket, connfd, len,err,status,c_id=0;
// 	struct sockaddr_in servaddr, cli;
// 	pthread_t t1;

// 	// socket create and verification
// 	accept_socket = socket(AF_INET, SOCK_STREAM, 0);
// 	if (accept_socket == -1) {
// 		printf("socket creation failed...\n");
// 		exit(0);
// 	}
// 	else
// 		printf("Socket successfully created..\n");
// 	bzero(&servaddr, sizeof(servaddr));

// 	// assign IP, PORT
// 	servaddr.sin_family = AF_INET;
// 	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
// 	servaddr.sin_port = htons(PORT);

// 	// Binding newly created socket to given IP and verification
// 	if ((bind(accept_socket, (SA*)&servaddr, sizeof(servaddr))) != 0) {
// 		printf("socket bind failed...\n");
// 		exit(0);
// 	}
// 	else
// 		printf("Socket successfully binded..\n");

// 	for(;;){
// 		// Now server is ready to listen and verification
// 		if ((listen(accept_socket, 5)) != 0) {
// 			printf("Listen failed...\n");
// 			exit(0);
// 		}
// 		else
// 			printf("Server listening..\n");
// 		len = sizeof(cli);

// 		// Accept the data packet from client and verification
// 		connfd = accept(accept_socket, (SA*)&cli, &len);
// 		if (connfd < 0) {
// 			printf("server accept failed...\n");
// 			exit(0);
// 		}
// 		else
// 			printf("server accept the client %d\n",c_id);

// 		// Function for chatting between client and server
// 		//func(connfd);

// 		client *c=malloc(sizeof(client*));
// 		c->connfd=connfd;
// 		c->id=c_id;

// 		if((err=pthread_create(&t1,NULL,&client_holder,c))!=0){
// 				//gestisci errore
// 				perror("creazione thread");exit(EXIT_FAILURE);
// 			}
// 		c_id++;
// 	}
// 	// After chatting close the socket
// 	close(accept_socket);
//}