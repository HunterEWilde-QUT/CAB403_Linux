/* Program:  roundRobinScheduling_Skeleton.c
 * Purpose:  Both the Ready Queue and the Job Queue are implemented as Linked Lists. 
 * Initial Job Queue: (A, 178)-->(B, 83)-->(C, 166)-->(D, 169)-->(E, 193)
 * Process is taken from the head of the Job queue (A, 178) and 
 * assigned a time slice on the CPU equal to the time quantum (20ms).
 * Process A is added to the tail of the Ready queue (A,20). As 
 * process A burst time is greater than allocated time quantum (20ms) 
 * the process is added back to the tail of the job queue.
 * Job queue now (B, 83)-->(C, 166)-->(D, 169)-->(E, 193)-->(A, 158)
 * Ready Queue now (A,20)
 * Process B taken from Job queue assigned time slice and then returned
 * to tail of Job Queue as process not completed. State of both linked lists now:
 * Job Queue: (C, 166)-->(D, 169)-->(E, 193)-->(A, 158)-->(B, 63)
 * Ready Queue: (A, 20)-->(B,20)
 *================================================================================*/

#include <stdlib.h>
#include <stdio.h>

typedef struct process {
    char processID;
    int burstTime;
    struct process *nextNode;
} node;

node *jobQueueHead = NULL; /* Job queue head */
node *jobQueueTail = NULL; /* Job queue tail */

node *readyQueueHead = NULL; /* Ready queue head */
node *readyQueueTail = NULL; /* Ready queue tail */

void printLinkedList(node *);
void insertNodeToJobQueue(node *);
node *removeJobQueueHeadNode(void);
void insertNodeToReadyQueue(node *);

int main() {
    node *processRep;
    node *jobQueueProcess;
    node *readyQueueProcess;

    int quantum = 20; // time quantum

    /* initializing the ready queue for RR */
    for (int i = 1; i <= 5; i++) {
        processRep = malloc(sizeof(node));
        processRep->processID = 64 + i;
        processRep->burstTime = (int)((double)(99) * rand() / (999999999 + 1.0));
        processRep->nextNode = NULL;
        insertNodeToJobQueue(processRep);
    }

    printf("The Job Queue Processes to be executed using a Round Robin Scheduling Algorithm: \n\n");
    printLinkedList(jobQueueHead);
    printf("\n\n");

    /* the RR scheduling algorithm, insert your solution here for task1 */
    
    /* Create a loop until the Job queue is empty */
    while (jobQueueHead != NULL) {
        jobQueueProcess = removeJobQueueHeadNode(); /* Get the next job in the queue; move jobQueueHead to next job */
        // Copy the contents of the next job for entry into the ready queue
        readyQueueProcess = malloc(sizeof(node));
        readyQueueProcess->processID = jobQueueProcess->processID;
        if (jobQueueProcess->burstTime >= quantum) {
            readyQueueProcess->burstTime = quantum; /* Set the burst time of the ready process to the time quantum */
        } else {
            readyQueueProcess->burstTime = jobQueueProcess->burstTime; /* Copy the burst time if smaller than quantum */
        }

        insertNodeToReadyQueue(readyQueueProcess); /* Insert the ready process into the ready queue */
        //free(readyQueueProcess); /* Deallocate memory */
        //readyQueueProcess = NULL;

        jobQueueProcess->burstTime -= quantum; /* Decrement the job process's burst time by the time quantum */

        // Reinsert the decremented job process into the tail of the job queue, if there's still time left
        if (jobQueueProcess->burstTime > 0) {
            insertNodeToJobQueue(jobQueueProcess);
        }
    }
    
    printf("The resulting RR schedule is: \n\n");
    printLinkedList(readyQueueHead);
    printf("\n\n");

    return 0;
}

/*Function to insert Node (Process) back to the tail
* of the Job Queue - Job Queue is a circular queue
* \param jobQueueNode - node to be added to tail of
*        Job Queue
*/
void insertNodeToJobQueue(node *jobQueueNode) {
    if (jobQueueTail == NULL)  {/* empty list */
        jobQueueHead = jobQueueNode;
    } else {
        jobQueueTail->nextNode = jobQueueNode;
    }
    jobQueueTail = jobQueueNode;
}

/* Function returns the next node to be taken from the
 * head of the Job Queue
 */
node *removeJobQueueHeadNode() {
    node *headOfJobQueue;
    if (jobQueueHead == NULL)  {/* list empty */
        return (NULL);
    }

    headOfJobQueue = jobQueueHead;
    if (jobQueueHead == jobQueueTail)  {/* only one element in the list */
        jobQueueHead = jobQueueTail = NULL;
    } else {
        jobQueueHead = jobQueueHead->nextNode;
    }
    return (headOfJobQueue);
}

/* Function inserts a new node to the tail
 * of the Ready Queue representing a process
 * allocated a time slice on the CPU
 * \param newReadyQueueNode: node to add to tail of Ready Queue
 */
void insertNodeToReadyQueue(node *newReadyQueueNode) {
    if (readyQueueTail == NULL)  {/* empty list */
        readyQueueHead = newReadyQueueNode;
    } else {
        readyQueueTail->nextNode = newReadyQueueNode;
    }
    readyQueueTail = newReadyQueueNode;
}

/* Helper function to print out a linked list
 * \param headNode: the head of the linked list to be printed
 */
void printLinkedList(node *headOfLinkedList) {
    if (headOfLinkedList == NULL) {
        printf("^\n");
    } else {
        while (headOfLinkedList->nextNode) {
            printf("(%c, %d) --> ", headOfLinkedList->processID, headOfLinkedList->burstTime);
            headOfLinkedList = headOfLinkedList->nextNode;
        }
        printf("(%c, %d) ^\n", headOfLinkedList->processID, headOfLinkedList->burstTime);
    }
}
