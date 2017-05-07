#include <sys/types.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define NUMPROCESS 9 


int alarmflag = 0;
struct process
{
	pid_t pid;
	struct process *next;
};
struct semaphore
{

	int value;
	struct process* front;
	struct process* rear;
};

void alarmHandler()
{
	alarmflag = 1;
}

void initialize(struct semaphore *S)
{
	S->value = 1;
	S->front = NULL;
}

void Enqueue(struct semaphore *S, pid_t p)
{
	struct process* temp = (struct process*) malloc(sizeof(struct process));
	temp->next = NULL;
	if(S->front == NULL)
	{
		S->front = S->rear = temp;
		return;
	}
	
		S->rear->next = temp;
		S->rear = temp;
	
}
void Dequeue(struct semaphore *S){
	struct process* temp = S->front;
	if(S->front == NULL)
	{
		return;
	}
	if(S->front == S->rear){
		S->front = S->rear = NULL;
	}
	else{
		S->front = S->front->next;
	}
 	free(temp);
}

int Front(struct semaphore *S)
{
	if(S->front == NULL)
	{
		printf("Queue empty\n");
		return;
	}
	return S->value;
}
int down(struct semaphore *S, pid_t p)
{
	S->value--;
	if(S->value < 0)
	{
		Enqueue(S,p);
		while(!alarmflag)
		{
			pause();
			return 1;
		}
	}
 return 1;
}
void up(struct semaphore *S)
{
	S->value++;
	if(S->value <= 0)
	{
		Dequeue(S);
		alarm(1);
	}
}

int critical_section(int loop_limit, int who_called)
{
	printf("\n");
	while(loop_limit > 0)
	{
		printf("\nProcess ID: %d Loop Index:%d", who_called, loop_limit);
		loop_limit--;
	}	
	printf("\nReturning to caller process:%d\n", who_called);
	return 0;

}


int main(){

	pid_t child;
	int count = 0;
	int turn = mmap(NULL,sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1,0);
	int *flag;
	flag = mmap(NULL, sizeof(int)*2, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1,0);
	struct semaphore *S = (struct semaphore*) malloc(sizeof(struct semaphore));;
	initialize(S);

	for(int i = 0; i < NUMPROCESS && (child = fork());)
	{
		if(child < 0)
		{	
			printf("fork error");
			exit(-1);
		}
		if (child == 0)
			break;
		else
		{
			i++;
		}
	}
	int processnum = getpid();

	flag[0] = 0;
	flag[1] = 0;
	turn;
	// creates child process
	if(child > 0)
	{
		if(down(S,child));
		{
			critical_section(10,getpid());
		}
		up(S);
	
	}

	if(child == 0)
	{
		if(down(S,child));
		{
			critical_section(10,getpid());
		}
		up(S);
	}
	
return 0;

}



