#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){

    if(argc!=2){perror("argomenti");exit(EXIT_FAILURE);}
    //pid_t pid,pid2;
    //int N = atoi(argv[1]);
    // int i;
    // for(i=0;i<N;i++){
    //     if((pid = fork())<0){
    //      perror("fork");
    //     }
    //     else if (pid == 0){
    //         printf("[figlio] mio pid: %d, pid padre: %d\n",getpid(),getppid());
    //         return 0;
    //     }
    //     else{
    //         //padre
    //         //sleep(5);
    //         printf("[padre] ho generato %d figli.\n",i+1);
    //         return 0;
    //     }
    // }


    //padre
    // printf("[padre] genero figlio\n");
    // if((pid = fork())<0){
    //     perror("fork");
    // }
    // else if (pid == 0){
    //     //figlio
    //     printf("[figlio] genero figlio\n");
    //     if((pid2 = fork())<0){
    //         perror("fork");
    //     }
    //     else if(pid==0){
    //         //nipote
    //         printf("[nipote] eseguo sleep");
    //         execl("/bin/sleep","",argv[1],NULL);
    //     }
    //     else{
    //         //figlio continuo
    //         printf("[figlio] pid fork: %d, mio pid: %d, pid padre: %d\n",pid,getpid(),getppid());
    //         return 0;
    //     }
        
    // }
    // else if(pid > 0){
    //     //padre continuo
    //     printf("[padre] pid fork: %d, mio pid: %d, pid padre: %d\n",pid,getpid(),getppid());
    //     return 0;
    // }

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