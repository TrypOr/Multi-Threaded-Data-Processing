/**
 * @file project_1.c
 * @author Orestis Trypidakis 4235
 * @brief Dll header of project
 * @date 2022-12-06
 * 
 */
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct DLLNode ListNode;
/*Create struct for DLL node*/
struct DLLNode {
  int productID;
  pthread_mutex_t lock;
  ListNode *next;
  ListNode *prev;
};


/*Create struct for DLL*/
struct LinkedList{
  ListNode *head;
  ListNode *tail;
  pthread_mutex_t h_lock;
};
typedef struct LinkedList List;
/*Initialise list*/
List *list;

/*Function to create a new list node*/
ListNode *DLL_new_node(int key) {
  /*Memory allocation*/
  ListNode *n = (ListNode *)malloc(sizeof(ListNode));
  /*Initialise variables*/
  n->productID = key;
  n->next = NULL;
  n->prev = NULL;
  /*Initialise lock*/
  pthread_mutex_init(&n->lock, NULL);
  return n;
}

/*Function to initialise a new list*/
void DLL_init(){
    list = (List *)malloc(sizeof(List));
    list->head = NULL;
    list->tail = NULL;
    pthread_mutex_init(&list->h_lock, NULL);
}

int comp(ListNode *prev, ListNode *current) {
  ListNode *tmp = list->head;
  if(prev == NULL) return current == tmp;
  while (tmp->productID <= prev->productID) {
    if (tmp == prev) 
      return (prev->next == current);
    
    tmp = tmp->next;
  }
  return 0;
}
/*Function that inserts in DLL*/
int DLL_insert(int key) {
  ListNode *prev, *current;
  /*Value to check in order to unlock before leaving*/
  int ret_val;
  int exit_val=0;

  if (list->head == NULL) {
    pthread_mutex_lock(&list->h_lock);
    if (list->head == NULL) {
      ListNode *n = DLL_new_node(key);
      list->head = n;
      pthread_mutex_unlock(&list->h_lock);
      return 1;
    }
    pthread_mutex_unlock(&list->h_lock);
  }

  while (!exit_val) {
    prev = NULL;
    current = list->head;
    while (current && current->productID < key) {
      prev = current;
      current = current->next;
    }
    if(prev) {
      pthread_mutex_lock(&prev->lock);
    }
    else{
      pthread_mutex_lock(&list->h_lock);
    } 
    if (current){
      pthread_mutex_lock(&current->lock);
    } 
    if (comp(prev, current) == 1) {
      if (current && (key == current->productID)) {
        ret_val = 0;
      } else {
        ListNode *n = DLL_new_node(key);
        n->next = current;
        if(prev == NULL) list->head = n;
        else prev->next = n;
        ret_val = 1;
      }
      exit_val=1;
    }
    if(prev){
      pthread_mutex_unlock(&prev->lock);
    }
    else{
      pthread_mutex_unlock(&list->h_lock);
    } 
    if(current){
      pthread_mutex_unlock(&current->lock);
    }  
  }
  return ret_val;
}
/*Function to print DLL*/
void DLLprint(){
  ListNode *prev, *current;
  prev = NULL;
  current = list->head;
    while (current) {
      printf("currentent product Id %d\n",current->productID);
      prev = current;
      current = current->next;
    }
}
/*Function to check list size*/
void list_size_check(int N,int expected){
  ListNode *current = list->head;
  int count=0;
  while (current!=NULL) {
    count ++;
    current = current->next;
  }

  if (count == expected) {
     printf("\nList size check (expected: %d, found: %d)\n",expected, count);
  } else {
     printf("\nList size check failed found : %d\n",count);
    exit(-1);
  }
 }
/*Function to check list keysum */
void list_keysum_check(int N){
    int expected=N*N*(N*N-1)/2;
  ListNode *current = list->head;
  int count=0;
  while (current!=NULL) {
    count += current->productID;
    current = current->next;
  }
  if (count == expected) {
    printf("\nList keysum check (expected: %d, found: %d)\n",expected, count);
  } else {
     printf("\nList keysum check failed\n");
    exit(-1);
  }
}
/*Function to delte from DLL*/
int DLL_Delete()
{
    /* must lock the whole list */
     pthread_mutex_lock(&list->h_lock);
    ListNode *temp = list->head;
    pthread_mutex_unlock(&list->h_lock);
    pthread_mutex_lock(&temp->lock);
    if (!temp->next) {
        pthread_mutex_unlock(&temp->lock);
    } else { 
        while (temp->next) {
          pthread_mutex_lock(&temp->next->lock);
            temp = temp->next;
        }
        while (list->head) {
            temp = list->head;
            list->head = temp->next;
            pthread_mutex_unlock(&temp->lock);
        }
    }
    return 0;
}
