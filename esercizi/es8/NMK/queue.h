//interfaccia della coda

//#ifndef QUEUE_H_
#define QUEUE_H_

#include <pthread.h>

//struttura di un nodo della coda
typedef struct Node {
    void *data;
    struct Node * next;
} Node_t;

//struttura della coda
typedef struct Queue{
    Node_t *head;
    Node_t *tail;
    unsigned long qlen;
    pthread_mutex_t qlock;
    pthread_cond_t qcond;
} Queue_t;

//metodo per creare una coda
Queue_t *initQueue();

//metodo per cancellare una coda
void deleteQueue(Queue_t *q);

//metodo per inserire un elemento nella coda
int push(Queue_t *q, void *data);

//metodo per prelevare un elemento dalla coda
void *pop(Queue_t *q);

//metodo che restituisce la lunghezza attuale della coda
unsigned long length(Queue_t *q);
