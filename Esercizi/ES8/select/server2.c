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

// struct sockaddr_un {
// 	sa_family_t sun_family;
// 	char sun_path[UNIX_PATH_MAX];
// };

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
