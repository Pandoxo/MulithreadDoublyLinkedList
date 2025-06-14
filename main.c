#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "slist.h"

#define THREAD_NUM 1



int main()
{

    list_t* lst = l_init(5);
    list_t* lst2 = l_init(5);
    char* words[10] = {"aa","bb","cc","dd","ee","ff","gg","hh","ii","jj"};
    
    pthread_t th[THREAD_NUM];
    for(int i=0;i<THREAD_NUM;i++){
        
        if(pthread_create(&th[i],NULL,&(void*)l_add(lst,words[i]),NULL) !=0){
                perror("failed to create thread");
            } 
       
    }
    for(int i=0;i<THREAD_NUM;i++){
        if(pthread_join(th[i],NULL)!= 0){
            perror("Failed to join thread");
        }
    }
    l_add(lst,"aa");
    l_add(lst,"bb");
    l_add(lst,"bb");
    l_add(lst,"cc");
    printf("lst1: ");
    l_print(lst);

    l_add(lst2,"aa");
    l_add(lst2,"dd");
    printf("lst2: ");
    l_print(lst2);

    l_destroy(lst);
    l_destroy(lst2);


    return 0;

}