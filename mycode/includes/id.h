#ifndef _ID_H_
#define _ID_H_
#include<pthread.h>


typedef struct Idgenerator Idgenerator_t;
 
Idgenerator_t *init_id();

int getid(Idgenerator_t*);

#endif