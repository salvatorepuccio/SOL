#include "id.h"
#include <pthread.h>
#include <stdlib.h>

Idgenerator_t *init_id(){
    Idgenerator_t *i=malloc(sizeof(Idgenerator_t));
    i->id=0;
    i->occupied=0;
    pthread_mutex_init(&i->lock,NULL);
    pthread_cond_init(&i->cond,NULL);
    return i;
}

int getid(Idgenerator_t *id){
    pthread_mutex_lock(&id->lock); 
    while(id->occupied==1)
        pthread_cond_wait(&id->cond,&id->lock);
    id->occupied=1;
    int your_id = id->id;
    id->id++;
    id->occupied=0;
    pthread_cond_signal(&id->cond);
    pthread_mutex_unlock(&id->lock);
    return your_id;
}

