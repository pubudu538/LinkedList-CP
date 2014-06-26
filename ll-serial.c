#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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
const int MaxValue = 50000;


int  Insert(int value, struct list_node_s** head_p);
int  Member(int value, struct list_node_s* head_p);
int  Delete(int value, struct list_node_s** head_p);
void Get_parameters(int argc, char* argv[]);
void Usage(char* prog_name);
int * populateLinkedList();
void performMemberOperation(int operationValue,int n);
void performInsertOperation(int insertOpValue,int n,int *num);
void performDeleteOperation(int deleteOpValue,int n,int *num);



int main(int argc, char* argv[]) {
 
     /* start with empty list */
   double start, finish, elapsed;
   int *numbers;

   Get_parameters(argc,argv); 
   numbers = populateLinkedList();
 
   GET_TIME(start);
   performMemberOperation((int)mMemberOps,nValues);
   performInsertOperation((int)mInsertOps,nValues,numbers);
   performDeleteOperation((int)mDeleteOps,nValues,numbers);
   GET_TIME(finish);

   elapsed = finish - start;

   printf("Total number of operations - %d\n",operations);
   printf("Total number of Member Operations - %d\n",(int)mMemberOps);
   printf("Total number of Insert Operations - %d\n",(int)mInsertOps);
   printf("Total number of Delete Operations - %d\n",(int)mDeleteOps);

   printf("Elapsed time = %e seconds\n", elapsed);

   return 0;
} 

void performDeleteOperation(int deleteOpValue,int n,int *num)
{
	int i,value;

	for (i = 0; i < deleteOpValue; i++)
	{
		value = *(num+i);
		Delete(value,&head_p);
		//printf("%s %d\n","did it",randomNumber);
	}
}

void performInsertOperation(int insertOpValue,int n,int *num)
{
	int i,value;
	for (i = 0; i < insertOpValue; i++)
	{
      value = *(num+i+nValues);
		Insert(value,&head_p);
		//printf("%s %d\n","did it - ",n+i+1);
	}


}

void performMemberOperation(int operationValue,int n)
{
	int i;
	for (i = 0; i < operationValue; i++)
	{
		int randomNumber = rand() % MaxValue;
		Member(randomNumber,head_p);
		//printf("%s %d %d\n", "did it - ",i,randomNumber);
	}


}

void Get_parameters(int argc, char* argv[])
{

  float memberOps;
  float insertOps;

   if (argc != 5) Usage(argv[0]);

   nValues = strtol(argv[1], NULL, 10);  
   if (nValues <= 0) Usage(argv[0]);

   operations = strtol(argv[2], NULL, 10);
   if (operations <= 0) Usage(argv[0]);

   memberOps = strtod(argv[3],NULL);
   if(memberOps <=0 || memberOps > 1) Usage(argv[0]);

   insertOps = strtod(argv[4],NULL);
   if(insertOps <=0 || insertOps > 1) Usage(argv[0]);

   if(memberOps+insertOps > 1) Usage(argv[0]);

   mMemberOps = (operations*memberOps);
   mInsertOps = (operations*insertOps);   
   mDeleteOps = operations - mMemberOps - mInsertOps;


}

int * populateLinkedList()
{
	int randomNum,returnValue,count=0;
   static int numArray[3500];
   int size = nValues+(int)mInsertOps+(int)mDeleteOps;

	while (count < size)
	{
      randomNum = rand() % MaxValue;
      returnValue = Member(randomNum,head_p);

      if(returnValue == 0)
      {
            Insert(randomNum,&head_p);
            numArray[count]=randomNum;
            count++;

      }
		
	}
	
   return numArray;
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