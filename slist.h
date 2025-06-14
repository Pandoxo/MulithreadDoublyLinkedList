#ifndef LCS_LIST_H
#define LCS_LIST_H
#include <pthread.h>
#include <semaphore.h>

// ==============================================
//
//  Version 1.0, 2025-06-04
//
// ==============================================



typedef struct Node {
	char* txt;
	struct Node* next;
	struct Node* prev;

}Node;

struct list_t {
	int capacity;
	int size;
	pthread_mutex_t mutexList;
	sem_t semEmpty;
	sem_t semTaken;
	Node* head;

};

typedef struct list_t list_t;

list_t* l_init(int c);
void l_clear(list_t* lst);

void l_destroy(list_t* lst);

void l_add(list_t* lst, char* str);

char* l_get(list_t* lst);

char* l_pop(list_t* lst);

int l_remove(list_t* lst, char* str);

int l_count(list_t* lst);

void l_setcapacity(list_t* lst, int c);

void l_remove_duplicates(list_t* lst);

void l_join(list_t* lst, list_t* lst2);



void l_print(list_t* lst);

#endif //LCS_LIST_H
