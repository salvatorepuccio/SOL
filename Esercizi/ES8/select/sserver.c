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
#define N 128
#define SA struct sockaddr


void strtoupper(const char* in, size_t len, char* out){
    for(int i=0;i<len;i++){
        if(in[i]>96&&in[i]<123) 
            out[i]=in[i]-32; 
        else 
            out[i]=in[i];
    }
}



int main (){

	struct sockaddr_un cli,serv_sock_address;
	int server_socket, client_socket, fd_num=0, fd,i;
	char buf[N], *upper=malloc(N); 
	fd_set current_sockets, read_ready_sockets, write_ready_sockets; 
	int nread,ret_select=0;

	server_socket=socket(AF_UNIX,SOCK_STREAM,0);
	if(server_socket == -1){
		printf("creazione socket fallita..\n");
		exit(EXIT_FAILURE);
	}
	else{
		printf("socket creata\n");
	}

	bzero(&serv_sock_address,sizeof(serv_sock_address));
	
	serv_sock_address.sun_family = AF_UNIX;
	strcpy(serv_sock_address.sun_path,SOCKNAME);

	if((bind(server_socket,(SA*)&serv_sock_address,sizeof(serv_sock_address))) !=0){
		printf("socket bind fallita...\n");
		exit(EXIT_FAILURE);
	}
	else{
		printf("socket bind riuscita\n");
	}

	if(listen(server_socket,SOMAXCONN)!=0){
		printf("listen fallita...\n");
		exit(EXIT_FAILURE);
	}
	else{
		printf("server in ascolto...\n");
	}
	
	if (server_socket > fd_num) fd_num = server_socket;

	FD_ZERO(&current_sockets);
	FD_SET(server_socket,&current_sockets); 

	while (true) {
		
		read_ready_sockets=current_sockets;
		write_ready_sockets=current_sockets;

		//printf("fd_num: %d, fd_sk: %d\n",fd_num,fd_sk);

		if ((ret_select=select(FD_SETSIZE,&read_ready_sockets,&write_ready_sockets,NULL,NULL))<0) {
			perror("select");
			exit(EXIT_FAILURE); 
		}
		else { /* select OK */
			//printf("ELSE: select a buon fine: %d\n",ret_select);
			for (fd = 0; fd<=FD_SETSIZE;fd++) {
				if (FD_ISSET(fd,&read_ready_sockets)) {
					if (fd== server_socket) {
						//nuova connessione da accettare
						int len = sizeof cli;
						client_socket=accept(server_socket,(SA*)&cli,&len);
						FD_SET(client_socket, &current_sockets);
					}
					else {/* sock I/0 pronto */
						read(fd,buf,N);
						printf("Ricevuto: %s\n",buf);
						if(strncmp("quit",buf,4)==0){
							FD_CLR(fd,&current_sockets);
							close(fd);
						}
						else{
							FD_SET(fd,&write_ready_sockets);
							strtoupper(buf,N,upper);
						}
					} 
				}
				else if(FD_ISSET(fd,&write_ready_sockets)){
					write(fd,upper,N);
				}
			} 
		} 
	} 

	return 0;
}
