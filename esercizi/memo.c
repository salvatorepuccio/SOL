#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>


void main(){

    //
    //
    //
    //creare un file syscall open
    int fd;
    char *path=NULL;
    if(( fd = open(path,O_WRONLY | O_APPEND | O_CREAT,0644)) == -1) {
        //la open returna il file descriptor, se il file non esiste verra' creato
            perror("in apertura/creazione");
            exit(EXIT_FAILURE); /* termina */
    }

    //
    //
    //
    //aprire un file il sola lettura
    if(( fd = open(path,O_RDONLY)) == -1) {
        perror("1 in apertura");
        exit(EXIT_FAILURE); /* termina */
    }

    //
    //
    //
    //aprire file in scrittura, cancellandolo se esiste gia' creandolo se non esiste
    //system call syscall sc
    FILE *output;
    if(( output = open("./output.txt",O_WRONLY | O_APPEND | O_CREAT,0644)) == -1) {
        //la open returna il file descriptor, se il file non esiste verra' creato
            perror("in apertura/creazione");
            exit(EXIT_FAILURE); /* termina */
    }
    //libreria
    if( (output = fopen("./passwd.txt","w+"))==NULL){
        perror("errore apertura file");
    }


    //
    //
    //
    //come leggere quanto scritto da tastiera fino ad "a capo"
    char buff[128];
    int n=0;
    while ((buff[n++] = getchar()) != '\n')
		;

    //
    //
    //
    //copiare un file in un altro (duplicare)
    //inizio lettura
    int nread=-1;
    int id1,id2,buffersize;
    char *buffer = malloc(buffersize);
    while((nread = read(id1,buffer,buffersize))>0){//leggo al max buffersize byte
        if(write(id2,buffer,nread)==-1){ //ma scrivo al max nread byte !
            perror("in scrittura");
            exit(EXIT_FAILURE);
        }
    }
    if(nread==-1){perror("lettura");exit(EXIT_FAILURE);}



    //
    //
    //
    //leggere una riga di un file txt
    char *line=malloc(128);
    
    while(fgets(line,sizeof(line),fd)!=NULL){
        //fare qualcosa con line ad esempio scriverla in un altro file
        fputs(line,output);

    }

    //
    //
    //
    //aprire un file con funzione di libreria
    FILE *bin,*txt;
    bin = fopen("./mat_dump.dat","w+");
    if(bin==NULL){printf("Impossibile aprire il file\n");return -1;}

    //
    //
    //
    //scrivere su di un file testuale text txt
    char buffer[10]; float item;
    snprintf(buffer, sizeof buffer, "%.1f",item);
    //quanto sopra serve per convertire un item float in un array di caratteri
    fprintf(txt,"%s ",buffer);
    //usare quando si deve scrivere in modo formattato (stile printf)

    //
    //
    //
    //scrivere dati raw sau file .dat
    fwrite(&item,sizeof(float),1,bin);


    //
    //
    //
    //calcolare il tempo di esecuzione di un programma
    #include <time.h>
    clock_t begin = clock();

    /* here, do your time-consuming job */

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("time: %f\n",time_spent/* *10 */);
    //*10 e va in secondi




    //
    //
    //
    //eseguire un file eseguibile
    execl("/mnt/d/SOL/Esercizi/ES4/main.o","","5",NULL);
    //primo parametro il path, che puo' essere anche relativo con execlp
    //secondo parametro sarebbe argv[0] quindi 
    //./main.o primo secondo diventa execl("path/main.o,"","primo","secondo")
    //   [0    ,  1  ,  2   ]                           [0 ,   1   ,    2   ]
    //NULL indica che sono finiti i parametri

    //
    //
    //eseguire un programma in background (da bash)
    //   ./nomeprog.o &
    //basta aggiungere l'&

    //
    //
    //
    //ricorsione con fork e waitpid
    while(1){
        if(2/*condizione di fine ciclo*/){
            //fai qualcosa
            break;
        }
        else{
            //genera un nuovo figlio che continua
            int pid =fork();
            waitpid(pid,NULL,0);
            if(pid>0){
                //padre
                //cosa deve fare il padre quando il figlio ha finito?
                //di solito qualcosa e poi break;
            }
            if(pid==0){
                //figlio
                //in questo caso probabilmente continuera' il ciclo, quindi non serve questo if
            }
        }
    }

    //
    //
    //
    //SEGNALI SIGNAL (maledetti)
    //gli unici segnali che si accumulano sono i SIGCHLD 
    //tutti gli altri vengono persi se ne arriva un altro dello stesso tipo
    sigset_t mask,oldmask,handlermask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT); 
    sigaddset (&mask, SIGTSTP);

    //ignora (SIG_BLOCK) i segnali in mask, mentre salva in oldmask quella attuale
    //solo per single thread
    sigprocmask(SIG_BLOCK, &mask, &oldmask);
    
    //come sopra ma per programmi multithread
    pthread_sigmask(SIG_BLOCK, &mask, &oldmask);

    //imposta la maschera dell'handler, cosi reagira' a questi 3 SIG
    struct sigaction sa;
    sa.sa_mask = handlermask;

    //imposto gestore di SIGINT
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler=handler; 

    //non ho capito a che serve
    sigaction(SIGINT,&sa,NULL);
    sigaction(SIGTSTP,&sa,NULL);
    //sigaction(SIGALRM,&sa,NULL);

}


    //
    //
    //
    //thread
    //creare 10 thread che fanno la stessa cosa e poi uscire
    static void* myfun (void* arg){
        //fa qualcosa
        pthread_exit((void*)17);
        //o return 0;
    }

    int main2(){
        pthread_t tid[10];
        int i=0,status,err;
        while(true){
        if((err=pthread_create(&tid[i],NULL, &myfun,(void*)i))!=0){
            //gestisci errore
            perror("creazione thread");exit(EXIT_FAILURE);
        }
        else{
            //creato correttamente
            printf("[] creato thread %d\n",i);
            i++;
        }
        if(i==10) break;
    }

    for(int j=0;j<10;j++){ pthread_join(tid[j],(void*) &status);}
    }
