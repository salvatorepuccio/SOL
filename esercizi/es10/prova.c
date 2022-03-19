#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handle_sigtstp (int sig) {
    if(sig==SIGTSTP)printf ("Stop not allowed\n");
    if(sig==SIGINT) printf("Interrupt not allowed\n");
}

void handler(int sig){
    printf("Input number: ");
    fflush(stdout);
}

int main(int argc, char* argv[]) {
    struct sigaction sa;
    sa.sa_handler = &handler; 

    sa.sa_flags=SA_RESTART;//server perche' stiamo usando printf, per un motivo non specificato
    // sigaction(SIGTSTP, &sa, NULL);
    // sigaction(SIGINT, &sa, NULL);
    sigaction(SIGCONT, &sa, NULL);

    // for(;;){
    //     printf("for\n");
    //     sleep(3); 
    // }


    int x;
    printf("Input number: ");
    scanf("%d", &x) ;
    printf("Result %d * 5 = %d\n", x, x * 5);
    return 0;
    //modifica da macos
}