/**
 * @file project_1.c
 * @author Orestis Trypidakis 4235
 * @brief Main of project
 * @date 2022-12-06
 * 
 */
#include <stdlib.h>
#include "DLL.h"
#include "HashTables.h"
#include "Stack.h"
/*Create barriers*/
pthread_barrier_t barrier_start_1st_phase;
pthread_barrier_t barrier_start_2nd_phase;
pthread_barrier_t barrier_end_2nd_phase;
pthread_barrier_t barrier_start_3rd_phase;
pthread_barrier_t barrier_end_3rd_phase;
pthread_barrier_t barrier_start_4th_phase;
pthread_barrier_t barrier_end_4th_phase;

/*Create N,Μ as global variable*/
int N,M;
/*Function to create Ids for double linked list*/
int create_DLL_ProductId(int thread_id, int prev_id) {
  int ProductId;
  if (prev_id == -1) {
    ProductId = thread_id;
  } else if (prev_id == thread_id) {
    ProductId = N + thread_id;
  } else {
    ProductId = prev_id - thread_id;
    ProductId = N + ProductId + thread_id;
  }
   //printf("Thread %d created id %d\n", thread_id, ProductId);
  return ProductId;
}
/*Function to create Ids for Hash table*/
int create_HT_ProductId(int thread_id, int prev_id){
int ProductId;
  if (prev_id == -1) {
    ProductId = N *thread_id;
  }else if (prev_id == thread_id && thread_id!=0) {
    ProductId = N * thread_id;
  }else{
    ProductId = prev_id + 1;
  }
  return ProductId;
}
/*Main routine for threads in program*/
void *routine(void* arg)
{
    int productId=-1,i;
    int thread_id = *(int*)arg;
    /*N threads insert products in DLL*/
    for (i = 0; i < N; i++) {
    productId = create_DLL_ProductId(thread_id, productId);
    if(DLL_insert(productId)==-1){
      printf("Empty list\n");
    }
  }
  /*Wait for all threads to finish*/
    pthread_barrier_wait(&barrier_start_1st_phase);
    /*Thread 0 performs the checks*/
    if (thread_id == 0) {
        list_size_check(N,N*N);
        list_keysum_check(N);
        DLL_Delete();
      //printf("Thread 0 finished 1st phase workload\n");
  }
  /*Wait for all threads to finish*/
  pthread_barrier_wait(&barrier_start_2nd_phase);
  productId=-1;
  /*init_start function to initialise where each thread will start inserting*/
    init_start(thread_id);
    /*N threads insert in M Hash tables*/
    for (i = 0; i < N; i++) {
    productId = create_HT_ProductId(thread_id, productId);
    HTInsert(productId,thread_id);
  }
  /*Wait for all threads to finish*/
  pthread_barrier_wait(&barrier_end_2nd_phase);
  /*Thread 0 performs the checks*/
  if (thread_id == 0) {
      HT_size_check(3*N);
      HT_keysum_check(N*N*(N*N-1)/2);
      //printf("Thread 0 finished 2nd phase workload\n");
  }
  pthread_barrier_wait(&barrier_start_3rd_phase);
   /*init_start function to initialise where each thread will start deleting*/
  init_start(thread_id);
  /*Delete M products from Hash table*/
  for (i = 0; i < M; i++) {
      push(HTDelete(productId,thread_id));
  }
  pthread_barrier_wait(&barrier_end_3rd_phase);
  /*Thread 0 performs the checks*/
  if (thread_id == 0) {
      HT_size_check(2*N);
      Stack_size_check((N*N)/3);
      //printf("Thread 0 finished 4th phase workload\n");
  }
  pthread_barrier_wait(&barrier_start_4th_phase);
  /*Pop N/3 from stack and insert in DLL*/
  for(i=0;i<N/3;i++){
    DLL_insert(pop());
  }
  /*Wait for all threads to finish*/
   pthread_barrier_wait(&barrier_end_4th_phase);
   /*Thread 0 performs the checks*/
   if (thread_id == 0) {
      list_size_check(N,((N*N)/3));
      //printf("Thread 0 finished 4th phase workload\n");
  }
    return 0;
}
int main(int argc, char * argv[]){
    int i;
    /*Check if arguements are correct*/
    if (argc != 2) {
        printf("Missing arguments\n");
        exit(0);
    }
    N=atoi(argv[1]);
    printf("Argument N = %d\n",N);
    pthread_t threads[N];
    M=N/3;
    /*Initialise barriers */
    pthread_barrier_init(&barrier_start_1st_phase, NULL, N);
    pthread_barrier_init(&barrier_start_2nd_phase, NULL, N);
    pthread_barrier_init(&barrier_end_2nd_phase, NULL, N);
    pthread_barrier_init(&barrier_start_3rd_phase, NULL, N);
    pthread_barrier_init(&barrier_end_3rd_phase, NULL, N);
    pthread_barrier_init(&barrier_start_4th_phase, NULL, N);
    pthread_barrier_init(&barrier_end_4th_phase, NULL, N);
    /*Initialise Double linked list*/
    DLL_init();
     /*Initialise Hash table*/
    create_tables(M);
    /*Create all the threads*/
    for (i = 0; i < N; i++) {
      int* a = malloc(sizeof(int));
      *a = i;
        pthread_create(&threads[i], NULL, routine, a);
    }
    for (i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }
}