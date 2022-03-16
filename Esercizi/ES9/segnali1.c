#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>


static void gestore (int signum) {
    printf("Ricevuto segnale %d\n",signum);
    exit(EXIT_FAILURE);
} 

int main (void) {
    struct sigaction s; 
    int i;

    /* inizializzo s a 0*/
    memset( &s, 0, sizeof(s) );

    /* registro gestore */
    s.sa_handler=gestore; 
    
    /* installo nuovo gestore s */
    //ec_meno1( sigaction(SIGINT,&s,NULL) );
    sigaction(SIGINT,&s,NULL);//CTRL-C (2)
    sigaction(SIGTSTP,&s,NULL);//CTRL-Z(18)

    for (i=1;;i++) {
        sleep(1);
        printf("%d \n",i);
    }
    exit(EXIT_SUCCESS); }