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

#define SA struct sockaddr
#define SOCKETNAME "./mysocket"

typedef struct arg{
	int connfd;
	int *pipe;
}arg_t;

static volatile sig_atomic_t sigint = 0;

static void sighandler (int sig) {
    if(sig == SIGINT){
		//printf("SIGINT\n");
		sigint=1;
	}
}



char* enlarge(char* enlargeme,int dim){
	char *new_bigger;
	new_bigger=malloc(dim*2);
	strcpy(new_bigger,enlargeme);
	free(enlargeme);
	return new_bigger;
}

void *monitor_connection(void* arg){
	printf("[MONITOR] Attivo\n");
	int fd = ((arg_t*)arg)->connfd;
	int *mypipe = ((arg_t*)arg)->pipe;
	int termina=0,error,retval;
	socklen_t len = sizeof(error);

	while(termina==0){
		error=0;
		retval = getsockopt (fd, SOL_SOCKET, SO_ERROR, &error, &len);
		if (error != 0) {
			/* socket has a non zero error status */
			printf("[MONITOR] Socket chiusa lato server, spegniamoci\n");
			char *buffer = malloc(5);
			strncpy(buffer,"EXIT",5);
			write(mypipe[1],buffer,5);
			free(buffer);
			termina=1;
		}
	}
	printf("[MONITOR] exit\n");
	return (void*)0;
}

void *text_service(void* arg){

	int myconnfd = ((arg_t*)arg)->connfd;
	int *mypipe = ((arg_t*)arg)->pipe;
	int n,dim=80,realdim=0;
    char *buff=malloc(dim),*temp=NULL,*buff2=NULL;



	fd_set current_sockets, read_ready_sockets;

	FD_ZERO(&current_sockets);
	//FD_SET(myconnfd,&current_sockets);
	FD_SET(mypipe[0],&current_sockets);
	FD_SET(STDIN_FILENO,&current_sockets);
	int ret_select,fd;
	int quitflag=0;
	for (;;) {
		read_ready_sockets=current_sockets;		
		//printf("[CH]select....\n");
		if((ret_select=select(100,&read_ready_sockets,NULL,NULL,NULL))<0) {
			if(quitflag==0){
				perror("select");
				exit(EXIT_FAILURE);
			}
			else break;
		}
		else{
			for (fd = 0;fd <= 100;fd++) {
				if(FD_ISSET(fd,&read_ready_sockets)) {
					char *buffer=malloc(80);

					if(fd == myconnfd){//risposta dal server
						read(myconnfd,buffer,80);
						int l = strlen(buffer);
						printf("Risposta: '%s' len %d\n",buffer,l);
						sleep(1);
						FD_SET(STDIN_FILENO,&current_sockets);
					}

					if(fd == STDIN_FILENO){//input da tastiera
						read(STDIN_FILENO,buffer,80);
						printf("Hai inserito: '%s'\n",buffer);
						int len = strlen(buffer);
						char lench[4];
						sprintf(lench,"%d",len);
						write(myconnfd,lench,4);

						write(myconnfd,buffer,len);
						FD_SET(myconnfd,&current_sockets);
					}

					if(fd == mypipe[0]){//messaggio dal thread di monitoraggio connessione
						read(mypipe[0],buffer,80);
						printf("mypipe[0]: '%s'\n",buffer);
						quitflag=1;
						break;
					}
					free(buffer);
				}
			}
		}
		// bzero(buff, dim);
		// printf("Inserisci la stringa : ");
		// n = 0;
		// while ((buff[n++] = getchar()) != '\n'){
        //     if(n>=dim){
        //         //allargo
        //         buff = enlarge(buff,dim);
		// 		dim=dim*2;
        //     }
        // }
		// if(n==1){
		// 	//ha soltanto premuto invio senza scrivere nulla
		// 	//non mandare "\n" al server
		// 	continue;
		// }

		// //printf("n: %d\n",n);

		// printf("Hai scritto questo: %s\n",buff);

		// realdim = strlen(buff);
		// char realdim_char[4];
		// sprintf(realdim_char,"%d",realdim);
		// //printf("Mando la dimensione al server: %s\n",realdim_char);
		// //printf("AU\n");
		// write(sockfd,realdim_char,4);
		// strtok(buff,"\n");
		// //printf("Mando la stringa al server: %s\n");
		// write(sockfd, buff, realdim);

        // if (strncmp("quit", buff, 4) == 0) {
		// 	close(sockfd);
		// 	printf("ESCO per quit\n");
		// 	free(buff);
		// 	break;
		// }

		// bzero(buff, dim);
		// read(sockfd, buff, dim);
		// printf("\nRisposta : %s\n\n", buff);
	}
	printf("Fuori dal for\n");
	close(myconnfd);
	//printf("fine\n");
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
		perror("connessione con il server\n");
		unlink(SOCKETNAME);
		exit(EXIT_FAILURE);
	}
	printf("Connesso al server..\n");
	//----------------------------------------------------------------

	pthread_t tsTh,monTh;
	arg_t payload = {client_socket,mypipe};

	if(pthread_create(&tsTh,NULL,text_service,&payload)<0){
		perror("creating thread text service");
		exit(EXIT_FAILURE);
	}

	if(pthread_create(&monTh,NULL,monitor_connection,&payload)<0){
		perror("creating thread monitor");
		exit(EXIT_FAILURE);
	}

	pthread_join(tsTh,NULL);
	pthread_join(monTh,NULL);
	// close the socket
	close(client_socket);
	return 0;
}
