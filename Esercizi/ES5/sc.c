#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

int main(int argc,char **argv){
    clock_t begin,end;
    double time_spent;

    begin = clock();

    if(argc<3||argc>4){printf("args: filein fileout [buffersize]"); return -1;}
    //FILE *ifp=NULL, *ofp=NULL;
    int id1,id2;

    if(( id1 = open(argv[1],O_RDONLY)) == -1) {
        perror("1 in apertura");
        exit(EXIT_FAILURE); /* termina */
    }

    if(( id2 = open(argv[2],O_WRONLY | O_TRUNC | O_CREAT,0644)) == -1) {
        perror("2 in apertura");
        exit(EXIT_FAILURE); /* termina */
    }
    
    int buffersize = 256;
    if(argc==4) buffersize=atoi(argv[3]);
    




    //inizio lettura
    int nread=-1;
    char *buffer = malloc(buffersize);
    begin = clock();

    while((nread = read(id1,buffer,buffersize))>0){
        if(write(id2,buffer,nread)==-1){
            perror("in scrittura");
            exit(EXIT_FAILURE);
        }
    }
    if(nread==-1){perror("lettura");exit(EXIT_FAILURE);}

    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("syscall: time %f sec w/ buffer: %d byte\n",time_spent*10,buffersize);
    free(buffer);



    //parte con funzioni di libreria

    FILE *fin,*fout; 
    mode_t oldmask = umask(033);

    if((fin=fopen(argv[1],"r"))==NULL){
        perror("in apertura file input");
        exit(EXIT_FAILURE);
    }
    if((fin=fopen(argv[2],"w+"))==NULL){
        perror("in apertura file output");
        exit(EXIT_FAILURE);
    }
    umask(oldmask);

    size_t len=0;
    begin = clock();
    buffer=malloc(buffersize);
    while((len=fread(buffer,1,buffersize,fin))>0){
        if(fwrite(buffer,1,len, fout)!=len){
            perror("in scrittura");
            exit(EXIT_FAILURE);
        }
    }
    end=clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("lib: time %f sec w/ buffer: %d byte\n",time_spent*10,buffersize);

    //fclose(fin);
    //fclose(fout);
    //free(buffer);
    
    return 0;
}

