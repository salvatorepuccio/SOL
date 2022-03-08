#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
//#include <sys/wait.h>

int main(int argc, char *argv[]){

    if(argc != 2){perror("argomenti");exit(EXIT_FAILURE);}

    for(int i=0;i<atoi(argv[1]);i++){
        if(fork()==0) exit(0);
    }

    sleep(50);
    printf("FINE");
    return 0;

}