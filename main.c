#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "slist.h"
#include <unistd.h>

#define THREAD_NUM 1


void* producer(void* args){
    list_t* lst = (list_t* )args;
    char* words[10] = {"aa","bb","cc","dd","ee","ff","gg","hh","ii","jj"};
    int x;
    while(1)
    {
        x = rand() % 10;
        l_add(lst,words[x]);
        printf("Added \"%s\"\n",words[x]);
        l_print(lst);
        sleep(1);

    }
    
}

void* consumer(void* args){
    list_t* lst = (list_t* )args;
    char* y;
    while(1)
    {
        y = l_get(lst);
        printf("Popped \"%s\"",y);
        l_print(lst);
        sleep(2);
    }
    
    

}


int main()
{

    srand(time(NULL));
    list_t* lst = l_init(7);
    
    pthread_t th[THREAD_NUM];

    if(pthread_create(&th[0],NULL,&producer,lst) !=0){
        perror("failed to create thread");
    }
     if(pthread_create(&th[1],NULL,consumer,lst) !=0){
        perror("failed to create thread");
    }  

    for(int i=0;i<THREAD_NUM;i++){
        if(pthread_join(th[i],NULL)!= 0){
            perror("Failed to join thread");
        }
    }

    l_destroy(lst);


    return 0;

}