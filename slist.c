//Definitions
#include "slist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>


list_t* l_init(int c){
    list_t* list;
    list = malloc(sizeof(list_t));
    list->capacity = c;
    list->size = 0;

    if(pthread_mutex_init(&list->mutexList,NULL)){
        free(list);
        return NULL;
    }
    if(sem_init(&list->semEmpty,0,c)){
        pthread_mutex_destroy(&list->mutexList);
        free(list);
        return NULL;
    }

    if(sem_init(&list->semTaken,0,0)){
        pthread_mutex_destroy(&list->mutexList);
        sem_destroy(&list->semEmpty);
        free(list);
        return NULL;
    }

    return list;

}
void l_clear(list_t* lst){
    if(lst == NULL || lst->head == NULL)
        return;
    pthread_mutex_lock(&lst->mutexList);
    Node* curr = lst->head;
    Node* temp; 
    while(curr!=NULL)
    {
        temp = curr->next;
        free(curr->txt);
        free(curr);
        curr->next = NULL;
        curr->prev = NULL;
        curr = temp;
        sem_post(&lst->semEmpty);
        sem_wait(&lst->semTaken);

    }
    lst->head = NULL;
    lst->size = 0;
    pthread_mutex_unlock(&lst->mutexList);
}


void l_destroy(list_t* lst){
    if(lst == NULL)
        return;
    l_clear(lst);
    pthread_mutex_destroy(&lst->mutexList);
    sem_destroy(&lst->semEmpty);
    sem_destroy(&lst->semTaken);
    free(lst);
    lst->head = NULL;
    lst = NULL;


}
void l_remove_node(list_t* lst, Node* node)
{
    
    if(lst == NULL ||node == NULL)
        return;
    // sem_wait(&lst->semTaken);
    // pthread_mutex_lock(&lst->mutexList);

    if(node->prev != NULL)
        node->prev->next = node->next;
    else
        lst->head = node->next;

    if(node->next != NULL)
        node->next->prev = node->prev;
    
    free(node->txt);
    free(node);
    node = NULL;
    lst->size -= 1;
    // sem_post(&lst->semTaken);
    // pthread_mutex_unlock(&lst->mutexList);
    
   
}
void l_insert_between(list_t* lst,Node* prev, Node* node, Node* next){

    if(lst==NULL || node == NULL){
        return;
    }
    if(prev == NULL)
        lst->head = node;
    else
        prev->next = node;
    
    node->prev = prev;
    if(next != NULL)
        next->prev = node;
    node->next = next;
    lst->size +=1;
}

void l_add(list_t* lst, char* str){
    if(lst ==NULL || str == NULL)
        return;
        
    sem_wait(&lst->semEmpty);
    pthread_mutex_lock(&lst->mutexList);
    
    if(lst->size == lst->capacity){
        pthread_mutex_unlock(&lst->mutexList);
        sem_post(&lst->semEmpty);
        return;
    }

    Node* new_node;
    new_node = malloc(sizeof(Node));

    if(new_node == NULL){
        pthread_mutex_unlock(&lst->mutexList);
        sem_post(&lst->semEmpty);
        return;
    }

    new_node->txt = strdup(str);
    if(new_node->txt == NULL){
        pthread_mutex_unlock(&lst->mutexList);
        sem_post(&lst->semEmpty);
        return;
    }

    new_node->next = NULL;
    new_node->prev = NULL;

    //Empty list
    if(lst->head == NULL){
        lst->head = new_node;
        lst->size = 1;
        sem_post(&lst->semTaken);
        pthread_mutex_unlock(&lst->mutexList);
        return;
    }
    
    int result;
    Node* curr = lst->head;
    while(curr->next != NULL){
        result =strcmp(curr->txt,str); 
        if( result < 0){
            curr = curr->next;
        }else if(result >= 0){
            l_insert_between(lst,curr->prev,new_node,curr);
            return;
        }
    }
    //reached tail curr->next = NULL
    if(result < 0)
        l_insert_between(lst,curr,new_node, curr->next);
    else
        l_insert_between(lst,curr->prev,new_node, curr);

    sem_post(&lst->semTaken);
    pthread_mutex_unlock(&lst->mutexList);

}

char* l_get(list_t* lst){

    
    if(lst == NULL){
        return strdup("");
    }

    sem_wait(&lst->semTaken);
    pthread_mutex_lock(&lst->mutexList);

    if(lst->head == NULL){
        pthread_mutex_unlock(&lst->mutexList);
        sem_post(&lst->semTaken);
        return strdup("");
    }
    Node* curr = lst->head;
    char* str = strdup(curr->txt);
    l_remove_node(lst,curr);

    sem_post(&lst->semEmpty);
    pthread_mutex_unlock(&lst->mutexList);
    return str;
    

}

char* l_pop(list_t* lst)
{
    if(lst==NULL){
        return strdup("");
    }
    sem_wait(&lst->semTaken);
    pthread_mutex_lock(&lst->mutexList);
    
    if(lst->head == NULL){
        pthread_mutex_unlock(&lst->mutexList);
        sem_post(&lst->semTaken);
        return strdup("");
    }

    Node* curr = lst->head;
    while(curr->next != NULL){
        curr = curr->next;
    }
    char* str = strdup(curr->txt);
    l_remove_node(lst, curr);
    
    sem_post(&lst->semEmpty);
    pthread_mutex_unlock(&lst->mutexList);
    return str;


}

int l_remove(list_t* lst, char* str){
    if(lst == NULL|| str == NULL ||lst->head == NULL ){
        return 0;
    }
    sem_wait(&lst->semTaken);
    pthread_mutex_lock(&lst->mutexList);
    
    Node* curr = lst->head;
    Node* temp;
    while(curr != NULL)
    {
        if(strcmp(curr->txt,str)==0){
            temp = curr->next;
            l_remove_node(lst,curr);
            curr = temp;
        }else
            curr = curr->next;
    }
    sem_post(&lst->semEmpty);
    pthread_mutex_unlock(&lst->mutexList);
    return 0;

}

int l_count(list_t* lst){
    return lst->size;
}

void l_setcapacity(list_t* lst, int c){
    if(lst ==NULL)
        return;
    pthread_mutex_lock(&lst->mutexList);
    if(lst->size > c){
        lst->capacity = lst->size;
    }else{
        lst->capacity = c;
    }
    pthread_mutex_unlock(&lst->mutexList);
}

void l_remove_duplicates(list_t* lst){
    //Because the list is already sorted 
     if(lst == NULL||lst->head == NULL){
        return;
    }

    pthread_mutex_lock(&lst->mutexList);
    Node* curr = lst->head;
    Node* temp;
    int duplicate = 0;
    while(curr != NULL && curr->next != NULL)
    {
        if(strcmp(curr->txt,curr->next->txt)==0){
            temp=curr->next;
            l_remove_node(lst,curr);
            duplicate = 1;
            curr= temp;
        }else{
            if(duplicate){
                temp = curr->next;
                l_remove_node(lst,curr);
                curr = temp;
                duplicate = 0;
            }else{
                curr = curr->next;
            }
        }
    }
    if (duplicate && curr != NULL) {
        l_remove_node(lst, curr);
    }
    pthread_mutex_unlock(&lst->mutexList);
}
void l_join(list_t* lst, list_t* lst2){
    
    if(lst == NULL || lst2 == NULL){
        return;
    }
    pthread_mutex_lock(&lst->mutexList);
    pthread_mutex_lock(&lst2->mutexList);
    
    
    Node* curr1 = lst->head;
    Node* curr2 = lst2->head;
    Node* next2;

    if (curr1 == NULL) {
        lst->head = curr2;
        lst2->head = NULL;
        pthread_mutex_unlock(&lst->mutexList);
        pthread_mutex_unlock(&lst2->mutexList);
        return;
    }
    while (curr1 != NULL && curr2 != NULL) {
        int result = strcmp(curr1->txt, curr2->txt);
        if (result <= 0) {
            curr1 = curr1->next;
        } else {
            next2 = curr2->next;
            l_insert_between(lst, curr1->prev, curr2, curr1);
            curr2 = next2;
        }
    }

    if (curr2 != NULL) {
        // Find the last node of lst
        Node* tail = lst->head;
        while (tail->next != NULL)
            tail = tail->next;

        // Append all remaining nodes
        while (curr2 != NULL) {
            next2 = curr2->next;
            l_insert_between(lst, tail, curr2, NULL);
            tail = curr2;
            curr2 = next2;
        }
    }

    lst2->head = NULL;
    pthread_mutex_unlock(&lst->mutexList);
    pthread_mutex_unlock(&lst2->mutexList);

}


void l_print(list_t* lst){
    if(lst==NULL)
        return;

    pthread_mutex_lock(&lst->mutexList);
    if(lst->head == NULL)
    {
        printf("[]\n");
        pthread_mutex_unlock(&lst->mutexList);
        return;
    }

    Node* curr = lst->head;
    printf("[");
    while(curr!=NULL)
    {
        printf("\"%s\"",curr->txt);
        if(curr->next != NULL)
            printf(",");
        curr = curr->next;
    }
    printf("]\n");
    pthread_mutex_unlock(&lst->mutexList);


}
