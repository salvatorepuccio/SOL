#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){

    if(argc!=2){perror("argomenti");exit(EXIT_FAILURE);}
    
    int pid = fork();
    //figlio1, padre non fa piu nulla
    if(pid==0){
        if((pid=fork())==0){
            //figlio del figlio1
            execl("/bin/sleep","",argv[1],NULL);
            //fa exec e muore
            perror("execl");
            exit(EXIT_FAILURE);
        }
        //qui siamo ancora nel figlio1, che ora aspetta che suo figlio abbia finito
        if(waitpid(pid,NULL,0)<0){
            fprintf(stderr,"error waiting pid\n");
            return -1;
        }
        //e stampa i suoi dati
        printf("io: %d, padre: %d\n",getpid(),getppid());


    }

    return 0;
}