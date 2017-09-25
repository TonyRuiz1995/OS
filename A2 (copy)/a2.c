#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

/*
Programmer: Antonio Ruiz
Hardin CS4540
Assignment 2 Process Scheduler information
*/

/*
Program Objectives:
A)Define at least 3 * 16 processes with different starting times

B)Add one to the ready queue, run the loop at least once then add the next.

C)loop until all processes exit. Each iteration is one time unit.Adjust time counters in each process

D) swap processes as necessary from CPU to either the I/O or ready queue

E)swap processes from I/O to ready queues when I/O time is up <------------------this one was not complete

F)Adjust priority of processes in ready queue so no processes waits too long and so higher priorities run first.

G) After the loop stops and all process have exited print the statistics for

               the OS setup for time limits

each process’s stats including total, min and max times

All I/O times should be > 2
*/


/*Objective A)
Create a struct for a Process PCB and make an array of these objects.
*/


typedef struct {
	
	int name; //Name of process
	int startPrio; //Starting priority, does not change.
	int currPrio; //Current priority of the process which may change with aging.
	int cpuTime; //Time in CPU needed before/between I/O (set once)
	int ioTime; //Time I/O takes (set once)
	int machineTime; //Sum of total time in CPU and total time in I/O.
	int currCPUTime; //Time in CPU currently.  Set to 0 when moved into CPU, when reaches A or quantum is up moves out of CPU.
	int currIOTime; // Time left waiting for current I/O
	int timeInRQ; //Time process has been waiting in ready queue
	int currTotalCPU; //current time in cpu (all iterations)
	int currTotalIO; //current tume in I/O (all iterations)
	int totalCPU;//Total Time in CPU all iterations[static]
	int totalIO;//Total Time in IO all iterations [static]
	int totalTimeInRQ; //Total time in ready queue
} Process;


typedef struct nodes {
	Process process; //hold the sturct processs data
	struct nodes *next; //pointer to next
}node;

typedef struct lists {
	node *head; //pointer to head
	node *tail; //pointer to tail
}list;


Process processes[] = {
	{1,1,3,4,9,0,0,0,0,0,0,0,0},
	{2,4,3,8,4,0,0,0,0,0,0,0,0},
	{3,12,14,7,12,0,0,0,0,0,0,0,0},
	{4,10,2,9,11,0,0,0,0,0,0,0,0},
	{5,11,13,6,14,0,0,0,0,0,0,0,0},
	{6,12,1,4,6,0,0,0,0,0,0,0,0}

};

void print(list * queue);
void add(list * queue, Process p);
void delete(list * queue);
void sort(list * queue);
void incrementCPU(list * queue);
void incrementIO(list * queue);
void incrementRQ(list * queue);
void checkAging(list * queue);
int checkIO(list * cpu, list * io, list * rq, int completedProcs);
int checkCPU(list * cpu, list * io, list * rq, int quanta);

int main(){

	list ready;
	list cpu;
	list io;
	
	ready.head = NULL;
	ready.tail = NULL;
	cpu.head = NULL;
	cpu.tail = NULL;
	io.head = NULL;
	io.tail = NULL;
	
	int numProcs = 6;
	int completedProcs = 0;
	int totalCycles = 0;
		
	int readyQueueSize = 0;//process in ready state
	int quanta = 8;//quanta of 4
	
	while(completedProcs != 6){
		
		if(numProcs > 0){//start adding them to ready queue
			Process temp = processes[readyQueueSize];
			add(&ready, temp);
			readyQueueSize++;//increment the size of ready queue
			numProcs--;//number of processes of availble decrements
			printf("%d PROCESSES LEFTS\n", numProcs);
			
		}//end if
		
		if(ready.head != NULL){
			printf("----CHECKING FOR AGING ON READY QUEUE--------------\n");
			checkAging(&ready);
			sort(&ready);//after aging is checked, priorities might be changed, must sort

		}
		

		printf("----------------PRINTING READY QUEUE--------------\n");
		print(&ready);
	        //D) swap processes as necessary from CPU to either the I/O or ready queue
		incrementRQ(&ready);
		incrementCPU(&cpu);
		incrementIO(&io);
		
		quanta = checkCPU(&cpu,&io,&ready, quanta);//do a cpu cycle
		completedProcs = checkIO(&cpu,&io,&ready,completedProcs);//see what went to io
		//checkIO(&cpu,&io,&ready,completedProcs);
		printf("Quanta is : %d\n",quanta);
		totalCycles++;
		printf("Completed Procs is : %d\n",completedProcs);
		printf("TOTAL CYCLES: %d,\n",totalCycles);
	//C)loop until all processes exit. Each iteration is one time unit.Adjust time counters in each process
	
	}//end while
	
	printf("FIN");
	return 0;
}
//hello



//B)Add one to the ready queue, run the loop at least once then add the next.
//I.e need methods to add to the queues, delete, etc...
//help from http://www.sanfoundry.com/c-program-queue-using-linked-list/

void add(list * queue, Process p){
	//if we are dealing with an empty queue
	if(queue->head == NULL){
		queue->head = queue->tail = malloc(sizeof(node)); //allocate memory for node
		queue->head->process = p;//store p into head
		printf("Added Process %d to the queue\n", p.name);
		queue->head->next = queue->tail->next = NULL;//set head next node and tails next node to null
		queue->head = queue->tail;			
	}
	else{
		node * temp;
		temp = malloc(sizeof(node));
		temp->next = NULL;
		temp->process = p;
		queue->tail->next = temp;
		queue->tail = temp;
		queue->tail->next = NULL;
		printf("Added Process %d to the tail of queue\n", queue->tail->process.name);
	}

	
}

void print(list * queue){

	node * temp;
	temp = queue->head;
	if(temp == NULL){
	printf("PRINT FAILED-QUEUE IS EMPTY\n");
	return;
	}
	while(temp){
	
	
		printf("%d\n", temp->process.name);
		temp = temp->next;
	}//end while
}//end print

//kudos go to http://www.sanfoundry.com/c-program-doubly-linked-list/

void delete(list *dest){
		
		if(dest->head== NULL){
		
			printf("DELETION FAILED-QUEUE EMPTY\n");
		}
		else{
		
			printf("DELETING\n");
			if(dest->head->next != NULL){
			
				node *temp = dest->head->next;
				dest->head = temp;
				temp->next= dest->head->next;
			}//end inner if
			else{
			printf("AFTER THIS DELETION, QUEUE IS EMPTY\n");
			dest->head = NULL;
			
			}//end inner else
		
		}//end else


}//end delete()




//kudos go to http://www.sanfoundry.com/c-program-doubly-linked-list/
void sort(list * src){

	int i, j, x;

	node * temp1 = src->head;
	node * temp2 = src->head;
	if(temp1 == NULL){
		printf("List is empty\n");
		return;
	}//end if
	
	for(temp1 = src->head; temp1 != NULL; temp1 = temp1->next){
	
		for(temp2 = temp1->next; temp2 != NULL; temp2 = temp2->next){
			if(temp1->process.currPrio > temp2->process.currPrio){
				x = temp1->process.currPrio;
				temp1->process.currPrio = temp2->process.currPrio;
				temp2->process.currPrio = x;
			
			}//end if
		}//end inner for
	
	}//end outer for
}

//kudos go to http://www.sanfoundry.com/c-program-doubly-linked-list/
//need to be able to sort IO queue based on times
void sortIO(list * src){
	printf("SORTING IO\n");
	int i, j, x;

	node * temp1 = src->head;
	node * temp2 = src->head;
	node * temp3 = malloc(sizeof(node));
	if(temp1 == NULL){
		printf("List is empty\n");
		return;
	}//end if
	
	for(temp1 = src->head; temp1 != NULL; temp1 = temp1->next){
		for(temp2 = temp1->next; temp2 != NULL; temp2 = temp2->next){
		//HERE I REDIFINE WHAT THE CURRENT PRIO IS FOR A PROCESS IN THE IO QUEUE
		//THE PROCESSES ARE CLOSEST TO COMPLETION WILL BE THE FIRST TO BE DELETED
		temp1->process.currPrio = temp1->process.ioTime - temp1->process.currTotalIO;
	        temp2->process.currPrio = temp2->process.ioTime - temp2->process.currTotalIO;
			if(temp1->process.currPrio > temp2->process.currPrio){
				temp3->process = temp1->process;
				temp1->process = temp2->process;
				temp2->process = temp3->process;
			
			}//end if
		}//end inner for
	
	}//end outer for
	print(src);
}
//go through each node in the ready queue and change priorities if need be
void checkAging(list * src){

	if(src->head == NULL){
	printf("CHECK AGING FAILED--NOTHING IN HERE\n");
	return;
	}
	node *temp;
	temp = src->head;
	
	
	while(temp){
	
		if(temp->process.timeInRQ == 10 && temp->process.currPrio !=0){
			//cant decrement a prio that is 0 
			temp->process.currPrio--;
			printf("ID %d has a new prio number of %d\n", temp->process.name, temp->process.currPrio);
			temp->process.timeInRQ = 0;
		}//end if
		
		temp = temp->next;
	}//end while

}


void incrementRQ(list * src){

	node *temp;
	temp = src->head;
	
	if(temp == NULL){
		printf("Ready Queue is empty\n");
	}
	
	while(temp){//go through our ready queue and increment the nodes
		
		temp->process.timeInRQ++;
		printf("ID %d has been the Ready Queue for %d seconds\n", temp->process.name, temp->process.timeInRQ);	
		temp->process.totalTimeInRQ++;
		printf("ID %d has been the Ready Queue for %d seconds\n", temp->process.name, temp->process.totalTimeInRQ);	
		
		temp = temp->next;
		
	}

}

void incrementCPU(list *src){
	
	node *temp;
	temp = src->head;
	
	if(temp == NULL){
	
		printf("CPU Queue is empty\n");
		return;
	}
	else{
	while(temp){//go through our ready queue and increment the nodes
			printf("--------------CPU STATS----------------\n");
			temp->process.currCPUTime++;
			printf("ID %d has been the cpu for %d seconds\n", temp->process.name, temp->process.currCPUTime);	
			temp->process.currTotalCPU++;
			printf("ID %d has total CPU time of %d seconds\n", temp->process.name, temp->process.currTotalCPU);
			temp->process.machineTime++;
			printf("ID %d has machine time of %d seconds\n", temp->process.name, temp->process.machineTime);
			printf("TIME PROCESS %d NEEDS TO BE IN CPU : %d\n",temp->process.name,temp->process.cpuTime);
			printf("----------------------------------------\n");	
			temp = temp->next;
		}
	}
	

}//end incrementCPU method

void incrementIO(list *src){
	
	node *temp;
	temp = src->head;
	
	if(temp == NULL){
	
		printf("IO Queue is empty\n");
		return;
	}
	else{
		while(temp){//go through our ready queue and increment the nodes
			printf("----------------IO STATS------------------------\n");	
			temp->process.currIOTime++;
			printf("ID %d has been the IO Queue for %d seconds\n", temp->process.name, temp->process.currIOTime);	
			temp->process.currTotalIO++;
			printf("ID %d has total io time of %d seconds\n", temp->process.name, temp->process.currTotalIO);
			temp->process.machineTime++;
			printf("ID %d has machine time of %d seconds\n", temp->process.name, temp->process.machineTime);
			printf("PROCESS %d needs to be in the IO for %d\n",temp->process.name,temp->process.ioTime);	
			printf("--------------------------------------------\n");	
			temp = temp->next;	
		}
	}
}//end incrementCPU method


int checkCPU(list * cpu, list * io, list * rq, int quanta){
	//one cpu cycle
	if(cpu->head == NULL){
		if(rq->head == NULL){
			printf("CPU queue is empty\n");
		}//end inner if
		else{
		//cpu is empty, but rq has something in it, lets run those processes
		rq->head->process.currCPUTime = 0;
		add(cpu, rq->head->process);
		delete(rq);
		}//end inner else
	}//end cpu if
	
	else{
		
		 if(cpu->head->process.currTotalCPU == cpu->head->process.cpuTime){ //process is done
			cpu->head->process.currCPUTime = 0;//reset
			printf("Process %d is finished\n", cpu->head->process.name);
			add(io,cpu->head->process);
			delete(cpu);
			quanta = 8;//reset back to 8


		}
		else if(quanta == 0){ //process has had its fun, back to ready queue
			add(rq, cpu->head->process);
			delete(cpu);
			printf("Time quantum has occurred\n");//notify of a time quantum
			quanta = 8;
			return quanta;
		}
		
			quanta--;
		
	}//end cpu else

	return quanta;
}

int checkIO(list * cpu, list * io, list * rq, int completeProcs){
	

	node *temp;
	Process x;
	temp = io->head;

	if(temp == NULL){
		printf("Nothing in I/O\n");	
	}//end if
	
	else{

		sortIO(io);
		while(temp){

			if(temp->process.currIOTime == temp->process.ioTime){
				//io ran, she is done
				printf("PROCESS %d is complete\n",temp->process.name);
				delete(io);
				completeProcs++;
				printf("COMPLETED PROCESSES: %d\n", completeProcs);
			}
			temp = temp->next;
		}
	
	}
	return completeProcs;
}
