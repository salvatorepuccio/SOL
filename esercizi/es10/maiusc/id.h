#define ID_H_
#include<pthread.h>


typedef struct Idgenerator {
	int id;
	int occupied;
    pthread_mutex_t lock;
    pthread_cond_t cond;
}Idgenerator_t;
 
Idgenerator_t *init_id();

int getid(Idgenerator_t*);