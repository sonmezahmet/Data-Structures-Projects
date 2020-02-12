#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define maxTrees (12)
#define capacity (4095)
#define infinity (6000000000L)
#define Error(Str)	FatalError(Str)
#define FatalError(Str)	fprintf(stderr, "%s\n", Str)

typedef struct binNode {
	char name[10];
	int e;
	int arrivalTime;
	int waitingTime;
	int isFinished;
	int isArrived;
	double priorityValue;
    struct binNode* LeftChild;
    struct binNode* NextSibling;
} binNode;
typedef binNode* binTree;

typedef struct collection {
    int currentSize;
    binTree theTrees[maxTrees];
} collection;
typedef collection* binQueue;

binQueue merge(binQueue H1, binQueue H2);

binQueue initialize(void) {
    binQueue H;
    int i;

    H = malloc(sizeof(collection));
    if(H == NULL)
        FatalError( "Out of space!!!" );
    H->currentSize = 0;
    for(i = 0; i < maxTrees; i++)
        H->theTrees[i] = NULL;
    return H;
}

static void destroyTree(binTree T) {
    if(T != NULL) {
    	destroyTree(T->LeftChild);
        destroyTree(T->NextSibling);
        free(T);
    }
}

void destroy(binQueue H) {
    int i;

    for(i = 0 ; i < maxTrees ; i++)
        destroyTree(H->theTrees[i]);
}

binQueue makeEmpty( binQueue H ) {
    int i;

    destroy(H);
    for(i = 0; i < maxTrees; i++)
        H->theTrees[i] = NULL;
    H->currentSize = 0;

	return H;
}

binQueue insert(char name[], int e, int arrivalTime, double priorityValue, binQueue H) {
    binTree newNode;
    binQueue oneItem;
	int i;
	
    newNode = malloc(sizeof(binNode));
    if(newNode == NULL)
        FatalError( "Out of space!!!" );
    newNode->LeftChild = newNode->NextSibling = NULL;
    strcpy(newNode->name, name);
    newNode->e = e;
    newNode->arrivalTime = arrivalTime;
    newNode->waitingTime = 0;
	newNode->isFinished = 0;
	newNode->priorityValue = priorityValue;
	newNode->isArrived = 0;
	
    oneItem = initialize();
    oneItem->currentSize = 1;
    oneItem->theTrees[0] = newNode;
    
	return merge(H, oneItem);
}

binTree deleteMin(binQueue H) {
    int i, j;
    int minTree;   /* The tree with the minimum item */
    binTree minBinTree = malloc(sizeof(binNode));
	binQueue deletedQueue;
    binTree deletedTree, oldRoot;
    double minItem;

    if(isEmpty(H)) {
        Error("Empty binomial queue");
		minItem=-infinity;
        return NULL;
    }

    minItem = infinity;
    for(i = 0 ; i < maxTrees ; i++) {
        if(H->theTrees[i] && H->theTrees[i]->priorityValue <= minItem) {
        	if (H->theTrees[i]->priorityValue == minItem && H->theTrees[i]->arrivalTime > H->theTrees[minTree]->arrivalTime) {
				continue;
			}
            /* Update minimum */
            minItem = H->theTrees[i]->priorityValue;
			minTree = i;
			strcpy(minBinTree->name, H->theTrees[i]->name);
			minBinTree->e = H->theTrees[i]->e;
			minBinTree->isFinished = H->theTrees[i]->isFinished;
			minBinTree->LeftChild = H->theTrees[i]->LeftChild;
			minBinTree->NextSibling = H->theTrees[i]->NextSibling;
			minBinTree->priorityValue = H->theTrees[i]->priorityValue;
			minBinTree->waitingTime = H->theTrees[i]->waitingTime;
			minBinTree->isArrived = H->theTrees[i]->isArrived;
        }
    }
    
    deletedTree = H->theTrees[minTree];
    oldRoot = deletedTree;
   	deletedTree = deletedTree->LeftChild;
	free(oldRoot);

    deletedQueue = initialize();
    deletedQueue->currentSize = (1 << minTree) - 1;
    for(j = minTree - 1 ; j >= 0; j--) {
        deletedQueue->theTrees[j] = deletedTree;
        deletedTree = deletedTree->NextSibling;
        deletedQueue->theTrees[j]->NextSibling = NULL;
    }

    H->theTrees[minTree] = NULL;
    H->currentSize -= deletedQueue->currentSize + 1;

    merge(H, deletedQueue);
    return minBinTree;
}

double findMin(binQueue H) {
    int i;
    double minItem;

    if(isEmpty(H)) {
        Error( "Empty binomial queue" );
		minItem=0;
        return minItem;
    }

    minItem = infinity;
			
    for(i = 0 ; i < maxTrees ; i++) {
        if(H->theTrees[i] && H->theTrees[i]->priorityValue < minItem) {
            minItem = H->theTrees[i]->priorityValue;
		}	
    }

    return minItem;
}

int isEmpty(binQueue H) {
    return H->currentSize == 0;
}

int isFull(binQueue H) {
    return H->currentSize == capacity;
}

binTree combineTrees(binTree T1, binTree T2) {
    if(T1->priorityValue > T2->priorityValue){
    	return combineTrees(T2, T1);
	}
    else if(T1->priorityValue == T2->priorityValue && T1->arrivalTime > T2->arrivalTime){
   	T1->NextSibling = T2->LeftChild;
    T2->LeftChild = T1;
    return T2;
	}
    T2->NextSibling = T1->LeftChild;
    T1->LeftChild = T2;
    return T1;
}

binQueue merge(binQueue H1, binQueue H2) {
    binTree T1, T2, Carry = NULL;
    int i, j;

    if(H1->currentSize + H2->currentSize > capacity)
        Error( "Merge would exceed capacity" );

    H1->currentSize += H2->currentSize;
    for(i = 0, j = 1 ; j <= H1->currentSize ; i++, j *= 2) {
        T1 = H1->theTrees[i]; T2 = H2->theTrees[i];

        switch(!!T1 + 2 * !!T2 + 4 * !!Carry) {
            case 0: /* No trees */
            case 1: /* Only H1 */
                break;
            case 2: /* Only H2 */
                H1->theTrees[i] = T2;
                H2->theTrees[i] = NULL;
                break;
            case 4: /* Only Carry */
                H1->theTrees[i] = Carry;
                Carry = NULL;
                break;
            case 3: /* H1 and H2 */
                Carry = combineTrees(T1, T2);
                H1->theTrees[i] = H2->theTrees[i] = NULL;
                break;
            case 5: /* H1 and Carry */
                Carry = combineTrees(T1, Carry);
                H1->theTrees[i] = NULL;
                break;
            case 6: /* H2 and Carry */
                Carry = combineTrees(T2, Carry);
                H2->theTrees[i] = NULL;
                break;
            case 7: /* All three */
                H1->theTrees[i] = Carry;
                Carry = combineTrees(T1, T2);
                H2->theTrees[i] = NULL;
                break;
        }
    }
    return H1;
}

int finishProcess(binTree currentProcess, binTree arr[], int size);
int printWaitingTimes(binTree arr[], int size, int q);

int main(void) {
	/*binQueue head = initialize();
	head = insert("p1", 3, 0, 9, head);
	head = insert("p2", 1, 2, 7, head);
	
	
	binTree del = deleteMin(head);
	printf("%s\n", del->name);
	head = insert("p3", 2, 3, 12, head);
	del = deleteMin(head);
	printf("%s\n", del->name);
	head = insert(del->name, del->e, del->arrivalTime, del->priorityValue, head);
	del = deleteMin(head);
	printf("%s\n", del->name);*/ //Binomial heap test (Working)
	
	//Read input.txt
	FILE *fPtr, *fPtr2;
	char processId[10];
	int e, arrivalTime, waitingTime, eMax, eTotal = 0, counter = 0;
	
	//Determine how many process there are and eMax and eTotal
	fPtr = fopen("input.txt", "r");
	while (fscanf(fPtr, "%s %d %d", processId, &e, &arrivalTime) != EOF) {
		counter++;
		eTotal += e;
		if (e > eMax) {
			eMax = e;
		}
	}
	fclose(fPtr);
	
	//Create processArray
	binTree processArray[counter];
	counter = 0;
	
	//Fill in processArray
	fPtr2 = fopen("input.txt", "r");
	while (fscanf(fPtr2, "%s %d %d", processId, &e, &arrivalTime) != EOF) {
		binTree newProcess = malloc(sizeof(binNode));
		
		strcpy(newProcess->name, processId);
		newProcess->e = e;
		newProcess->arrivalTime = arrivalTime;
		newProcess->priorityValue = e;
		newProcess->waitingTime = 0;
		newProcess->isFinished = 0;
		newProcess->isArrived = 0;
		
		processArray[counter++] = newProcess;
	}

	/*int i;
	for (i = 0 ; i < counter ; i++) {
		printf("id:%s, e:%d, arrivalTime:%d\n", processArray[i]->name, processArray[i]->e, processArray[i]->arrivalTime);
	}*/ //processArray test (Working)
	

	
	int q, t, index;
	for (q = 1 ; q <= 10 ; q++) {
		//Initialize binomial heaps
		binQueue mpHead = initialize();
		
		for (t = 0 ; t < eTotal ; t++) {
			int i;
			for (i = 0 ; i < counter ; i++) {
				if (processArray[i]->isArrived == 0 && processArray[i]->arrivalTime <= t){
					mpHead = insert(processArray[i]->name, processArray[i]->e, processArray[i]->arrivalTime, processArray[i]->priorityValue, mpHead);
					processArray[i]->isArrived = 1;
				}
			}
			
			binTree currentProcess = deleteMin(mpHead);
			
			if(currentProcess->e > q) {
				currentProcess->e -= q;
				//calculate new priority value
				float newPriorityValue = (float) (2 * currentProcess->e) / (float) (3 * eMax);
				newPriorityValue = pow(newPriorityValue, 3);
				newPriorityValue = exp(-newPriorityValue);
				newPriorityValue = pow(newPriorityValue, -1);
				currentProcess->priorityValue = newPriorityValue * currentProcess->e;
				mpHead = insert(currentProcess->name, currentProcess->e, currentProcess->arrivalTime, currentProcess->priorityValue, mpHead);
				t += q - 1;
			} else {
				//printf("%s\n", currentProcess->name);
				finishProcess(currentProcess, processArray, counter);
				int i, e, arrivalTime;
				for (i = 0 ; i < counter ; i++) {
					if (strcmp(processArray[i]->name, currentProcess->name) == 0){
						e = processArray[i]->e;
						arrivalTime = processArray[i]->arrivalTime;
					
						break;
					}
				}
				t += currentProcess->e - 1;
				//printf("%s, e: %d, at: %d, t:%d\n", processArray[i]->name, e, arrivalTime, t);
				int waitingTime = t - (e + arrivalTime) + 1;
				if(waitingTime < 0){
					waitingTime = 0;
				}
				
				processArray[i]->waitingTime = waitingTime;
			}
		}
		destroy(mpHead);
		
		//print Waiting Times
		//printf("q = %d\n", q);
		printWaitingTimes(processArray, counter, q);
		
		//Reset array
		int i;
		for (i = 0 ; i < counter ; i++) {
			processArray[i]->isArrived = processArray[i]->isFinished = processArray[i]->waitingTime = 0;
		}
	}
	printf("Printed to output.txt");
	return 0;
}

int finishProcess(binTree currentProcess, binTree arr[], int size) {
	int i;
	for (i = 0 ; i < size ; i++) {
		if(arr[i]->isArrived && strcmp(arr[i]->name, currentProcess->name) == 0){
			//printf("%s is finished\n", arr[i]->name);
			arr[i]->isFinished = 1;
		}
	}
}

int printWaitingTimes(binTree arr[], int size, int q) {
	FILE *output = fopen("output.txt", "a");
	fprintf(output, "q=%d\n", q);
	int i, total = 0;
	for (i = 0 ; i < size; i++) {
		fprintf(output, "%s\t%d\n", arr[i]->name, arr[i]->waitingTime);
		total += arr[i]->waitingTime;
	}
	fprintf(output, "AWT = %d/%d = %.3f\n", total, size, (float) total / (float) size);
}
