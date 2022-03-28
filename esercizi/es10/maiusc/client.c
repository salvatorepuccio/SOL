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

void enlarge(char** enlargeme,int dim){
	//printf("enlarge '%s'\n with size: %ld new size: %d",*enlargeme,strlen(*enlargeme),dim);
	char *new_bigger;
	new_bigger=malloc(dim);
	strncpy(new_bigger,*enlargeme,strlen(*enlargeme));
	//strtok(new_bigger,"\n");
	//free(enlargeme);
	*enlargeme = new_bigger;

}

int cpystore(char** paste, char* copy){
	int len;

	if(*paste == NULL){
		//printf("paste == NULL\n");
		len = strlen(copy)+80;
		*paste = malloc(sizeof(char)*len);
	}
	else{
		//printf("chiamo enlarge\n");
		len = strlen(*paste)+80;
		enlarge(paste,len);
		//printf("paste post enlarge: '%s' \n",*paste);
	}
	
	strncat(*paste,copy,strlen(copy));
	//printf("paste dopo: '%s' \n",*paste);
	return len;
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
		//printf("[CH]select....\n");
		
		if(sigint == 1 | (ret_select=select(100,&read_ready_sockets,NULL,NULL,NULL))<0) {
			if(sigint == 1){
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
					if(fd == myconnfd){//risposta dal server
						bzero(buffer,dim);
						if(read(myconnfd,buffer,dim) == 0){
							//sono arrivati 0 caratteri, server shutdown
							quitflag=1;
							break;
						}
						strtok(buffer,"\n");
						printf("Risposta: %s\n\n",buffer);
						//printf("Inserisci una stringa: ");
						FD_SET(STDIN_FILENO,&current_sockets);
						dim=80;
						//printf("Ho reimpostato la dimensione del buffer\n");	
					}

					if(fd == STDIN_FILENO){//input da tastiera

						bzero(buffer,dim);
						if(read(STDIN_FILENO,buffer,dim) > 1){
							if(strstr(buffer,"\n") == NULL){
								char *buffer2=NULL;
								do{
									//printf("un ciclo\n");
									cpystore(&buffer2,buffer);
									//printf("buffer2: %s\n",buffer2);
									//strtok(buffer2,"\n");
									bzero(buffer,dim);
									read(STDIN_FILENO,buffer,dim);
								}while(strstr(buffer,"\n") == NULL);
								cpystore(&buffer2,buffer);
								//free(buffer);
								buffer=buffer2;
								//printf("fine ciclo buffer2: %s\n",buffer2);
							}
							
						
							strtok(buffer,"\n");
							//printf("Hai inserito: '%s'\n",buffer);
							dim = strlen(buffer);
							//printf("len %d\n",dim);
							char lench[4];
							sprintf(lench,"%d",dim);
							//printf("scrivo lench %s\n",lench);
							write(myconnfd,lench,4);

							//printf("scrivo buffer");
							write(myconnfd,buffer,dim);
							FD_SET(myconnfd,&current_sockets);
						}
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
			if(quitflag==1)break;
		}
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

	// if(pthread_create(&monTh,NULL,monitor_connection,&payload)<0){
	// 	perror("creating thread monitor");
	// 	exit(EXIT_FAILURE);
	// }

	pthread_join(tsTh,NULL);
	//pthread_join(monTh,NULL);

	close(client_socket);
	unlink(SOCKETNAME);
	printf("FINE DEL CLIENT\n");
	return 0;
}
