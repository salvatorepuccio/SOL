//Realizzare un programma C che implementa un server che rimane sempre attivo in attesa di richieste da parte di 
//uno o piu' processi client su una socket di tipo AF_UNIX. Ogni client richiede al server la trasformazione di 
//tutti i caratteri di una stringa da minuscoli a maiuscoli (es. ciao –> CIAO). 
//Per ogni nuova connessione il server lancia un thread POSIX che gestisce tutte le richieste del client 
//(modello “un thread per connessione” – i thread sono spawnati in modalità detached) e quindi termina la sua 
//esecuzione quando il client chiude la connessione.
//Per testare il programma, lanciare piu' processi client ognuno dei quali invia una o piu' richieste al server multithreaded.

//MODIFICA: installare un signal handler, ignorare SIGPIPE e gestire SIGINT/QUIT/HUP/TERM con una terminazione graceful
//ovvero liberando la memoria allocata per non avere errori o memory leak
//testare tutto con valgrind --leak-cheack=full

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
#include <sys/select.h>
#include <sys/un.h>
#include <errno.h>
#include "id.h"
#include <signal.h>
#include <sys/time.h>

#define SA struct sockaddr
#define SOCKETNAME "./mysocket"


Idgenerator_t *generator=NULL;


//lista dei thread
typedef struct Listath{
	pthread_t thread;
	int connfd;
	int status;
	struct Listath *next;
}Listath_t;


typedef struct arg{
	int connfd;
	int *pipe;
}arg_t;













static volatile sig_atomic_t terminazione = 0;//se =1 dobbiamo terminare
static void sighandler (int sig) {
	if(sig == SIGINT || sig == SIGHUP || sig == SIGQUIT || sig == SIGTERM){
		printf("[***SIGHANDLER***] ctrl-c intercettato\n");
		terminazione = 1;
	}
}














void strtoupper(const char* in, size_t len, char* out){
    for(int i=0;i<len;i++){
        if(in[i]>96&&in[i]<123) 
            out[i]=in[i]-32; 
        else 
            out[i]=in[i];
    }
}















void* client_holder(void* arg){
	printf("client iniziato\n");
    int n,toread=-1,my_id,quitflag=0;
	char size[4];

	int myconnfd = ((arg_t*)arg)->connfd;
	int *mypipe = ((arg_t*)arg)->pipe;

	my_id=getid(generator);

	fd_set current_sockets, read_ready_sockets;

	FD_ZERO(&current_sockets);
	FD_SET(myconnfd,&current_sockets);
	FD_SET(mypipe[0],&current_sockets);
	int ret_select,fd;

	for (;;) {
		read_ready_sockets=current_sockets;		
		//printf("[CH]select....\n");
		if(quitflag == 1 | (ret_select=select(100,&read_ready_sockets,NULL,NULL,NULL))<0) {
			if(quitflag==1){
				//uscita prevista
				break;
			}
			else{
				//errore non previsto
				perror("select client");
				exit(EXIT_FAILURE);
			}
		}
		else { /* select OK */
			//printf("[CH]select a buon fine\n");
			for (fd = 0;fd <= 100;fd++) {
				if(FD_ISSET(fd,&read_ready_sockets)) {
					if(fd == myconnfd){//richiesta da servire
						if(toread==-1){
							//devo leggere la dimensione
							read(myconnfd, size, 4);
							toread = atoi(size);
							printf("[CH] Dimensione del prossimo messaggio: %d\n",toread);
							FD_SET(myconnfd,&current_sockets);
						}
						else {
							//devo leggere la stringa
							char *buff = malloc(toread);
							bzero(buff, toread);
							read(myconnfd, buff, toread);
							printf("[CH] ricevuto: '%s'\n",buff);
							if (strncmp("quit", buff, 4) == 0) {
								printf("Disconnetto client %d\n",my_id);
								free(buff);
								quitflag=1;
								break;
							}

							//non era un quit, allora rispondo
							char *upper=NULL;
							if (strncmp("my_id", buff, 5) == 0) {
								toread=17;
								upper=malloc(toread);
								snprintf(upper, toread, "your id is: %d",my_id);
							}
							else{
								upper=malloc(toread);
								strtoupper(buff,toread,upper);
							}
							printf("[CH]Mando '%s' a %d\n",upper,my_id);
							write(myconnfd, upper, toread);
							free(upper);
							toread=-1;
							FD_SET(myconnfd,&current_sockets);
						}		
					}

					else if(fd == mypipe[0]){
						//dobbiamo terminare
						char *buff = malloc(sizeof(char)*5);
						read(mypipe[0],buff,5);
						printf("[thread]select legge da pipe[1] '%s'\n",buff);
						if(strncmp(buff,"EXIT",5) == 0){
							printf("[thread] Il Main mi ha detto di uscire\n");
							quitflag=1;
							free(buff);
							break;
						}
						free(buff);
					}
				}	
			}
			if(quitflag==1)break;
		}
	}
	close(myconnfd);
	printf("[Thread]Fine thread %d\n",my_id);
	return (void*)0;
}
































int main(){

	if (system("clear")<0) 
        perror ("system");
	int *mypipe=malloc(sizeof(int)*2);
	//pipe
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
		else{
			printf("la pipe ha lim sup illimitato\n");
		}	
	}
	else{
		printf("[S]lim sup pipe = %ld\n",v);
	}

	
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

	//segnali
    struct sigaction sa;
    // resetto la struttura
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

	


    
    

	int accept_socket,connfd,len,err;
	struct sockaddr_un servaddr, cli;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sun_family = AF_UNIX;
	strcpy(servaddr.sun_path,SOCKETNAME);

	//SOCKET
	accept_socket = socket(AF_UNIX, SOCK_STREAM, 0);
	if (accept_socket == -1) {
		perror("creazione di accept socket\n");
		unlink(SOCKETNAME);
		exit(EXIT_FAILURE);
	}	

	//BINDING
	if ((bind(accept_socket, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		perror("binding\n");
		unlink(SOCKETNAME);
		exit(EXIT_FAILURE);
	}

	generator=init_id();

	//LISTEN
	if ((listen(accept_socket, SOMAXCONN)) != 0) {
		perror("listening\n");
		unlink(SOCKETNAME);
		exit(EXIT_FAILURE);
	}
	else
		printf("Server in ascolto..\n");
	
	if(pthread_sigmask(SIG_SETMASK, &oldmask, NULL) != 0) {//NON ignorare piu i segnali sopra
        fprintf (stderr, "FATAL ERROR\n");
        return EXIT_FAILURE;
    }


	//lista dei thread che usero' (solo io)
	Listath_t *head=NULL,*current=head;
	fd_set current_sockets, read_ready_sockets;
	FD_ZERO(&current_sockets);
	FD_SET(accept_socket,&current_sockets);
	FD_SET(mypipe[0],&current_sockets);
	int ret_select=0,fd; 

	for(;;){
		read_ready_sockets=current_sockets;
		
		if((ret_select=select(100,&read_ready_sockets,NULL,NULL,NULL))<0 & errno == EINTR) {
			printf("select annullata\n");
			if(terminazione == 1) break; 
		}
		else { /* select OK */
			printf("ELSE: select a buon fine: %d\n",ret_select);
			for (fd = 0; fd<=100;fd++) {
				if (FD_ISSET(fd,&read_ready_sockets)) {
					if (fd == accept_socket) {
						printf("nuova connessione da accettare");
						socklen_t len = sizeof cli;
						connfd = accept(accept_socket, (SA*)&cli, &len);
						if(connfd<0){
							perror("errore accept");
							exit(EXIT_FAILURE);
						}
						//creare nuovo thread
						if(head==NULL){
							head = malloc(sizeof(Listath_t));
							//printf("ho fatto una malloc\n");
							current = head;
						}
						else{
							//printf("ho fatto una malloc\n");
							current->next = malloc(sizeof(Listath_t));
							current = current->next;
						}
						current->connfd=connfd;
						current->next=NULL;
						arg_t passme = {connfd,mypipe};
						if((err=pthread_create(&current->thread,NULL,&client_holder,&passme))!=0){
								//gestisci errore
								perror("creazione thread per gestire client\n");
								unlink(SOCKETNAME);
								exit(EXIT_FAILURE);
						}
						else printf("Thread creato\n");
					}
					else if(fd == mypipe[0]) {
						char* buffer = malloc(100);
						read(mypipe[0],buffer,100);
						printf("[S] read da pipe: '%s'\n",buffer);
						break;
					} 
				}
			} 
		}		
	}
	if(terminazione == 1){
		if(head != NULL){
			Listath_t *tmp=NULL;
			int i=0;
			//se c'e' almeno un elemento
			printf("Elemento n: %d, connfd: %d\n",i,head->connfd);
			//dico a questo thread di terminare tramite pipe
			write(mypipe[1],"EXIT",5);
			pthread_join(head->thread,(void*) &head->status);close(head->connfd);
			printf("Thread terminato correttamente\n");
			current=head;
			while(current->next != NULL){
				i++;
				tmp=current;
				current=current->next;
				free(tmp);
				printf("Elemento n: %d, connfd: %d\n",i,current->connfd);
				
				pthread_join(current->thread,(void*) &current->status);close(current->connfd);
			}
			free(current);
		}
		free(generator);
	}
	close(accept_socket);
	unlink(SOCKETNAME);
	printf("FINE DEL SERVER\n");
	return 0;
}