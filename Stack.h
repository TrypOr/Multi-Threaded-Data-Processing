/**
 * @file project_1.c
 * @author Orestis Trypidakis 4235
 * @brief Stack header of project
 * @date 2022-12-06
 * 
 */
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
/*Initialise global lock for CAS*/
pthread_mutex_t global_lock;
struct stackNode{
 int productID;
 struct stackNode *next;
};

typedef struct stackNode stackNode;
/*Initialise top of stack*/
stackNode* top = NULL;
struct Stack {
    stackNode *head;
};
typedef struct Stack Stack;
/*Initialise lock*/
int init_stack(int N){
    pthread_mutex_init(&global_lock,NULL);
}
/*Function that pushes in stack*/
int push(int productID)
  {
    stackNode *new =malloc(sizeof(stackNode));
    new->productID=productID;
    pthread_mutex_lock(&global_lock);
    if (top == NULL) {
        new->next = NULL;
    } else {
        new->next = top; 
    }
    top = new; 
    pthread_mutex_unlock(&global_lock);
 return 0;
 }
 /*Function that pops in stack*/
int pop() {
    int ret_val;
    pthread_mutex_lock(&global_lock);
    if (top == NULL) {
        printf("\n Stack is empty");
        ret_val=-1;
    } else {
        struct stackNode *temp = top;
        ret_val = top->productID; 
        top = top->next;
    }
    pthread_mutex_unlock(&global_lock);
    return ret_val;
    
}
/*Function that prints stack for debugging*/
void print_stack() {
    if (top == NULL) {
        printf("\n Stack is empty\n");
    } else {
        printf("The stack is \n");
        struct stackNode *temp = top;
        while (temp->next != NULL) {
            printf("%d--->", temp->productID);
            temp = temp->next;
        }
    }
}
/*Function to check stack size*/
void Stack_size_check(int expected){
    int i,j;
    int count=1;
    struct stackNode *temp = top;
        while (temp->next != NULL) {
            count++;
            temp = temp->next;
        }
  if (count == expected) {
    printf("\nStack size check (expected: %d, found: %d)\n",expected, count);
  } else {
     printf("\n Stack size check failed found:%d\n",count);
    exit(-1);
  }
}