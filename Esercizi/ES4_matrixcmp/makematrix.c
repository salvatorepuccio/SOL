#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Modifica fatta da RINGHIO 18:54
//Modifica fatta da T430 18:57

int main (int argc , char **argv){
    if(argc!=2) {printf("inserire (solo) il numero di elementi della matrice quadrata."); return -1;}
    int N=atoi(argv[1]);
    printf("N=%d\n",N);
    float **M1;
    M1=malloc(sizeof(float*)*N);
    for(int i=0;i<N;i++){ 
        M1[i]=malloc(sizeof(float)*N);
        for(int j=0;j<N;j++){
            M1[i][j] = (i+j)/2.0;
        }
    }

    FILE *bin,*txt;
    bin = fopen("./mat_dump.dat","w+");
    if(bin==NULL){printf("Impossibile aprire il file\n");return -1;}
    txt = fopen("./mat_dump.txt","w+");
    if(txt==NULL){printf("Impossibile aprire il file\n");return -1;}

    printf("Scrittura\n");
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++){
            printf("scrivo %.2f\n",M1[i][j]);
            char buffer[10];
            snprintf(buffer, sizeof buffer, "%.1f",M1[i][j]);
            fwrite(&M1[i][j],sizeof(float),1,bin);
            fprintf(txt,"%s ",buffer);
        }
        fprintf(txt,"\n");
    }
    printf("fine scrittura.\n");

    fclose(bin);
    fclose(txt);
    free(M1);
    
//

    return 0;
}