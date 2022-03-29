#ifndef _STRINGSMANAGER_H_
#define _STRINGSMANAGER_H_

#include <stdlib.h>


/**
 * @brief ingrandisce un array di char mantenendo il contenuto presente
 * enlarge(target, 10)
 * se target = ['a','b','c','d','','\n'] strlen = 6 dimensione effettiva = 5
 * diventa ['a','b','c','d','','','','','','\n'] strlen = 10 dimensione effettiva = 9
 * 
 * @note libera la memoria occupata dal vecchio puntatore
 * 
 * @note enlarge(&buffer,dim); modifica buffer in loco, passare per indirizzo
 * 
 * @param target puntatore ad un array di char
 * @param dim la dimensione a cui lo si vuole portare > strlen
 * 
 * @return 0 success / -1 if dim < strlen(target)
 */
int enlarge(char** target,int dim);


/**
 * @brief concatena target e b, mettendo il risultato in target. gestisce la memoria da solo
 * 
 * @example mystrcat(&target, b,5); //target = [a,a,a,a,,] b=[b,b] // target = [a,a,a,a,b,b,,,,,,]
 * 
 * @param target parte 1
 * @param copy parte 2
 * @param oversize la dimensione libera che vogliamo alla fine dell'array
 * @return int la dimensione dell'array -> tutta la dimensione lo spazio non occupato a dx (!=strlen)
 */
int mystrcat(char** target, char* b, int oversize);

#endif