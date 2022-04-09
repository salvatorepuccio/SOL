#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define H (*head)
#define MX (*head)->mutex
#define CND (*head)->cond

typedef struct head{
    request_t *firstelem;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
}head_t;

typedef struct request{
    int client; //client's connection fd
    void *func; //funzione
    int ret;//valore di ritorno della funzione
    int param1;
    int param2;
    request_t *next;
}request_t;


/**
 * @brief inizializza la coda e restituisce la testa
 * 
 * @return request_t* 
 */
request_t* init(){

    head_t *head = (head_t*)malloc(sizeof(head_t));
    head->firstelem=NULL;
    pthread_mutex_init(&head->mutex,NULL);
    pthread_cond_init(&head->cond,NULL);

    return head;
}

/**
 * @brief 
 * 
 * @param H 
 * @return request_t* 
 */
request_t* pop(head_t *H){

    if(*head == NULL) return;

    request_t *first;

    pthread_mutex_lock(&MX);
    while(H->firstelem==NULL)
        pthread_cond_wait(&CND,&MX);
    
    first = H->firstelem;

    if(first->next == NULL)
        H->firstelem=NULL;
    else
        H->firstelem=first->next;

    pthread_cond_signal(&CND);
    pthread_mutex_unlock(&MX);
    return first;
}

/**
 * @brief inserisce elem in fondo alla coda definita da head
 * 
 * @param head testa della coda
 * @param elem elemento da inserire
 * @return int posizione in cui e' stato messo H->1,2,3
 */
int push(head_t *H, request_t *elem){
    if(H==NULL) return -1;
    if(elem==NULL) return -1;
    if(elem->next != NULL) elem->next = NULL;
    int index=1;

    pthread_mutex_lock(&MX);

    if(H->firstelem == NULL){
        elem->next=NULL;
        H->firstelem = elem;
    }
    else{
        index++;
        request_t *current=H->firstelem;
        while(current->next != NULL){
            current=current->next;
            index++;
        }
        current->next = elem;
    }

    pthread_cond_signal(&CND);
    pthread_spin_unlock(&MX);

    return index;

}

