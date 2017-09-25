

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
/* 
BUGS
1. numberOfProcessesCompleted is getting set back to 0 or is never incrementing even though I have a line that increments the number of processes
*/
typedef struct {
	int name; //Name of the process 1
	int startPN; //starting priority number DOES NOT CHANGE 2
	int currPN; //current priority number 3
	int timeInCPU; //time needed in the cpu DOES NOT CHANGE 4
	int timeInIO; //time needed in I/O DOES NOT CHANGE 5
	int machineTime; //time needed in the CPU + time needed in I/O 6
	int currTimeInCPU; //current time in CPU 7
	int currTimeInIO; //current time in I/O 8
	int timeinReadyQueue; //time process has been in the ready queue 9
	int currTotalTimeinCPU; // current total time in CPU for all iterations 10
	int currTotalTimeinIO; //current total time in I/O for all iterations 11
	int totalTimeinCPU; //total time in CPU for all iterations DOES NOT CHANGE 12
	int totalTimeInIO; //total time in I/O for all iterations DOES NOT CHANGE 13
	int totalTimeinReadyQueue; //total time in ready queue for all iterations 14
	int minTimeInReadyQueue; //minimum time in ready queue 15
	int maxTimeInReadyQueue; //maximum time in ready queue 16
	int whereItIs; //tell where the process is located; 17
} Process;

typedef struct nodes {
	Process process; //hold the sturct processs data
	struct nodes *next; //pointer to next
}node;

typedef struct lists {
	node *head; //pointer to head
	node *tail; //pointer to tail
}list;

//this is an arry of processes with all of the data initialized
Process processes[] = {
		{0, 14, 14, 7, 3, 0, 0, 0, 0, 0, 0, 14, 6, 0, 0, 0, 0},
		{1, 3, 3, 8, 5, 0, 0, 0, 0, 0, 0, 16, 10, 0, 0, 0, 0},
		{2, 6, 6, 2, 3, 0, 0, 0, 0, 0, 0, 4, 6, 0, 0, 0, 0},
		{3, 1, 1, 14, 3, 0, 0, 0, 0, 0, 0, 28, 6, 0, 0, 0, 0},
		{4, 4, 4, 13, 12, 0, 0, 0, 0, 0, 0, 26, 24, 0, 0, 0, 0},
		{5, 5, 5, 7, 9, 0, 0, 0, 0, 0, 0, 14, 18, 0, 0, 0, 0},
		{6, 1, 1, 8, 11, 0, 0, 0, 0, 0, 0, 16, 22, 0, 0, 0, 0}		
};

//prototypes
void add(list * dest, Process p);
void printQueue(list *src);
void delete(list *dest);
void Sort(list *dest);
void check4Aging(list *dest);
void increment4RQ(list *rq);
void increment4CPU(list *cpu);
void increment4IO(list *io);
int checkCPU(list *cpu, list *rq, list *io, int TQ);
int checkIO(list *cpu, list *rq, list *io, int numberOfProcessesCompleted);

int main() {
	//The lists 
	list RQ;
	list IOQ;
	list CPU;
	//set all of the lists heads and tails to NULL
	RQ.head = NULL;
	RQ.tail = NULL;
	CPU.head = NULL;
	CPU.tail = NULL;
	IOQ.head = NULL;
	IOQ.tail = NULL;
	int numberOfProcesses = 7;
	int numberOfProcessesCompleted = 0;
	int RQsize = 0;
	int tq = 5;
	//while there are still processes that are not completed
	while(numberOfProcessesCompleted != 7) {
		if(numberOfProcesses > 0) { //if we haven't added all of the processes to the ready queue
			Process temp = processes[RQsize]; //add process data to temp
			add(&RQ,temp); //add temp to the ready queue
			RQsize++; 
			numberOfProcesses--;
		}
		printQueue(&RQ); //prints ready queue

		check4Aging(&RQ); //checks for aging

		Sort(&RQ); //sorts the ready queue
		printQueue(&RQ); //prints ready queue

		increment4RQ(&RQ); //increments ready queue variables
		
		increment4CPU(&CPU); //increments CPU variables
		
		increment4IO(&IOQ); //increments I/O variables
		
		tq = checkCPU(&CPU, &RQ, &IOQ, tq); //checks various CPU conditions

		numberOfProcessesCompleted = checkIO(&CPU, &RQ, &IOQ, numberOfProcessesCompleted); //checks various I/O conditions

		printf("Number of processses completed is %d\n",numberOfProcessesCompleted);
	}
	printf("ALL PROCESSES COMPLETED\n");
	return 0;
}
	
// This add method was modeled off of Hardin's ll.c add method
void add(list * dest, Process p) {  
	if (dest->head == NULL) { //if queue is empty
		// first node
		dest->head = dest->tail = malloc(sizeof(node)); // allocate memory for head and tail
		dest->head->process = p; //store process data into head
		dest->head->next = dest->tail->next = NULL; // set head and tail's next node to NULL
	}
	else {
		node * temp; 
		temp = malloc(sizeof(node)); //allocate memory for temp 
		temp->next = NULL; //set the node next to temp to null
		temp->process = p;  //store proess data into temp
		dest->tail->next = temp; //set the temp to the back of the queue
		dest->tail = temp;  //change tail to point to temp
	}
}
// This add method was from Hardin's ll.c print method
void printQueue(list *src) {
		node * temp;
		temp = src->head;
		while (temp) {  // follow list
			printf("%d ", temp->process.name); //prints the name of the list
			temp = temp->next; //move to next item in queue
		} // end follow list
		printf("\n");
}

// Got help from Matt Mazzerela with logic of delete method
void delete(list *dest) {
	if(dest->head == NULL) { // if queue is empty. tell user that the queue is empty
		printf("Queue is empty\n");
	}
	else {
		printf("Something is in the Queue\n"); // tell them that there is something in the queue
		if(dest->head->next != NULL) { // if there is more than 2 items in the queue
			printf("We have at least 2 things in the queue\n"); //tell user that there are at least two things in the queue
			node *temp = dest->head->next; // set temp to point to head's next node
			dest->head->process = dest->head->next->process; //set head's next nodes proccess data to be the head's process data. NOTE: WORKS WITHOUT THIS LINE
			dest->head->next = temp->next; //set head's next node to point the temp's next node
		}
		else { //if we only have one item in the queue
			printf("We only have head\n"); // tell user that we have only 1 item in the queue
			dest->head = NULL; // set head to null
		}
	}
}
//modeled this sorting algorithm from https://www.daniweb.com/programming/software-development/threads/419177/selection-sort-linked-list
void Sort(list *dest) {
    node *h = dest->head, *i, *j, *next_i;
    for(i = h; i!=NULL && i->next!=NULL; i=i->next) //these double for loops are for traversing the linked list so that every value can be checked
    {
        node *min; 
        min = i;
        for(j = i->next; j!=NULL ; j=j->next)
        {
            if(j->process.currPN < min->process.currPN) //if process j's priority number is smaller than min's priority number
                min=j;
        }
        if(min!=i)
        {
        	//swaps the values of min and i
            Process temp;
            temp = min->process;
            min->process = i->process;
            i->process = temp;
        }
    }
    dest->head = h;
}

void check4Aging(list *dest) {
	node *temp;
	temp = dest->head;
	while(temp) {
		if(temp->process.timeinReadyQueue == 10 && temp->process.currPN != 0) { //if time in ready queue is 10 and the current priority number isn't already 0
			temp->process.currPN--; 
			printf("%d's priority number change to %d\n",temp->process.name, temp->process.currPN);//tell user priority number has decreased
			temp->process.timeinReadyQueue = 0; //set time in ready queue back to 0
		}
		temp = temp->next; //go to next node
	}
}

void increment4RQ(list *rq) {
	node *temp;
	temp = rq->head;
	if(temp == NULL) { //if ready queue is empty, notify user
		printf("Ready Queue is empty\n");
	}
	while(temp) { //this loop will go through each node in the ready queue and increment the appropriate variables
		temp->process.timeinReadyQueue++;
		printf("Time in ready queue is %d\n",temp->process.timeinReadyQueue);

		temp->process.totalTimeinReadyQueue++;
		printf("Total time in ready queue is %d\n",temp->process.totalTimeinReadyQueue);

		temp = temp->next;
	}
}

void increment4CPU(list *cpu) {
	node *temp;
	temp = cpu->head;
	if(temp == NULL) { //if cpu queue is empty, notify user
		printf("CPU Queue is empty\n");
	}
	else {
		while(temp) { //this loop will go through each node in the ready queue and increment the appropriate variables
			temp->process.currTimeInCPU++;
			printf("Current time in CPU is %d\n", temp->process.currTimeInCPU);
	
			temp->process.currTotalTimeinCPU++;
			printf("Current total time in CPU is %d\n", temp->process.currTotalTimeinCPU);
	
			temp->process.machineTime++;
			printf("Current machine time is %d\n", temp->process.machineTime);
	
			temp = temp->next;
		}
	}
}

void increment4IO(list *io) {
	node *temp;
	temp = io->head;
	if(temp == NULL) { //if I/O queue is empty, notify user
		printf("I/O Queue is empty\n");
	}
	else {
		while(temp) { //this loop will go through each node in the ready queue and increment the appropriate variables
			temp->process.currTimeInIO++;
			printf("Current time in I/O is %d\n",temp->process.currTimeInIO);
	
			temp->process.currTotalTimeinIO++;
			printf("Current total time in I/O is %d\n",temp->process.currTotalTimeinIO);
	
			temp->process.machineTime++;
			printf("Total time in CPU and I/O is %d\n", temp->process.machineTime);
	
			temp = temp->next;
		}
	}
}

int checkCPU(list *cpu, list *rq, list *io, int TQ) {
	if(cpu->head == NULL) { //if cpu is empty
		if(rq->head == NULL) { //if ready queue is empty, we can't add anything to the cpu
			printf("Ready Queue is empty\n");
		}
		else {
			rq->head->process.timeinReadyQueue = 0; //set time in ready queue for head node to 0
			add(cpu,rq->head->process); //add first item of ready queue to cpu
			delete(rq); //delete first item of ready queue
			printf("Added Process %d to the CPU\n", cpu->head->process.name); //tell user item was added to cpu
		}
	}
	else {
		if(TQ == 0) { //if a time quantum has occured
			add(rq,cpu->head->process); //add process back to the ready queue
			delete(cpu); //delete the process from the CPU
			printf("Time Quantum has Occured!!\n"); //notify user
			TQ = 5; //set time quantum back to 5
			return TQ; //return that value
		}
		if(cpu->head->process.currTimeInCPU == cpu->head->process.timeInCPU) { //if process is done running in the CPU
			cpu->head->process.currTimeInCPU = 0; //set current CPU time back to 0
			printf("Process %d went to I/O queue\n",cpu->head->process.name); //notify user that the process is in I/O
			add(io,cpu->head->process); //add process to I/O queue
			delete(cpu); //delete processs from CPU
		}
	}
	TQ--; 
	return TQ;
}

int checkIO(list *cpu, list *rq, list *io, int numberOfProcessesCompleted) {
	node *temp;
	Process temp2;
	temp = io->head;
	if(temp == NULL) {
		printf("Nothing in I/O\n");
	}
	else {
	while(temp) { //this while loop will execute until it reaches NULL
		//if the process is complete
		if(temp->process.currTotalTimeinCPU == temp->process.totalTimeinCPU && temp->process.currTotalTimeinIO == temp->process.totalTimeInIO) {
			//swap the values of temp and head
			temp2 = io->head->process; 
			io->head->process = temp->process;
			temp->process = temp2;
			printf("Process %d is complete\n", temp->process.name); //notify user that the process is completed
			delete(io); //delete process from I/O queue
			numberOfProcessesCompleted++; //increment number of processes BUG: NOT WORKING!!!!
			printf("%d\n",numberOfProcessesCompleted);
			return numberOfProcessesCompleted; 
		}
		if(temp->process.currTimeInIO == temp->process.timeInIO) { //if process is done in I/O
			temp->process.currTimeInIO = 0;
			printf("%d\n", temp->process.timeInIO);
			printf("Process %d is moving back to the ready queue\n", temp->process.name);
			add(rq,temp->process);//add processs back into the ready queue
			//swap the values of head and temp
			temp2 = io->head->process; 
			io->head->process = temp->process;
			temp->process = temp2;
			printf("Process %d is being deleted from the io queue\n",io->head->process.name); //notify that the process has been moved from I/O to ready queue
			io->head->process.currTimeInIO = 0;
			delete(io); //delete process from I/O
		}
		temp = temp->next; //move to next node
	}
	}
	return numberOfProcessesCompleted;
}


