#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){

    printf("eseguo il programma makematrix\n");

    execl("/mnt/d/SOL/Esercizi/ES4/main.o","","5",NULL);

    return 0;
}