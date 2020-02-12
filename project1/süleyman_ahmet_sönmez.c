#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <math.h>

// Struct definitions

typedef struct termAddress {
	void *termAddress;
	struct termAddress *nextPtr;
} termAddress;
typedef termAddress* termAddressPtr;
typedef termAddress** termAddressPtrPtr;

typedef struct documentInfo {
	char categoryName[20];
	char documentName[20];
	int occurences;
	struct documentInfo *nextPtr;
} documentInfo;
typedef documentInfo* documentInfoPtr;
typedef documentInfo** documentInfoPtrPtr;

typedef struct term {
	char term[1024];
	documentInfo *documents;
	termAddress *firstOrders;
	termAddress *secondOrders;
	termAddress *thirdOrders;
	int firstCategoryOccurences;
	int secondCategoryOccurences;
	int thirdCategoryOccurences;
	float idf;
	struct term *nextPtr;
} term;
typedef term* termPtr;
typedef term** termPtrPtr;

typedef struct categoryInfo {
	char categoryName[20];
	termAddress *mostFrequentTerms;
} categoryInfo;
typedef categoryInfo* categoryInfoPtr;
typedef categoryInfo** categoryInfoPtrPtr;

// Function prototypes
int insertTerm (termPtrPtr headerOfTerm, char t[], char categoryName[], char documentName[],  categoryInfo categories[]);
int insertDocumentInfo (documentInfoPtrPtr headerOfDocumentInfo, char categoryName[], char documentName[]);
int findFirstOrders (termPtr headerOfTerm);
int insertFirstOrder (termAddressPtrPtr headerOfTermAddress1, termAddressPtrPtr headerOfTermAddress2, termPtr term1, termPtr term2);
int findSecondOrder (termPtr headerOfTerm);
int insertSecondOrder (termAddressPtrPtr headerOfTermAdress, termPtr term);
int findThirdOrder (termPtr headerOfTerm);
void deleteTermAddress (termAddressPtrPtr headerOfTermAddress, termPtr key);
void printSecondOrders(termPtr headerOfTerm);
void printThirdOrders (termPtr headerOfTerm);
int findMostFrequentWords (termPtr headerOfTerm, termAddressPtrPtr cat1, termAddressPtrPtr cat2, termAddressPtrPtr cat3);
void printFirstCategoryOccurences (termAddressPtr headerOfTermAddress, categoryInfo category);
void printSecondCategoryOccurences (termAddressPtr headerOfTermAddress, categoryInfo category);
void printThirdCategoryOccurences (termAddressPtr headerOfTermAddress, categoryInfo category);
void printFirstCategoryIdf (termAddressPtr headerOfTermAddress, int numberOfDocuments, categoryInfo category);
void printSecondCategoryIdf (termAddressPtr headerOfTermAddress, int numberOfDocuments, categoryInfo category);
void printThirdCategoryIdf (termAddressPtr headerOfTermAddress, int numberOfDocuments, categoryInfo category);
void printMLL (termPtr headerOfTerm);


int main() {
	
	termPtr headerOfTerm = NULL;
	categoryInfo categories[3];
	
	// Determine the category names
	char categoryNames[3][20];

	struct dirent *dirent;
	
	DIR *directory = opendir("dataset");
	
	if (directory == NULL) {
		printf("Could not open the dataset");
		return 0;
	}
	
	int i = 0;
	while ((dirent = readdir(directory)) != NULL) {
		
		if (strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..") == 0){
			continue;
		}

		strcpy(categoryNames[i++], dirent->d_name);
	}
	
	closedir(directory);
	
	
	// Read first category' s text files
	
	categoryInfo cat1; // Create categoryInfo object
	strcpy(cat1.categoryName, categoryNames[0]);
	cat1.mostFrequentTerms = NULL;
	categories[0] = cat1;
	
	int firstCategoryDocNumber = 0; 
	
	char directoryPath1[40] = "dataset/"; // Get relative path
	strcat(directoryPath1, categoryNames[0]);
	DIR *firstCategory = opendir(directoryPath1);
	
	if (firstCategory == NULL) {
		printf("Could not open the first category");
		return 0;
	}
	
	while ((dirent = readdir(firstCategory)) != NULL) {
		
		if (strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..") == 0){
			continue;
		}
		
		firstCategoryDocNumber++;
		
		char filePath[40];
		sprintf(filePath, "%s/%s", directoryPath1, dirent->d_name);
		FILE *filePtr = fopen(filePath, "r");
		
		if (filePtr != NULL){
			
			char term[1024];
			
			while (!feof(filePtr)) {
				// Read terms and add the MLL
				fscanf(filePtr, "%1023s", term);
				insertTerm(&headerOfTerm, term, categoryNames[0], dirent->d_name, categories);
			}
			
			fclose(filePtr);
		}
	}
	
	
	// Read second category' s text files
	
	categoryInfo cat2; // Create categoryInfo object
	strcpy(cat2.categoryName, categoryNames[1]);
	cat2.mostFrequentTerms = NULL;
	categories[1] = cat2;
	
	int secondCategoryDocNumber = 0;
	
	char directoryPath2[40] = "dataset/"; // Get relative path
	strcat(directoryPath2, categoryNames[1]);
	DIR *secondCategory = opendir(directoryPath2);
	
	if (secondCategory == NULL) {
		printf("Could not open the first category");
		return 0;
	}
	
	while ((dirent = readdir(secondCategory)) != NULL) {
		
		if (strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..") == 0){
			continue;
		}
		
		secondCategoryDocNumber++;
		
		char filePath[40];
		sprintf(filePath, "%s/%s", directoryPath2, dirent->d_name);
		FILE *filePtr = fopen(filePath, "r");
		
		if (filePtr != NULL){
			
			char term[1024];
			
			while (!feof(filePtr)) {
				// Read terms and add the MLL
				fscanf(filePtr, "%1023s", term);
				insertTerm(&headerOfTerm, term, categoryNames[1], dirent->d_name, categories);
			}
			
			fclose(filePtr);
		}
	}

	
	// Read third category' s text files
	
	categoryInfo cat3; // Create categoryInfo object
	strcpy(cat3.categoryName, categoryNames[2]);
	cat3.mostFrequentTerms = NULL;
	categories[2] = cat3;
	
	int thirdCategoryDocNumber = 0;
	
	char directoryPath3[40] = "dataset/"; // Get relative path
	strcat(directoryPath3, categoryNames[2]);
	DIR *thirdCategory = opendir(directoryPath3);
	
	if (thirdCategory == NULL) {
		printf("Could not open the first category");
		return 0;
	}
	
	while ((dirent = readdir(thirdCategory)) != NULL) {
		
		if (strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..") == 0){
			continue;
		}
		
		thirdCategoryDocNumber++;
		
		char filePath[40];
		sprintf(filePath, "%s/%s", directoryPath3, dirent->d_name);
		FILE *filePtr = fopen(filePath, "r");
		
		if (filePtr != NULL){
			
			char term[1024];
			
			while (!feof(filePtr)) {
				// Read terms and add the MLL
				fscanf(filePtr, "%1023s", term);
				insertTerm(&headerOfTerm, term, categoryNames[2], dirent->d_name, categories);
			}
			
			fclose(filePtr);
		}
	}
	
	printf("Output file is writing...\n");
	findFirstOrders(headerOfTerm);
	findSecondOrder(headerOfTerm);
	findThirdOrder(headerOfTerm);
	printSecondOrders(headerOfTerm);
	printThirdOrders(headerOfTerm);
	findMostFrequentWords(headerOfTerm, &(categories[0].mostFrequentTerms), &(categories[1].mostFrequentTerms), &(categories[2].mostFrequentTerms));
	printFirstCategoryOccurences(categories[0].mostFrequentTerms, categories[0]);
	printSecondCategoryOccurences(categories[1].mostFrequentTerms, categories[1]);
	printThirdCategoryOccurences(categories[2].mostFrequentTerms, categories[2]);
	printFirstCategoryIdf(categories[0].mostFrequentTerms, firstCategoryDocNumber, categories[0]);
	printSecondCategoryIdf(categories[1].mostFrequentTerms, secondCategoryDocNumber, categories[1]);
	printThirdCategoryIdf(categories[2].mostFrequentTerms, thirdCategoryDocNumber, categories[2]);
	printf("Output file is writed...");
	
	return 0;
}

int insertTerm (termPtrPtr headerOfTerm, char t[], char categoryName[], char documentName[], categoryInfo categories[]) {
	
	// Allocate memory
	termPtr newTerm = (termPtr) malloc(sizeof(term));
	
	strcpy(newTerm->term, t);
	newTerm->documents = NULL;
	newTerm->firstOrders = NULL;
	newTerm->secondOrders = NULL;
	newTerm->thirdOrders = NULL;
	newTerm->firstCategoryOccurences = 0;
	newTerm->secondCategoryOccurences = 0;
	newTerm->thirdCategoryOccurences = 0;
	newTerm->idf = 0;
	
	if (strcmp(categoryName, categories[0].categoryName) == 0) {
		newTerm->firstCategoryOccurences++;
	}
			
	else if (strcmp(categoryName, categories[1].categoryName) == 0) {
		newTerm->secondCategoryOccurences++;
	}
				
	else if (strcmp(categoryName, categories[2].categoryName) == 0) {
		newTerm->thirdCategoryOccurences++;
	}
	
	insertDocumentInfo(&(newTerm->documents), categoryName, documentName);
	
	// If the MLL is empty
	if ((*headerOfTerm) == NULL) {
		(*headerOfTerm) = newTerm;
		return 0;
	} else {
		termPtr iter = (*headerOfTerm), prev;
		
		if (strcmp(iter->term, t) == 1) {
			newTerm->nextPtr = (*headerOfTerm);
			(*headerOfTerm) = newTerm;
			return 0;
		}
		
		while (iter != NULL) {
			
			if (strcmp(iter->term, t) == 0) { // Don' t add duplicates
			
				insertDocumentInfo(&(iter->documents), categoryName, documentName);
				
				if (strcmp(categoryName, categories[0].categoryName) == 0) {
					iter->firstCategoryOccurences++;
				}
				
				else if (strcmp(categoryName, categories[1].categoryName) == 0) {
					iter->secondCategoryOccurences++;
				}
				
				else if (strcmp(categoryName, categories[2].categoryName) == 0) {
					iter->thirdCategoryOccurences++;
				}
				
				return 1;
			}
			
			if (strcmp(iter->term, t) == -1) { // Insert in alphabetical order
				prev = iter;
				iter = iter->nextPtr;
			} else {
				break;
			}		
		}
		
		
		prev->nextPtr = newTerm;
		newTerm->nextPtr = iter;
	}
}

int insertDocumentInfo (documentInfoPtrPtr headerOfDocumentInfo, char categoryName[], char documentName[]) {
	
	// Allocate memory
	documentInfoPtr newDocumentInfo = (documentInfoPtr) malloc(sizeof(documentInfo));
	
	strcpy(newDocumentInfo->categoryName, categoryName);
	strcpy(newDocumentInfo->documentName, documentName);
	newDocumentInfo->occurences = 1;
	newDocumentInfo->nextPtr = NULL;
	
	// If the linked list is empty
	if ((*headerOfDocumentInfo) == NULL) {
		(*headerOfDocumentInfo) = newDocumentInfo;
		return 0;
	} else {
		documentInfoPtr iter = (*headerOfDocumentInfo), prev;
		
		while (iter != NULL) {
			
			if (strcmp(iter->categoryName, categoryName) == 0 
			&& strcmp(iter->documentName, documentName) == 0) { // Don' t add duplicates
				iter->occurences++;
				return 1;
			}
			
			prev = iter;
			iter = iter->nextPtr;
		}
		
		prev->nextPtr = newDocumentInfo;
		return 0;
	}
	
}

int findFirstOrders (termPtr headerOfTerm) {
	
	termPtr curr, iter;
	documentInfoPtr curr_documents, iter_documents;
	
	FILE *output = fopen("output.txt", "w");
	fprintf(output, "First Orders: ");
	if (output == NULL) {
		printf("Error!");
		exit(1);	
	}
	
	for (curr = headerOfTerm ; curr != NULL ; curr = curr->nextPtr) {
		for (iter = curr->nextPtr ; iter != NULL ; iter = iter->nextPtr) {
			for (curr_documents = curr->documents ; curr_documents != NULL ; curr_documents = curr_documents->nextPtr) {
				for (iter_documents = iter->documents ; iter_documents != NULL ; iter_documents = iter_documents->nextPtr) {
					
					if (strcmp(curr_documents->categoryName, iter_documents->categoryName) == 0
					&& strcmp(curr_documents->documentName, iter_documents->documentName) == 0) {
						
						fprintf(output, "{%s, %s} ", curr->term, iter->term); // Give an output to txt	
						
						insertFirstOrder(&(curr->firstOrders), &(iter->firstOrders), curr, iter);	
					}
				}
			}
		}
	}
	
	fclose(output);
	return 0;
}

int insertFirstOrder (termAddressPtrPtr headerOfTermAddress1, termAddressPtrPtr headerOfTermAddress2, termPtr term1, termPtr term2) {
	
	// Firstly add the term2 into the term1's firstOrder linked list
	// Allocate memory
	termAddressPtr newTermAddress2 = (termAddressPtr) malloc(sizeof(termAddress));
	
	newTermAddress2->termAddress = term2;
	newTermAddress2->nextPtr = NULL;
	
	// If the linked list is empty
	if ((*headerOfTermAddress1) == NULL) {
		
		(*headerOfTermAddress1) = newTermAddress2;
	} else {
		
		termAddressPtr iter = (*headerOfTermAddress1), prev;
		
		while (iter != NULL) {
			prev = iter;
			iter = iter->nextPtr;
		}
		
		prev->nextPtr = newTermAddress2;
	}
	
	// Then add the term1 into the term2's first order linked list
	//Allocate memory
	termAddressPtr newTermAddress1 = (termAddressPtr) malloc(sizeof(termAddress));
	
	newTermAddress1->termAddress = term1;
	newTermAddress1->nextPtr = NULL;
	
	// If the linked list is empty
	if ((*headerOfTermAddress2) == NULL) {
		
		(*headerOfTermAddress2) = newTermAddress1;
	} else {
		
		termAddressPtr iter2 = (*headerOfTermAddress2), prev2;
		
		while (iter2 != NULL) {
			prev2 = iter2;
			iter2 = iter2->nextPtr;
		}
		
		prev2->nextPtr = newTermAddress1;
	}
	
	return 0;
}

int findSecondOrder (termPtr headerOfTerm) {
	
	termPtr curr;
	termAddressPtr curr_firstOrder, curr_firstOrder_firstOrder,
	curr_firstOrder_firstOrder_firstOrder;
	
	
	for (curr = headerOfTerm ; curr != NULL ; curr = curr->nextPtr) {
		
		for (curr_firstOrder = curr->firstOrders ; curr_firstOrder != NULL ;
		curr_firstOrder = curr_firstOrder->nextPtr) {
			
			termPtr curr_firstOrderNode = curr_firstOrder->termAddress; // Casting
			
			for (curr_firstOrder_firstOrder = curr_firstOrderNode->firstOrders ; 
			curr_firstOrder_firstOrder != NULL ; 
			curr_firstOrder_firstOrder = curr_firstOrder_firstOrder->nextPtr) {
				
				termPtr curr_firstOrder_firstOrderNode = curr_firstOrder_firstOrder->termAddress; // Casting
				
				if (curr_firstOrder_firstOrderNode == curr) {

					continue;
				}
				int key = 1;
				for (curr_firstOrder_firstOrder_firstOrder = curr_firstOrder_firstOrderNode->firstOrders ; 
				curr_firstOrder_firstOrder_firstOrder != NULL ;
				curr_firstOrder_firstOrder_firstOrder = curr_firstOrder_firstOrder_firstOrder->nextPtr) {
					
					termPtr curr_firstOrder_firstOrder_firstOrderNode = curr_firstOrder_firstOrder_firstOrder->termAddress; // Casting
					
					if (curr_firstOrder_firstOrder_firstOrderNode == curr) {
						
						key = 0;
						break;
					}
					
				}
				
				if (key){
					
					insertSecondOrder(&(curr->secondOrders), curr_firstOrder_firstOrderNode);
					
				}
			}
		}
	}
}

int insertSecondOrder (termAddressPtrPtr headerOfTermAdress, termPtr term) {
	
	// Allocate memory
	termAddressPtr newTermAddress = (termAddressPtr) malloc(sizeof(termAddress));
	
	newTermAddress->termAddress = term;
	newTermAddress->nextPtr = NULL;
	
	// If the linked list is empty
	if ((*headerOfTermAdress) == NULL) {
		
		(*headerOfTermAdress) = newTermAddress;

	} else {
		
		termAddressPtr iter = (*headerOfTermAdress), prev;
		
		while (iter != NULL) {
			
			termPtr iterNode = iter->termAddress; // Casting
			if (iterNode == term) { // Don' t add duplicates
				return 1;
			}
			
			prev = iter;
			iter = iter->nextPtr;
			
		}
		
		prev->nextPtr = newTermAddress;
		
	}
}

int findThirdOrder (termPtr headerOfTerm) {
	
	termPtr curr;
	termAddressPtr curr_firstOrder, curr_firstOrder_secondOrder,
	curr_firstOrder_secondOrder_firstOrder,
	curr_firstOrder_secondOrder_secondOrder;
	
	for (curr = headerOfTerm ; curr != NULL ; curr = curr->nextPtr) {
		
		for (curr_firstOrder = curr->firstOrders ; curr_firstOrder != NULL ;
		curr_firstOrder = curr_firstOrder->nextPtr) {
			
			termPtr curr_firstOrderNode = curr_firstOrder->termAddress; // Casting
			
			for (curr_firstOrder_secondOrder = curr_firstOrderNode->secondOrders ; 
			curr_firstOrder_secondOrder != NULL ;
			curr_firstOrder_secondOrder = curr_firstOrder_secondOrder->nextPtr) {
				
				termPtr curr_firstOrder_secondOrderNode = curr_firstOrder_secondOrder->termAddress; //Casting
				
				int key = 1;
				
				for (curr_firstOrder_secondOrder_firstOrder = curr_firstOrder_secondOrderNode->firstOrders ;
				curr_firstOrder_secondOrder_firstOrder != NULL ;
				curr_firstOrder_secondOrder_firstOrder = curr_firstOrder_secondOrder_firstOrder->nextPtr) {
					
					termPtr curr_firstOrder_secondOrder_firstOrderNode = curr_firstOrder_secondOrder_firstOrder->termAddress;
					
					if (curr_firstOrder_secondOrder_firstOrderNode == curr) {
						
						key = 0;
						break;
					}
				}
				
				for (curr_firstOrder_secondOrder_secondOrder = curr_firstOrder_secondOrderNode->secondOrders ;
				curr_firstOrder_secondOrder_secondOrder != NULL ;
				curr_firstOrder_secondOrder_secondOrder = curr_firstOrder_secondOrder_secondOrder->nextPtr) {
					
					termPtr curr_firstOrder_secondOrder_secondOrderNode = curr_firstOrder_secondOrder_secondOrder->termAddress;
					
					if (curr_firstOrder_secondOrder_secondOrderNode == curr) {
						
						key = 0;
						break;
					}
				}
				
				if (key) {					
					insertThirdOrder(&(curr->thirdOrders), curr_firstOrder_secondOrderNode);
				}
			}
		}
	}
}

int insertThirdOrder (termAddressPtrPtr headerOfTermAdress, termPtr term) {
	
	// Allocate memory
	termAddressPtr newTermAddress = (termAddressPtr) malloc(sizeof(termAddress));
	
	newTermAddress->termAddress = term;
	newTermAddress->nextPtr = NULL;
	
	// If the linked list is empty
	if ((*headerOfTermAdress) == NULL) {
		
		(*headerOfTermAdress) = newTermAddress;

	} else {
		
		termAddressPtr iter = (*headerOfTermAdress), prev;
		
		while (iter != NULL) {
			
			termPtr iterNode = iter->termAddress; // Casting
			if (iterNode == term) { // Don' t add duplicates
				return 1;
			}
			
			prev = iter;
			iter = iter->nextPtr;
		}
		
		prev->nextPtr = newTermAddress;
		
	}
}

void printSecondOrders(termPtr headerOfTerm) {
	
	termPtr curr;
	termAddressPtr iter;
	
	FILE *output = fopen("output.txt", "a");
	
	fprintf(output, "\n\n\nSecond Orders: ");
	
	for (curr = headerOfTerm ; curr != NULL ; curr = curr->nextPtr) {
		
		for (iter = curr->secondOrders ; iter != NULL ; iter = iter->nextPtr) {
			
			termPtr iterNode = iter->termAddress; // Casting
			fprintf(output, "{%s, %s} ", curr->term, iterNode->term);
			deleteTermAddress(&(iterNode->secondOrders), curr);
			
		}
	}
	
	fclose(output);
}

void printThirdOrders (termPtr headerOfTerm) {
	
	termPtr curr;
	termAddressPtr iter;
	
	FILE *output = fopen("output.txt", "a");
	
	fprintf(output, "\n\n\nThird Orders: ");
	
	for (curr = headerOfTerm ; curr != NULL ; curr = curr->nextPtr) {
		
		for (iter = curr->thirdOrders ; iter != NULL ; iter = iter->nextPtr) {
			
			termPtr iterNode = iter->termAddress; // Casting
			fprintf(output, "{%s, %s} ", curr->term, iterNode->term);
			deleteTermAddress(&(iterNode->thirdOrders), curr);
			
		}
	}
	
	fclose(output);
}

void deleteTermAddress (termAddressPtrPtr headerOfTermAddress, termPtr key) {
	
	termAddressPtr iter = (*headerOfTermAddress), prev;
	termPtr iterNode = iter->termAddress; // Casting
	
	// If the linked list is empty
	if ((*headerOfTermAddress) == NULL) {
		return;
	}
	
	// Search key in the list
	while (iterNode != NULL && iterNode != key) {
		
		prev = iter;
		iter = iter->nextPtr;
		iterNode = iter->termAddress;
		
	}
	
	// If key is the first element of the linked list
	if (iter == (*headerOfTermAddress)) {
		
		(*headerOfTermAddress) = iter->nextPtr;
		free(iter);
		return;
		
	} else {
		
		prev->nextPtr = iter->nextPtr;
		free(iter);
		return;
		
	}
}

int findMostFrequentWords (termPtr headerOfTerm, termAddressPtrPtr cat1, termAddressPtrPtr cat2, termAddressPtrPtr cat3) {
	
	termPtr curr;
	
	for (curr = headerOfTerm ; curr != NULL ; curr = curr->nextPtr) {
		
		// First Category
		if (curr->firstCategoryOccurences > 0) {
			// Allocate memory
			termAddressPtr newTermAddress = (termAddressPtr) malloc(sizeof(termAddress));
		
			newTermAddress->termAddress = curr;
			newTermAddress->nextPtr = NULL;
			
			//If the linked list is empty
			if ((*cat1) == NULL) {
				(*cat1) = newTermAddress;
			} else {
				// If the curr's first category occurences bigger than header of cat1' s first category occurences
				termPtr cat1Node = (*cat1)->termAddress; // Casting
				if (cat1Node->firstCategoryOccurences < curr->firstCategoryOccurences) {
					newTermAddress->nextPtr = (*cat1);
					(*cat1) = newTermAddress;
				} else {
					termAddressPtr iter = (*cat1), prev;
					while (iter != NULL) {
						termPtr iterNode = iter->termAddress; // Casting
						if (iterNode->firstCategoryOccurences >= curr->firstCategoryOccurences) {
							prev = iter;
							iter = iter->nextPtr;
						} else {
							break;
						}
					}
					prev->nextPtr = newTermAddress;
					newTermAddress->nextPtr = iter;
				}
			}	
		}
		
		// Second Category
		if (curr->secondCategoryOccurences > 0) {
			// Allocate memory
			termAddressPtr newTermAddress = (termAddressPtr) malloc(sizeof(termAddress));
		
			newTermAddress->termAddress = curr;
			newTermAddress->nextPtr = NULL;
			
			//If the linked list is empty
			if ((*cat2) == NULL) {
				(*cat2) = newTermAddress;
			} else {
				// If the curr's first category occurences bigger than header of cat1' s first category occurences
				termPtr cat2Node = (*cat2)->termAddress; // Casting
				if (cat2Node->secondCategoryOccurences < curr->secondCategoryOccurences) {
					newTermAddress->nextPtr = (*cat2);
					(*cat2) = newTermAddress;
				} else {
					termAddressPtr iter = (*cat2), prev;
					while (iter != NULL) {
						termPtr iterNode = iter->termAddress; // Casting
						if (iterNode->secondCategoryOccurences >= curr->secondCategoryOccurences) {
							prev = iter;
							iter = iter->nextPtr;
						} else {
							break;
						}
					}
					prev->nextPtr = newTermAddress;
					newTermAddress->nextPtr = iter;
				}
			}	
		}
		
		// Third Category
		if (curr->thirdCategoryOccurences > 0) {
			// Allocate memory
			termAddressPtr newTermAddress = (termAddressPtr) malloc(sizeof(termAddress));
		
			newTermAddress->termAddress = curr;
			newTermAddress->nextPtr = NULL;
			
			//If the linked list is empty
			if ((*cat3) == NULL) {
				(*cat3) = newTermAddress;
			} else {
				// If the curr's first category occurences bigger than header of cat1' s first category occurences
				termPtr cat3Node = (*cat3)->termAddress; // Casting
				if (cat3Node->thirdCategoryOccurences < curr->thirdCategoryOccurences) {
					newTermAddress->nextPtr = (*cat2);
					(*cat3) = newTermAddress;
				} else {
					termAddressPtr iter = (*cat3), prev;
					while (iter != NULL) {
						termPtr iterNode = iter->termAddress; // Casting
						if (iterNode->thirdCategoryOccurences >= curr->thirdCategoryOccurences) {
							prev = iter;
							iter = iter->nextPtr;
						} else {
							break;
						}
					}
					prev->nextPtr = newTermAddress;
					newTermAddress->nextPtr = iter;
				}
			}	
		}			
	}
}

void printFirstCategoryIdf (termAddressPtr headerOfTermAddress, int numberOfDocuments, categoryInfo category) {
	
	termAddressPtr curr;
	int i;
	termPtr terms[5];
	
	FILE *output = fopen("output.txt", "a");
	
	fprintf(output, "\n\n\nIDF' s in %s:\n", category.categoryName);
	
	for (curr = headerOfTermAddress, i = 0 ; curr != NULL && i < 5 ; curr = curr->nextPtr, i++) {
		
		termPtr currNode = curr->termAddress; // Casting
		documentInfoPtr iter;
		float docCounter = 0;
		
		for (iter = currNode->documents ; iter != NULL ; iter = iter->nextPtr) {
			
			if (strcmp(category.categoryName, iter->categoryName) == 0) {
				docCounter++;
			}
		}
		
		currNode->idf = log(numberOfDocuments / docCounter) * currNode->firstCategoryOccurences;
		terms[i] = currNode;		
	}
	
	// Insertion sort
	int j, k, minIndex;
	termPtr min, temp;
	for (j = 0 ; j < 4 ; j++) {
		minIndex = j;
		min = terms[j];
		for (k = j + 1 ; k < 5 ; k++) {
			
			if (terms[j]->idf < min->idf){
				minIndex = k;
				min = terms[k];
			}
		}
		temp = terms[j];
		terms[j] = terms[minIndex];
		terms[minIndex] = temp;
	}
	
	for (j = 4 ; j >= 0 ; j--) {
		fprintf(output, "%s;%f\n", terms[j]->term, terms[j]->idf);
	}
	
	fclose(output);
}

void printSecondCategoryIdf (termAddressPtr headerOfTermAddress, int numberOfDocuments, categoryInfo category) {
	termAddressPtr curr;
	int i;
	termPtr terms[5];
	
	FILE *output = fopen("output.txt", "a");
	
	fprintf(output, "\n\n\nIDF' s in %s:\n", category.categoryName);
	
	for (curr = headerOfTermAddress, i = 0 ; curr != NULL && i < 5 ; curr = curr->nextPtr, i++) {
		
		termPtr currNode = curr->termAddress; // Casting
		documentInfoPtr iter;
		float docCounter = 0;
		
		for (iter = currNode->documents ; iter != NULL ; iter = iter->nextPtr) {
			
			if (strcmp(category.categoryName, iter->categoryName) == 0) {
				docCounter++;
			}
		}
		
		currNode->idf = log(numberOfDocuments / docCounter) * currNode->secondCategoryOccurences;
		terms[i] = currNode;		
	}
	
	// Insertion sort
	int j, k, minIndex;
	termPtr min, temp;
	for (j = 0 ; j < 4 ; j++) {
		minIndex = j;
		min = terms[j];
		for (k = j + 1 ; k < 5 ; k++) {
			
			if (terms[j]->idf < min->idf){
				minIndex = k;
				min = terms[k];
			}
		}
		temp = terms[j];
		terms[j] = terms[minIndex];
		terms[minIndex] = temp;
	}
	
	for (j = 4 ; j >= 0 ; j--) {
		fprintf(output, "%s;%f\n", terms[j]->term, terms[j]->idf);
	}
	
	fclose(output);
}

void printThirdCategoryIdf (termAddressPtr headerOfTermAddress, int numberOfDocuments, categoryInfo category) {
	termAddressPtr curr;
	int i;
	termPtr terms[5];
	
	FILE *output = fopen("output.txt", "a");
	
	fprintf(output, "\n\n\nIDF' s in %s:\n", category.categoryName);
	
	for (curr = headerOfTermAddress, i = 0 ; curr != NULL && i < 5 ; curr = curr->nextPtr, i++) {
		
		termPtr currNode = curr->termAddress; // Casting
		documentInfoPtr iter;
		float docCounter = 0;
		
		for (iter = currNode->documents ; iter != NULL ; iter = iter->nextPtr) {
			
			if (strcmp(category.categoryName, iter->categoryName) == 0) {
				docCounter++;
			}
		}
		currNode->idf = log(numberOfDocuments / docCounter) * currNode->thirdCategoryOccurences;
		terms[i] = currNode;
	}
	
	// Insertion sort
	int j, k, minIndex;
	termPtr min, temp;
	for (j = 0 ; j < 4 ; j++) {
		minIndex = j;
		min = terms[j];
		for (k = j + 1 ; k < 5 ; k++) {
			
			if (terms[j]->idf < min->idf){
				minIndex = k;
				min = terms[k];
			}
		}
		temp = terms[j];
		terms[j] = terms[minIndex];
		terms[minIndex] = temp;
	}
	
	for (j = 4 ; j >= 0 ; j--) {
		fprintf(output, "%s;%f\n", terms[j]->term, terms[j]->idf);
	}
	
	fclose(output);
}


void printMLL (termPtr headerOfTerm) { // For only test
	termPtr iter = headerOfTerm;
	
	while (iter != NULL) {
		printf("%s, firstCatOcc: %d, secCatOcc: %d, thirdCatOcc: %d\n", iter->term, iter->firstCategoryOccurences, iter->secondCategoryOccurences, iter->thirdCategoryOccurences);
		
		termAddressPtr iter_firstOrders = iter->firstOrders;
		
		printf("First Orders:\n");
		
		while (iter_firstOrders != NULL) {
			termPtr iter_firstOrderNode = iter_firstOrders->termAddress; // casting
			printf("\t%s\n", iter_firstOrderNode->term);
			iter_firstOrders = iter_firstOrders->nextPtr;
		}
		
		termAddressPtr iter_secondOrders = iter->secondOrders;
		
		printf("Second Orders:\n");
		
		while (iter_secondOrders != NULL) {
	
			termPtr iter_secondOrderNode = iter_secondOrders->termAddress; // Casting
			printf("\t%s\n", iter_secondOrderNode->term);
			iter_secondOrders = iter_secondOrders->nextPtr;
		}
		
		termAddressPtr iter_thirdOrders = iter->thirdOrders;
		
		printf("Third Orders:\n");
		
		while (iter_thirdOrders != NULL) {
	
			termPtr iter_thirdOrdersNode = iter_thirdOrders->termAddress; // Casting
			printf("\t%s\n", iter_thirdOrdersNode->term);
			iter_thirdOrders = iter_thirdOrders->nextPtr;
		}
		
		iter = iter->nextPtr;
		printf("\n\n\n");
	}
}

void printFirstCategoryOccurences (termAddressPtr headerOfTermAddress, categoryInfo category) {
	
	termAddressPtr iter;
	int i;
	
	FILE *output = fopen("output.txt", "a");
	
	fprintf(output, "\n\n\nMost Frequent Words in %s:\n", category.categoryName);
	
	// Print first 5 term
	for (iter = headerOfTermAddress, i = 0 ; iter != NULL && i < 5 ; iter = iter->nextPtr, i++) {
		termPtr iterNode = iter->termAddress; // Casting
		
		fprintf(output, "%s;%d\n", iterNode->term, iterNode->firstCategoryOccurences);
	}
	
	fclose(output);
}

void printSecondCategoryOccurences (termAddressPtr headerOfTermAddress, categoryInfo category) {
	
	termAddressPtr iter;
	int i;
	
	FILE *output = fopen("output.txt", "a");
	
	fprintf(output, "\n\n\nMost Frequent Words in %s:\n", category.categoryName);
	
	// Print first 5 term
	for (iter = headerOfTermAddress, i = 0 ; iter != NULL && i < 5 ; iter = iter->nextPtr, i++) {
		termPtr iterNode = iter->termAddress; // Casting
		
		fprintf(output, "%s;%d\n", iterNode->term, iterNode->secondCategoryOccurences);
	}
	
	fclose(output);
}

void printThirdCategoryOccurences (termAddressPtr headerOfTermAddress, categoryInfo category) {
	
	termAddressPtr iter;
	int i;
	
	FILE *output = fopen("output.txt", "a");
	
	fprintf(output, "\n\n\nMost Frequent Words in %s:\n", category.categoryName);
	
	// Print first 5 term
	for (iter = headerOfTermAddress, i = 0 ; iter != NULL && i < 5 ; iter = iter->nextPtr, i++) {
		termPtr iterNode = iter->termAddress; // Casting
		
		fprintf(output, "%s;%d\n", iterNode->term, iterNode->thirdCategoryOccurences);
	}
	
	fclose(output);
}
