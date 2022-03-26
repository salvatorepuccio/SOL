#ifndef THREADPOOL_
#define THREADPOOL_H_
#include <pthread.h>
/*
@file threadpool.h
@brief Interfaccia per il ThreadPool

@struct threafun_t
@brief generico task che un thread del threadpool deve eseguire
@var fun Puntatore alla funzione da eseguire
@var arg Argomento della funzione
*/

typedef struct taskfun_t {
    void (*fun) (void *);
    void *arg;
}taskfun_t;

/*
@struct threadpool
@brief Rappresentazione dell'oggetto threadpool
*/

typedef struct threadpool_t {
    pthread_mutex_t lock; //mutua esclusione nell'accesso all'oggetto
    pthread_cond_t cond; // usata per notificare un worker thread
    pthread_t *threads;// array di worker id
    int numthreads; // numero di thread (size dell'array threads)
    taskfun_t *pending_queue; // coda interna per task pendenti
    int queue_size; // massima size della coda, puo essere anche ad indicare che non si vogliono gestire task pendenti
    int taskonthefly; //numero di task attualmente in esecuzione
    int head, tail; //riferimenti della coda
    int count; //numero di task nella coda dei task pendenti
    int exiting; // se > 0 e' iniziato il protocollo di uscita,1 il thread aspetta che non ci siano piu' lavori in coda
}threadpool_t;

threadpool_t *createThreadPool (int numthreads, int pending_size);

int destroyThreadPool(threadpool_t *pool, int force);

int addToThreadPool(threadpool_t *pool, void (*fun) (void *), void *arg);

int spawnThread(void (*f) (void*), void* arg);

#endif