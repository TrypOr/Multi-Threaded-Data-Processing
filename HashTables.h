/**
 * @file project_1.c
 * @author Orestis Trypidakis 4235
 * @brief Hash table header of project
 * @date 2022-12-06
 * 
 */
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
/*Initialise struct HTNode*/
struct HTNode{
 int productID;
 pthread_mutex_t lock;
};
/*Set N and M as global variables*/
int M,N;
typedef struct HTNode HTNode;
/*Main structure for hashtable a 2D array of structs*/
HTNode ***HT;
/*Variable to set where each thread starts*/
int *start;
/*Function that initialises the hash table*/
int create_tables(int size) {
    N=size*3;
    M=size;
    int buckets=N*4;
    start=malloc(buckets*sizeof(int));
    HT=malloc(M * sizeof(HT) );
    int i,j;
    srand(time(NULL));
    for(i=0;i<size;i++)
    {
        HT[i]=malloc(buckets*sizeof(**HT));
         for(j=0;j<buckets;j++){
            HT[i][j] = malloc (sizeof(*** HT));
             HT[i][j]->productID=-1;
            pthread_mutex_init(& HT[i][j]->lock, NULL);
        }   
    }
    return 0;
}
/*Hash function 1*/
int hashcode1(int key) {
   return (key%M);
}
/*Hash function 2*/
int hashcode2(int key)
{
	return (key%3)+1;
}
/*Function that initialises start variable*/
int init_start(int thread_id){
    start[thread_id]= thread_id % M;
}
/*Hash table instertion*/
int HTInsert(int key,int thread_id){   
    /*Hash key with hashcode 1 and hashcode 2*/
    int hash1=hashcode1(key);
    int hash2=hashcode2(key);
    int index,exit_val=0;
    int i=0;
    /*If i mod M = M set as 0*/
    if(start[thread_id]==M){
        start[thread_id]=0;
    }
    /*Loop until position to insert key is found*/
    while(i<N*4 && exit_val==0){
        /*Double hash*/
        index=(hash1+i*hash2);
        hash1=hashcode1(key);
        /*Lock to perform checks*/
        pthread_mutex_lock(&HT[start[thread_id]][index]->lock);
         if(HT[start[thread_id]][index]->productID !=-1){
        }
        if(HT[start[thread_id]][index]->productID==-1){
            HT[start[thread_id]][index]->productID=key;
            exit_val=1;
        }
         pthread_mutex_unlock(&HT[start[thread_id]][index]->lock);
        i++; 
    }

    /*Circle the M tables*/
    start[thread_id]++; 
    
    
}
/*Print hash table*/
void HTprint(){
    int i,j;
    for(i=0;i<M;i++){
        for(j=0;j<N*4;j++){
            printf("HT[%d][%d]=%d\n",i,j,HT[i][j]->productID);
        }
    }
}
/*Function to check hash table size*/
void HT_size_check(int expected){
    int i,j;
    int count=0;
    for(i=0;i<M;i++){
        count=0;
        for(j=0;j<N*4;j++){
            if(HT[i][j]->productID!=-1){
                count++;
            }
        }
    if (count == expected) {
        printf("\n HT[%d] size check (expected: %d, found: %d)\n",i,expected, count);
    } else {
        printf("\nHT[%d] size check failed\n",i);
        exit(-1);
    }
 }
}
/*Function to check hast table keysum*/
void HT_keysum_check(int expected){
    int i,j;
    int count=0;
    for(i=0;i<M;i++){
        for(j=0;j<N*4;j++){
            if(HT[i][j]->productID!=-1){
                count+=HT[i][j]->productID;
            }
        }
    }
  if (count == expected) {
    printf("\n HT keysum check (expected: %d, found: %d)\n",expected, count);
  } else {
     printf("\n HT keysum check failed\n");
    exit(-1);
  }
}
/*Hash table deletion*/
int HTDelete(int key,int thread_id){   
    int hash1=hashcode1(key);
    int hash2=hashcode2(key);
    int index;
    int i=0;
    int temp,exit_val=0;
    /*Random number to delete*/
    int random=rand()%(N*4);
     if(start[thread_id]==M){
        start[thread_id]=0;
    }
    /*Loop until a position to delte is found*/
    while(exit_val==0){
        pthread_mutex_lock(&HT[start[thread_id]][random]->lock);
        if(HT[start[thread_id]][random]->productID!=-1){
            temp=HT[start[thread_id]][random]->productID;
            //printf("Thread %d Removed %d key in [%d][%d]\n",thread_id, HT[start[thread_id]][index]->productID,start[thread_id],random);
            HT[start[thread_id]][random]->productID=-1;
            exit_val=1;
        }
        pthread_mutex_unlock(&HT[start[thread_id]][random]->lock);
        i++;
        random=rand()%(N*4); 
    }
   
    /*Circle the M tables*/
    start[thread_id]++; 
    return temp; 
}