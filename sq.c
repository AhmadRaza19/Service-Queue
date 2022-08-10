#include <stdio.h>
#include <stdlib.h>
#include "sq.h"

typedef struct node{
  int val;
  struct node* next;
  struct node* prev;
} NODE;

typedef struct list{
  NODE* front;
  NODE* back;
} LIST;

typedef struct arr{
  int currSize;    
  int totalSize; 
  NODE** arrs;  
} ARRAY;

typedef struct service_queue{
  int buzNum;
  ARRAY* arr;
  LIST* queue;
  LIST* buzzer;
} SQ;

//Initialize Array
ARRAY* init(){
  ARRAY* arr = (ARRAY*) malloc (sizeof(ARRAY));
  arr->arrs = (NODE**) calloc (4, sizeof(NODE*));
  arr->currSize = 0;
  arr->totalSize = 4;
  return arr;
}

//Initialize List
LIST* init_list(){
  LIST* l = malloc(sizeof(LIST));
  l->front = NULL;
  l->back = NULL;
  return l;
}

//Resize Array
void resize_array(ARRAY* arr, NODE* N){
  if (arr->currSize >= arr->totalSize){
    arr->totalSize = arr->totalSize * 2;
    NODE** buzz = realloc(arr->arrs, sizeof(NODE*) * arr->totalSize);
    arr->arrs = buzz;
  }
  arr->arrs[N->val] = N;
}

//Push value to front
void push_front(LIST *l, int val){
  NODE* p = (NODE*) malloc (sizeof(NODE));
  p->val = val;
  p->next = l->front; 
  p->prev = NULL;
  l->front = p;
  if(l->back != NULL){
    p->next->prev = p;
  }else{
     l->back = p;
  }
}

//Push value to back
void push_back(LIST *l, int value){
  if(l->back != NULL){
    NODE* p = (NODE*) malloc (sizeof(NODE));
    p->val = value;
    p->next = NULL;
    p->prev = l->back;
    l->back->next = p;
    l->back = p;
    return;
    
  } else { 
    push_front(l, value);
    return;
  }
}

//Remove value from front
int pop_front(LIST *l) {
  int temp;
  NODE *n1;
  if(l->front == NULL)
	  return -1; 

  temp = l->back->val;
  
  if(l->front != l->back) 
  {  
     n1 = l->front;  
    l->front = l->front->next; 
    l->front->prev = NULL;
    free(n1);
  } 
  else 
  {
     free(l->front);
    l->front = NULL;
    l->back = NULL;
   
  }
  return temp;
}

//Creates service queue
SQ * sq_create() {
  SQ* sq = (SQ*) malloc (sizeof(SQ));
  sq->arr = init();
  sq->queue = init_list();
  sq->buzzer = init_list();
  sq->buzNum = -1;
  return sq;
}

//Frees service queue
void sq_free(SQ *q) {
  NODE *n1 = q->queue->front;
  NODE *n2;
  while(n1 != NULL) {
    n2 = n1->next;   
    free(n1);
    n1 = n2;
  }
  free(q->queue);
  NODE *temp = q->buzzer->front;
  
  while(temp != NULL) {
    n2 = temp->next;   
    free(temp);
    temp = n2;
  }
  free(q->buzzer);
  free(q->arr->arrs);
  free(q->arr);
  free(q);
}

//Displays service queue
void sq_display(SQ *q) {
  NODE * current = q->queue->front;
  while(current != NULL){
     printf("current-queue contents:\n    ");
      printf("%d ", current->val);
    current = current->next;
  }
}

//Returns current size
int  sq_length(SQ *q) {
  return q->arr->currSize;
}

//Returns value at buzzer
int  sq_give_buzzer(SQ *q){
  if (q->buzzer->front != NULL) {
      int val = q->buzzer->front->val;
      pop_front(q->buzzer);
      push_back(q->queue, val);
    } else {
      push_back(q->queue, q->buzNum + 1);
    }
    resize_array(q->arr, q->queue->back);

    if (q->queue->back->val > q->buzNum) {
      q->buzNum = q->queue->back->val;
    }
    q->arr->currSize++;
    return q->queue->back->val;
}        

//Seats a buzzer
int sq_seat(SQ *q) {
  if (q->queue->front != NULL) { 
    NODE* st = q->queue->front;
    int bz = st->val;

    pop_front(q->queue);
    push_front(q->buzzer, bz);
    q->arr->currSize--;

    if (bz == q->buzNum && q->buzNum > 0) {
      q->buzNum--;
    }
    return bz;
  }
  return -1;   
}

//Kicks out a buzzer
int sq_kick_out(SQ *q, int buzzer) 
{
  NODE* nd = q->arr->arrs[buzzer];
  if (nd == NULL || nd->val > q->buzNum || q->queue->front == NULL) {
    return 0;
  }
  if (nd == q->queue->front) {
    sq_seat(q);
  }else{
    if (nd == q->queue->back) {
     q->queue->back = q->queue->back->prev;
     q->queue->back->next = NULL;
    }else{  
      nd->prev->next =nd->next;  
      nd->next->prev = nd->prev;
    }
    push_front(q->buzzer, nd->val);
    q->arr->arrs[buzzer] = NULL;
    free(nd);
    q->arr->currSize--;
  }
  return 1;
}

//Takes a bribe
int sq_take_bribe(SQ *q, int buzzer) 
{
  NODE* num = q->arr->arrs[buzzer];
  if (num == NULL || num->val > q->buzNum) {
    return 0;
  }
  if (q->queue->front == NULL){
    return 0;
  }
  sq_kick_out(q, buzzer);

  int temp = q->buzzer->front->val;

  pop_front(q->buzzer);
  push_front(q->queue, temp);
  resize_array(q->arr, q->queue->front);
  q->arr->currSize++;
  return 1;
}