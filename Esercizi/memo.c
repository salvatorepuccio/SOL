#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>



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



}
