#include <stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <sys/wait.h>



int main(int argc , char *argv[]){

    if(argc!=2){perror("argomenti");exit(EXIT_FAILURE);}

    int N = atoi(argv[1]);
    if(!N>1){perror("N deve essere >1");exit(EXIT_FAILURE);}
    int pid; 


    int var=0;
    while(1){
        if(var==N){
            printf("%d: sono l'ultimo discendente\n",getpid());
            break;
        }
        else{
            //genera figlio
            for(int j=var;j<N;j++) printf("-"); printf(" ");
            printf("%d: creo un processo figlio\n",getpid());
            
            pid = fork();
            waitpid(pid,NULL,0);
            if(pid>0){
                //sono il padre
                break;
            }
            //sono il figlio e continuo
            var++;
        }
    }
    if(var!=N){ for(int j=var;j<N;j++) printf("-"); printf(" ");}
    printf("%d: terminato con successo\n",getpid());
    return 0;

}