// @file threadpool.c
// @brief File di implementazione dell'interfaccia Threadpool

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <util.h>
#include "threadpool.h"

// @function void *threadpool_thread(void *threadpool)
// @brief funzione eseguita dal thread worker che appartiene al pool

static void *workerpool_thread(void *threadpool) {
    threadpool_t *pool = (threadpool_t *)threadpool; // cast
    taskfun_t task; // generic task
    pthread_t self = pthread_self();
    int myid =-1;
    // non efficiente, si puo' fare meglio....
    do {
        for (int i=0;i<pool->numthreads;++i)
            if (pthread_equal(pool->threads[i], self)) {
                myid=i;
                break;
            }
    } while (myid < 0);
    LOCK_RETURN(& (pool->lock), NULL);

    for (;;) {
        // in attesa di un messaggio, controllo spurious wakeups.
        while((pool->count == 0) && (!pool->exiting)) {
            pthread_conq_wait(&(pool->cond),&(pool->lock));
        }
        if (pool->exiting == 1) break; // exit forzato, esco immediatamente
        // devo uscire ma ci sono messaggi pendenti 
        if (pool->exiting == 1 && !pool->count) break;
        // nuovo task
        task. fun = pool->pending_queue[pool->head]. fun;
        task.arg = pool->pending_queue[pool->head].arg;
        pool->head++; pool->count--;
        pool->head = (pool->head == abs (pool->queue_size)) ? 0 : pool->head;

        pool->taskonthefly++;
        UNLOCK_RETURN(&(pool->lock),NULL);
        // eseguo la funzione
        (*(task. fun)) (task. arg);
        LOCK_RETURN (& (pool->lock), NULL);
        pool->taskonthefly--;
    }
    UNLOCK_RETURN(&(pool->lock), NULL);
    fprintf(stderr, "thread %d exiting\n", myid);
    return NULL;
}

static int freePoolResources(threadpool_t *pool) {
    if(pool->threads) {
        free(pool->threads);
        free(pool->pending_queue);
        pthread_mutex_destroy(&(pool->lock));
        pthread_cond_destroy(&(pool->cond));
    }
    free(pool);
    return 0;
}

//threadpool_t *createThreadPool (int numthreads, int pending_size) {
//if (numthreads <= 0 pending size < 0) {