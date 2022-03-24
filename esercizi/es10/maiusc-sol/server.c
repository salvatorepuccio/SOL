#define POSIX C SOURCE 200112L
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
#include <signal.h>
#include <sys/time.h>
#include <assert.h>

#define SOCKNAME "./mysock"
#define SA struct sockaddr
#define MAXBACKLOG 100



typedef struct msg {
int len;
    char *str;
}msg_t;

static volatile sig_atomic_t termina=0;
static void sighandler (int useless) {
    termina=1;
}

// converte tutti i carattere minuscoli in maiuscoli
void toup(msg_t str){
    for(int i=0;i<str.len;i++){
        if(str.str[i]>96&&str.str[i]<123) 
            str.str[i]=str.str[i]-32; 
    }
}

//thread che gestisce tutta la connessione verso il client
void *threadF(void *arg) {
    assert(arg);
    long connfd= (long)arg;
    do {
        msg_t str;
        int n;
        if ((n=read(connfd, &str.len, sizeof(int))) ==-1) {
            perror("read1");
            break;
        }
        printf("len: %d\n",str.len);
        if (n==0) break;
        str.str = calloc((str.len), sizeof(char));
        if (!str.str) {
            perror("calloc");
            fprintf(stderr, "Memoria esaurita....\n");
            break;
        }
        if((n=read(connfd, str.str, str.len*sizeof(char))) ==-1) {
            perror("read2");
            free(str.str);
            break;
        }
        toup(str);
        if ((n=write(connfd, str.str, str.len*sizeof(char))) ==-1) {
            perror("write2");
            free(str.str);
            break;
        }
        free(str.str);
    }while(1);
    close(connfd);
    return NULL;
}

int spawn_thread(long connfd) {
    pthread_attr_t thattr;
    pthread_t thid;
    sigset_t mask,oldmask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGQUIT);
    sigaddset(&mask, SIGTERM);
    sigaddset(&mask, SIGHUP);
    if (pthread_sigmask(SIG_BLOCK, &mask,&oldmask)!= 0) {
        fprintf(stderr, "FATAL ERROR, pthread_sigmask\n");
        close(connfd);
        return 1;
    }
    if (pthread_attr_init(&thattr) != 0) {
        fprintf(stderr, "pthread_attr_init FALLITA\n");
        close(connfd);
        return -1;
    }
    // settiamo il thread in modalità detached
    if(pthread_attr_setdetachstate(&thattr,PTHREAD_CREATE_DETACHED)!=0){
        fprintf(stderr, "pthread_attr_setdetachstate FALLITA\n");
        pthread_attr_destroy(&thattr);
        close(connfd);
        return 1;
    }
    if (pthread_create(&thid, &thattr, threadF, (void*)connfd) != 0) {
        fprintf(stderr, "pthread create FALLITA");
        pthread_attr_destroy(&thattr);
        close(connfd);
        return 1;
    }

    if (pthread_sigmask(SIG_SETMASK, &oldmask, NULL) != 0) {
        fprintf (stderr, "FATAL ERROR\n");
        close(connfd);
        return-1;
    }
    return 0;
}


int main(int argc,char *argv[]){
    sigset_t mask,oldmask;
    sigemptyset(&mask);
    sigaddset(&mask,SIGINT);
    sigaddset(&mask, SIGQUIT);
    sigaddset(&mask, SIGTERM);
    sigaddset(&mask, SIGHUP);

    if (pthread_sigmask(SIG_BLOCK, &mask, &oldmask) != 0) {//ignora i segnali sopra
        fprintf(stderr, "FATAL ERROR, pthread_sigmask\n");
        return EXIT_FAILURE;
    }

    // installo il signal handler per tutti i segnali che mi interessano
    struct sigaction sa;
    // resetto la struttura
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sighandler;
    
    if (sigaction(SIGINT, &sa, NULL) ==-1)
        perror("sigaction SIGINT");
    if (sigaction(SIGQUIT, &sa, NULL) ==-1)
        perror("sigaction SIGINT");
    if (sigaction(SIGTERM, &sa, NULL) ==-1)
        perror("sigaction SIGINT");
    if (sigaction(SIGHUP, &sa, NULL) ==-1)
        perror("sigaction SIGINT");

    int listenfd;
    listenfd=socket(AF_UNIX, SOCK_STREAM, 0);
    if(listenfd<0){
        perror("listen");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strncpy(serv_addr.sun_path,SOCKNAME, strlen(SOCKNAME)+1);
    bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr));
    listen(listenfd, MAXBACKLOG);

    if(pthread_sigmask(SIG_SETMASK, &oldmask, NULL) != 0) {//NON ignorare piu i segnali sopra
        fprintf (stderr, "FATAL ERROR\n");
        return EXIT_FAILURE;
    }

    int r=0;
    while(!termina) {
        long connfd=-1;
        if (!termina && (connfd=accept(listenfd, (struct sockaddr*)NULL, NULL)) == -1){
            if(errno==EINTR){
                if (termina) break;
            } 
            else {
            perror("accept");
            r=EXIT_FAILURE;
            }
        }
        printf ("connection accepted\n");
        if (spawn_thread(connfd)<0) {
        r=EXIT_FAILURE;
        break;
        }
    }
    // clean-up I
    unlink(SOCKNAME);
    return r;
}
