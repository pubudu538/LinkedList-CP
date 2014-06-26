#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

struct list_node_s {
   int    data;
   struct list_node_s* next;
};

int nValues;
int operations;
float mMemberOps;
float mInsertOps;
float mDeleteOps;
struct list_node_s* head_p = NULL;
int thread_count;
pthread_rwlock_t rwlock;
pthread_mutex_t count_mutex;
int member_total=0, insert_total=0, delete_total=0;


int  Insert(int value, struct list_node_s** head_p);
int  Member(int value, struct list_node_s* head_p);
int  Delete(int value, struct list_node_s** head_p);
void Get_parameters(int argc, char* argv[]);
void Usage(char* prog_name);
void populateLinkedList(int size);
void performMemberOperation(int operationValue,int n);
void performInsertOperation(int insertOpValue,int n);
void performDeleteOperation(int deleteOpValue,int n);
void* Thread_work(void* rank);



int main(int argc, char* argv[]) {
 
     /* start with empty list */
   double start, finish, elapsed;
   pthread_t* thread_handles;
   long i;

   Get_parameters(argc,argv);
     
   populateLinkedList(nValues);

   thread_handles = malloc(thread_count*sizeof(pthread_t));
   pthread_mutex_init(&count_mutex, NULL);
   pthread_rwlock_init(&rwlock, NULL);
 
   GET_TIME(start);
   for (i = 0; i < thread_count; i++)
   {
      pthread_create(&thread_handles[i], NULL, Thread_work, (void*) i);
   }
      

   for (i = 0; i < thread_count; i++)
   {
      pthread_join(thread_handles[i], NULL);
   }
      
   GET_TIME(finish);

   pthread_rwlock_destroy(&rwlock);
   pthread_mutex_destroy(&count_mutex);
   free(thread_handles);

   elapsed = finish - start;

   printf("Total number of operations - %d\n",operations);
   printf("Total number of Member Operations - %d\n",(int)member_total);
   printf("Total number of Insert Operations - %d\n",(int)insert_total);
   printf("Total number of Delete Operations - %d\n",(int)delete_total);

   printf("Elapsed time = %e seconds\n", elapsed);

   return 0;
} 

void* Thread_work(void* rank)
{

   long my_rank = (long) rank;
   int i;
   double which_op;
   int my_member=0, my_insert=0, my_delete=0;
   int ops_per_thread = operations/thread_count;

   for (i = 0; i < ops_per_thread; i++) {
      which_op = (my_rank*ops_per_thread) + i+1;

      if (which_op <= (double)mMemberOps) {
         pthread_rwlock_rdlock(&rwlock);
         int randomNumber = rand() % nValues;
         Member(randomNumber,head_p);
         pthread_rwlock_unlock(&rwlock);
         my_member++;
      } else if (which_op <= (double)mMemberOps + (double)mInsertOps) {
         pthread_rwlock_wrlock(&rwlock);
         Insert(nValues+i+1,&head_p);
         pthread_rwlock_unlock(&rwlock);
         my_insert++;
      } else { /* delete */
         pthread_rwlock_wrlock(&rwlock);
         int randomNumber = rand() % nValues;
         Delete(randomNumber,&head_p);
         pthread_rwlock_unlock(&rwlock);
         my_delete++;
      }
   }  /* for */

   pthread_mutex_lock(&count_mutex);
   member_total += my_member;
   insert_total += my_insert;
   delete_total += my_delete;
   pthread_mutex_unlock(&count_mutex);

   return NULL;

}


void performDeleteOperation(int deleteOpValue,int n)
{
	int i;

	for (i = 0; i < deleteOpValue; i++)
	{
		int randomNumber = rand() % n;
		Delete(randomNumber,&head_p);
		//printf("%s %d\n","did it",randomNumber);
	}
}

void performInsertOperation(int insertOpValue,int n)
{
	int i;
	for (i = 0; i < insertOpValue; i++)
	{
		Insert(n+i+1,&head_p);
		//printf("%s %d\n","did it - ",n+i+1);
	}

}

void performMemberOperation(int operationValue,int n)
{
	int i;
	for (i = 0; i < operationValue; i++)
	{
		int randomNumber = rand() % n;
		Member(randomNumber,head_p);
		//printf("%s %d %d\n", "did it - ",i,randomNumber);
	}


}

void Get_parameters(int argc, char* argv[])
{

  float memberOps;
  float insertOps;

   if (argc != 6) Usage(argv[0]);

   nValues = strtol(argv[1], NULL, 10);  
   if (nValues <= 0) Usage(argv[0]);

   operations = strtol(argv[2], NULL, 10);
   if (operations <= 0) Usage(argv[0]);

   memberOps = strtod(argv[3],NULL);
   if(memberOps <=0 || memberOps > 1) Usage(argv[0]);

   insertOps = strtod(argv[4],NULL);
   if(insertOps <=0 || insertOps > 1) Usage(argv[0]);

   if(memberOps+insertOps > 1) Usage(argv[0]);

   thread_count = strtol(argv[5],NULL,10);

   mMemberOps = (operations*memberOps);
   mInsertOps = (operations*insertOps);   
   mDeleteOps = operations - mMemberOps - mInsertOps;


}

void populateLinkedList(int size)
{
	int i;

	for (i = 1; i <= size; i++)
	{
		Insert(i,&head_p);
	}
	
}


void Usage(char* prog_name) {
   fprintf(stderr, "usage: %s <n values> <# operations> <mMember> <mInsert> <mDelete>\n", prog_name);
   fprintf(stderr, "   n values is the number of values to be populated \n");
   fprintf(stderr, "   operations is the # of operations to perform \n");
   fprintf(stderr, "   mMemeber is the # of Member operations and should be given as a percentage \n");
   fprintf(stderr, "   mInsert is the # of Insert operations and should be given as a percentage \n");
   fprintf(stderr, "   mDelete is the # of Delete operations and should be given as a percentage \n");
   fprintf(stderr, "   mMemeber + mInsert + mDelete should be equal to 1 \n");
  
   exit(0);
} 

int Insert(int value, struct list_node_s** head_pp) {
   struct list_node_s* curr_p = *head_pp;
   struct list_node_s* pred_p = NULL;
   struct list_node_s* temp_p;
   
   while (curr_p != NULL && curr_p->data < value) {
      pred_p = curr_p;
      curr_p = curr_p->next;
   }

   if (curr_p == NULL || curr_p->data > value) {
      temp_p = malloc(sizeof(struct list_node_s));
      temp_p->data = value;
      temp_p->next = curr_p;
      if (pred_p == NULL)    /* New first node */
         *head_pp = temp_p;
      else
         pred_p->next = temp_p;
      return 1;
   } else { /* value in list */
      return 0;
   }
} 


int  Member(int value, struct list_node_s* head_p) {
   struct list_node_s* curr_p;

   curr_p = head_p;
   while (curr_p != NULL && curr_p->data < value)
      curr_p = curr_p->next;

   if (curr_p == NULL || curr_p->data > value) {
      return 0;
   } else {
      return 1;
   }
} 


int Delete(int value, struct list_node_s** head_pp) {
   struct list_node_s* curr_p = *head_pp;
   struct list_node_s* pred_p = NULL;

   while (curr_p != NULL && curr_p->data < value) {
      pred_p = curr_p;
      curr_p = curr_p->next;
   }
   
   if (curr_p != NULL && curr_p->data == value) {
      if (pred_p == NULL) { /* Deleting first node in list */
         *head_pp = curr_p->next;
         free(curr_p);
      } else { 
         pred_p->next = curr_p->next;
         free(curr_p);
      }
      return 1;
   } else {   /* Value isn't in the list */
      return 0;
   }
}