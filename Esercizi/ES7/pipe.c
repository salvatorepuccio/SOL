#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define N 8

typedef struct node {
    char* val;
    struct node * next;
} node_t;

pthread_mutex_t mtx1=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtx2=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t b1cond=PTHREAD_COND_INITIALIZER;
pthread_cond_t b2cond=PTHREAD_COND_INITIALIZER;
node_t *h1 = NULL;
node_t *h2 = NULL;

static void* myt1(void* arg){
    printf("[T1] iniziato\n");

    FILE *ifp=NULL;
    ifp = fopen(arg,"r");
    if(ifp==NULL){printf("[T1] Impossibile aprire il file\n");exit(EXIT_FAILURE);}
    char* line=malloc(300);
    node_t *p1=h1;

    while(fgets(line,300,ifp)!=NULL){
        //strcat(line,"\n");
        printf("[T1] lock acquire per: %s",line);
        //scrivere nel buffer
        pthread_mutex_lock(&mtx1);
        if(p1!=NULL){
            while(p1->next!=NULL)
                p1=p1->next; 
            p1->next=(node_t*)malloc(sizeof(node_t));
            p1=p1->next;
        }
        else{
            p1 = (node_t*)malloc(sizeof(node_t));
            h1=p1;
        }
        //scrivo in lista
        
        p1->next=NULL;
        p1->val = line;
        pthread_cond_signal(&b1cond);
        pthread_mutex_unlock(&mtx1);
        printf("[T1] SCRITTO\n");
        //sleep(3);
    }
    printf("[T1] Ho finito\n");
    fclose(ifp);
    free(line);
    return (void*)0;
}

static void* myt2(void* arg){
    printf("[T2] iniziato\n");
    char* curr_line=NULL,*tok;
    node_t *temp=NULL,*p2=h2;


    while(true){
        //prelevo dalla testa
        printf("[T2] acquire lock per prelevare\n");
        pthread_mutex_lock(&mtx1);
        while(h1==NULL)
            pthread_cond_wait(&b1cond,&mtx1);
        
        curr_line=h1->val;
        if(h1->next!=NULL){
            temp = h1;
            h1=h1->next;
            free(temp);
            temp=NULL;
        }
        else{
            free(h1);
            h1=NULL;
        }
        printf("[T2] prelevato %s\n",curr_line);
        pthread_mutex_unlock(&mtx1);
         
        //lavoro sull'elemento   
        tok = strtok(curr_line, " ");
        pthread_mutex_lock(&mtx2);
        
        while( tok != NULL ){ 
        printf("[T2] metto %s\n",curr_line);
            
            if(p2!=NULL){
                while(p2->next!=NULL)
                    p2=p2->next; 
                p2->next=(node_t*)malloc(sizeof(node_t));
                p2=p2->next;
            }
            else{
                p2 = (node_t*)malloc(sizeof(node_t));
                h2=p2;
            }
            //scrivo in lista
            p2->next=NULL;
            p2->val = tok;
            
            tok = strtok(NULL, " ");
        }
        pthread_cond_signal(&b2cond);
        pthread_mutex_unlock(&mtx2);
    }
    return (void*)0;
}

static void* myt3(void* argc){
    printf("[T3] iniziato\n");

    char* curr_tok=NULL;
    node_t *temp=NULL;
    while(true){
        //prelevo dalla testa
        printf("[T3]cerco di acquire lock\n");
        pthread_mutex_lock(&mtx2);
        while(h2==NULL)
            pthread_cond_wait(&b2cond,&mtx2);
        
        curr_tok=h2->val;
        if(h2->next!=NULL){
            temp = h2;
            h2=h2->next;
            free(temp);
            temp=NULL;
        }
        else{
            free(h2);
            h2=NULL;
        }
        fprintf(stdout,"XXX[T3]Ho letto: %s\n",curr_tok);
        pthread_mutex_unlock(&mtx2);

        
    }
    return (void*)0;
}

int main(int argc, char *argv[]){

    if(argc != 2){perror("argomenti != 2");exit(EXIT_FAILURE);}
    printf("\n\nMAIN\n\n\n");
    int err1,err2,err3,status1,status2,status3;
    pthread_t t1,t2,t3;
 if((err2=pthread_create(&t2,NULL,&myt2,NULL))!=0){
        //gestisci errore
        perror("creazione t2");exit(EXIT_FAILURE);
    }
    if((err1=pthread_create(&t1,NULL,&myt1,argv[1]))!=0){
        //gestisci errore
        perror("creazione t1");exit(EXIT_FAILURE);
    }
    
   

    if((err3=pthread_create(&t3,NULL,&myt3,NULL))!=0){
        //gestisci errore
        perror("creazione t3");exit(EXIT_FAILURE);
    }
    
    pthread_join(t2,(void*) &status2);
    pthread_join(t1,(void*) &status1);
    
    pthread_join(t3,(void*) &status3);

    return 0;

}