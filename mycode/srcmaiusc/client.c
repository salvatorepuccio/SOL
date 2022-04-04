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
#include <pthread.h>
#include "stringsmanager.h"

#define SA struct sockaddr
#define SOCKETNAME "./mysocket"

typedef struct arg{
	int connfd;
	int *pipe;
}arg_t;

static volatile sig_atomic_t terminazione = 0;

static void sighandler (int sig) {
    if(sig == SIGINT){
		//printf("SIGINT\n");
		terminazione=1;
	}
}

























void *text_service(void* arg){

	int myconnfd = ((arg_t*)arg)->connfd;
	int *mypipe = ((arg_t*)arg)->pipe;
	int dim=80;

	fd_set current_sockets, read_ready_sockets;

	FD_ZERO(&current_sockets);
	FD_SET(mypipe[0],&current_sockets);
	FD_SET(STDIN_FILENO,&current_sockets);

	int ret_select,fd,res;
	int quitflag=0;

	//printf("Inserisci una stringa: ");

	for (;;) {
		read_ready_sockets=current_sockets;		
		
		if((ret_select=select(100,&read_ready_sockets,NULL,NULL,NULL))<0 | quitflag == 1) {
			if(quitflag == 1){
				break;
			}
			else{
				//errore della select
				perror("select");
				exit(EXIT_FAILURE);
			}
		}
		else{

			char *buffer=malloc(dim*sizeof(char));
			for (fd = 0;fd <= 100;fd++) {
				if(FD_ISSET(fd,&read_ready_sockets)) {

					//MSG DAL SERVER------------------------------------------------------
					if(fd == myconnfd){
						bzero(buffer,dim);
						if(read(myconnfd,buffer,dim) == 0){
							//sono arrivati 0 caratteri, server shutdown
							printf("dal server una stringa vuota\n");
							quitflag=1;
							break;
						}
						//strtok(buffer,"\n");
						printf("Risposta: %s\n",buffer);
						//printf("Inserisci una stringa: ");
						FD_SET(STDIN_FILENO,&current_sockets);
						dim=80;
						//printf("Ho reimpostato la dimensione del buffer\n");	
					}
					
					//-----------------------------INPUT DA TASTIERA---------------------------
					else if(fd == STDIN_FILENO){//input da tastiera
						printf("input da tastiera\n");
						//bzero(buffer,dim);
						if(read(STDIN_FILENO,buffer,dim) > 1){
							char *buffer2=NULL;
							if(strstr(buffer,"\n") == NULL){
								do{
									printf("buffer: '%s'\n");
									mystrcat(&buffer2,buffer,0,1);
									bzero(buffer,dim);
									read(STDIN_FILENO,buffer,dim);
								}while(strstr(buffer,"\n") == NULL);
								buffer=buffer2;
							}
							
							strtok(buffer,"\n");
							dim = strlen(buffer);
							char lench[4];
							sprintf(lench,"%d",dim);
							write(myconnfd,lench,4);
							write(myconnfd,buffer,dim);
							FD_SET(myconnfd,&current_sockets);
						}
					}

					//-------------------------------PIPE-------------------------------
					else if(fd == mypipe[0]){
						read(mypipe[0],buffer,80);
						quitflag=1;
						close(myconnfd);
						break;
					}
				}
			}
			free(buffer);
			if(quitflag==1)break;
		}
	}
	close(myconnfd);
	terminazione=1;
	return (void*)0;
}
























int main(){

	//CREAZIONE PIPE----------------------------------------------------------------
	int *mypipe=malloc(sizeof(int)*2);
	if(pipe(mypipe)==-1){
		perror("[S]crezione pipe");
		exit(EXIT_FAILURE);
	}
	long int v;errno=0;
	if((v=fpathconf(mypipe[0],_PC_PIPE_BUF))==-1){
		if(errno != 0){
			perror("limite sup pipe");
			exit(EXIT_FAILURE);
		}
		else printf("la pipe ha lim sup illimitato\n");
	}
	else printf("[S]lim sup pipe = %ld\n",v);
	//FINE----------------------------------------------------------------

	//SEGNALI----------------------------------------------------------------
	sigset_t mask,oldmask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGHUP);
    sigaddset(&mask, SIGQUIT);
	sigaddset(&mask, SIGTERM);

	if (pthread_sigmask(SIG_BLOCK, &mask, &oldmask) != 0) {//ignora i segnali sopra
        fprintf(stderr, "FATAL ERROR, pthread_sigmask\n");
        return EXIT_FAILURE;
    }

    struct sigaction sa;
    memset (&sa, 0, sizeof(sa));
    sa.sa_handler = sighandler;

	if (sigaction(SIGINT, &sa, NULL) ==-1)
        perror("sigaction SIGINT");
    if (sigaction(SIGHUP, &sa, NULL) ==-1)
        perror("sigaction SIGHUP");
    if (sigaction(SIGQUIT, &sa, NULL) ==-1)
        perror ("sigaction SIGQUIT");
	if (sigaction(SIGTERM, &sa, NULL) ==-1)
        perror ("sigaction SIGTERM");
	//FINE----------------------------------------------------------------

	if(pthread_sigmask(SIG_SETMASK, &oldmask, NULL) != 0) {//NON ignorare piu i segnali sopra
			fprintf (stderr, "FATAL ERROR\n");
			return EXIT_FAILURE;
		}

	int client_socket;
	struct sockaddr_un servaddr, cli;

	system("clear");
	
	// socket create and verification
	if((client_socket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
		perror("socket");
		exit(EXIT_FAILURE);
	}
	
	

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sun_family = AF_UNIX;
	strcpy(servaddr.sun_path,SOCKETNAME);

	//CONNESSIONE----------------------------------------------------------------
	if (connect(client_socket, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		perror("ERR connessione con il server\n");
		//unlink(SOCKETNAME);
		exit(EXIT_FAILURE);
	}
	printf("Connesso al server..\n");
	//----------------------------------------------------------------

	pthread_t tsTh;
	arg_t payload = {client_socket,mypipe};

	if(pthread_create(&tsTh,NULL,text_service,&payload)<0){
		perror("creating thread text service");
		exit(EXIT_FAILURE);
	}

	while(terminazione == 0){
		;
	}

	//dico al thread text_service di terminare
	write(mypipe[1],"EXIT",5);

	pthread_join(tsTh,NULL);

	close(client_socket);
	free(mypipe);
	//printf("FINE DEL CLIENT\n");
	return 0;
}
