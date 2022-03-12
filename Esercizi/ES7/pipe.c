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


void stampalista(){
    node_t *pp1=h1;
        int x=1;
        if(pp1!=NULL){
            //printf("[T1 stampo lista] %d: %s->",x,pp1->val);
            while(pp1->next!=NULL){
                pp1=pp1->next;
                x++;
                //printf(" %d: %s->",x,pp1->val);  
            }
           // printf("NULL\n");
        }
}

static void* myt1(void* arg){
    //printf("[T1] iniziato\n");

    FILE *ifp=NULL;
    ifp = fopen(arg,"r");
    if(ifp==NULL){printf("[T1] Impossibile aprire il file\n");exit(EXIT_FAILURE);}
    char* line=malloc(100);
    node_t *pt1=h1; 

    while(fgets(line,100,ifp)!=NULL){
        
        //stampalista();
        pthread_mutex_lock(&mtx1);
        //printf("[T1] preso lock per: %s\n",line);
        if(h1!=NULL){
            //printf("[T1] h1 NON e' null\n");
            pt1=h1;
            while(pt1->next!=NULL){
               // printf("[T1] vado avanti\n");
                pt1=pt1->next; 
            }
            pt1->next=(node_t*)malloc(sizeof(node_t));
            pt1=pt1->next;
           
        }
        else{
            //printf("[T1] h1 e' null\n");
            pt1 = (node_t*)malloc(sizeof(node_t));
            h1=pt1;
        }
        //stampalista();
        
        pt1->next=NULL;
        pt1->val = malloc(300);
        strcpy(pt1->val , line);
        
        //printf("[T1] ho inserito: %s",pt1->val);
        pthread_cond_signal(&b1cond);
        pthread_mutex_unlock(&mtx1);
        //sleep(rand()%10);
        //stampalista();
        
        
    }
    //printf("[T1] Ho finito\n");
    fclose(ifp);
    free(line);
    return (void*)0;
}







static void* myt2(void* arg){
    //printf("[T2] iniziato\n");
    char* curr_line=NULL,*tok;
    node_t *temp=NULL,*p2=h2;


    while(true){
        //prelevo dalla testa
        //printf("[T2] cerco di prendere lock 1\n");
        pthread_mutex_lock(&mtx1);
        //printf("[T2] lock1 presa\n");
        while(h1==NULL)
            pthread_cond_wait(&b1cond,&mtx1);
        //printf("[T2] risvegliato cond1\n");
        curr_line = malloc(100);
        strcpy(curr_line,h1->val);
        if(h1->next!=NULL){
            temp = h1;
            h1=h1->next;
            free(temp->val);
            free(temp);
            temp=NULL;
        }
        else{
            free(h1->val);
            free(h1);
            h1=NULL;
        }
        //printf("[T2] prelevato %s\n",curr_line);
        pthread_mutex_unlock(&mtx1);
         
        //lavoro sull'elemento   
        tok = strtok(curr_line, " ");
        pthread_mutex_lock(&mtx2);
        
        while( tok != NULL ){ 
        //printf("[T2] lock2 /metto %s\n",tok);
            
            if(h2!=NULL){
                p2=h2;
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
            p2->val = malloc(100);
            strcpy(p2->val,tok);
            
            tok = strtok(NULL, " ");
        }
        tok=NULL;
        curr_line=NULL;
        pthread_cond_signal(&b2cond);
        pthread_mutex_unlock(&mtx2);
    }
    //printf("[T2] finito %s\n",tok);
    return (void*)0;
}

static void* myt3(void* argc){
    //printf("[T3] iniziato\n");

    char* curr_tok=NULL;
    node_t *temp=NULL;
    while(true){
        //prelevo dalla testa
        
        pthread_mutex_lock(&mtx2);
        //printf("[T3]lock presa\n");
        while(h2==NULL)
            pthread_cond_wait(&b2cond,&mtx2);
        //printf("[T3]risvegliato\n");
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
    //printf("[T3]finito\n");
    return (void*)0;
}

int main(int argc, char *argv[]){

    if(argc != 2){perror("argomenti != 2");exit(EXIT_FAILURE);}
    //printf("\n\nMAIN\n\n\n");
    int err1,err2,err3,status1,status2,status3;
    pthread_t t1,t2,t3;



    


    if((err1=pthread_create(&t1,NULL,&myt1,argv[1]))!=0){
        //gestisci errore
        perror("creazione t1");exit(EXIT_FAILURE);
    }
    else{
        printf("[]T1 creato\n");
    }


    if((err2=pthread_create(&t2,NULL,&myt2,NULL))!=0){
        //gestisci errore
        perror("creazione t2");exit(EXIT_FAILURE);
    }
    else{
        printf("[]T2 creato\n");
    }


    if((err3=pthread_create(&t3,NULL,&myt3,NULL))!=0){
        //gestisci errore
        perror("creazione t3");exit(EXIT_FAILURE);
    }

    else{
        printf("[]T3 creato\n");
    }
    
    pthread_join(t1,(void*) &status1);
    pthread_join(t2,(void*) &status2);
    pthread_join(t3,(void*) &status3);

    return 0;

}