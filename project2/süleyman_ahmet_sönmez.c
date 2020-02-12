#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Structs
typedef struct binaryTreeNode {
	int data;
	int numbersOfLeftChilds;
	int numbersOfRightChilds;
	long long int sizeOfResults;
	struct binaryTreeNode *left;
	struct binaryTreeNode *right;
} binaryTreeNode;
typedef binaryTreeNode* binaryTreeNodePtr;

// Helper Functions
int factorial(int number) {
	int fact = 1;
	int i;
	if (number == 0) {
		return 1;
	}
	for (i = 1 ; i <= number ; i++) {
		fact *= i;
	}
	return fact;
}

int findPermutationWithRepetition(int a, int b) {
	int nominator  = 1;
	int i, j;
	for (i = a + b, j = b ; j > 0 ; j--, i--) {
		nominator *= i;
	}
	int denominator = factorial(b);
	return nominator / denominator;
}

void reverseArray(int a[], int n) {
  int c, t;
  for (c = 0; c < n/2; c++) {
    t = a[c];
    a[c] = a[n-c-1];
    a[n-c-1] = t;
  }
}

// Function prototypes
binaryTreeNodePtr insert(binaryTreeNodePtr node, int data);
int** findSequences(binaryTreeNodePtr node);
void traverse(binaryTreeNodePtr node);
void inOrderTraverse(binaryTreeNodePtr node);
void findResultArraySize(binaryTreeNodePtr node);

int main() {
	binaryTreeNodePtr root = malloc(sizeof(binaryTreeNode));
	
	int number, counter = 0;
	char check;
	
	// Get inputs from the user and construct bst
	while (check != '\n') {
		scanf("%d%c", &number, &check);
		if (counter == 0) { // Insert root to the bst
			root->data = number;
			root->numbersOfLeftChilds = root->numbersOfRightChilds = 0;
			root->sizeOfResults = 0;
			root->left = root->right = NULL;
			counter++;
		} else {
			insert(root, number); // Insert node bst
		}	
	}
	
	printf("In order traverse: ");
	inOrderTraverse(root); // Print bst
	printf("\n");
	
	traverse(root);
	
	int i, j;
	int** result = (int**) malloc(root->sizeOfResults * sizeof(int*));
	for (i = 0 ; i < root->sizeOfResults ; i++) {
		result[i] = (int*) malloc((root->numbersOfLeftChilds + root->numbersOfRightChilds + 1) * sizeof(int));
	}
	
	result = findSequences(root); // Find all possible sequences yield to same bst
	
	FILE *f = fopen("output.txt", "w");
	if (f == NULL) {
		printf("Error while opening file");
		return 0;
	}
	
	for (i = 0 ; i < root->sizeOfResults ; i++) {
		fprintf(f, "%d - ", i + 1);
		for (j = 0 ; j < root->numbersOfLeftChilds + root->numbersOfRightChilds + 1 ; j++) {
			fprintf(f, "%d ", result[i][j]);
		}
		fprintf(f, "\n");
	}
	printf("Sequences printed to output.txt");
	return 1;
}

binaryTreeNodePtr insert(binaryTreeNodePtr node, int data) {
	if (node == NULL) {
		node = malloc(sizeof(binaryTreeNode));
		if (node == NULL) return 0;
		node->data = data;
		node->numbersOfLeftChilds = node->numbersOfRightChilds = 0;
		node->sizeOfResults = 0;
		node->left = node->right = NULL;
		return node;
	}
	else if (data < node->data) {
		node->numbersOfLeftChilds++;
		node->left = insert(node->left, data);
	}
	else if (data > node->data) {
		node->numbersOfRightChilds++;
		node->right = insert(node->right, data);
	}
}

void traverse(binaryTreeNodePtr node) {
	if (node != NULL) {
	traverse(node->left);
	traverse(node->right);
	findResultArraySize(node);	
	}	
}

void inOrderTraverse(binaryTreeNodePtr node) {
	if (node != NULL) {
		inOrderTraverse(node->left);
		printf("%d ", node->data);
		inOrderTraverse(node->right);
	}
}

void findResultArraySize(binaryTreeNodePtr node) {
	long long int resultArraySize = findPermutationWithRepetition(node->numbersOfLeftChilds, node->numbersOfRightChilds);
	if (node->numbersOfLeftChilds != 0) {
		resultArraySize *= node->left->sizeOfResults;
	}
	if (node->numbersOfRightChilds != 0) {
		resultArraySize *= node->right->sizeOfResults;
	}
	node->sizeOfResults = resultArraySize;
}

int** findPatterns(int numberOfLeftChilds, int numberOfRightChilds) {
	int numberOfPatterns = findPermutationWithRepetition(numberOfLeftChilds, numberOfRightChilds);
	int** patterns = (int**) malloc(numberOfPatterns * sizeof(int*));
	
	int i, j, k, m = 1;
	for (i = 0 ; i < numberOfPatterns ; i++) {
		patterns[i] = (int*) malloc((numberOfLeftChilds + numberOfRightChilds) * sizeof(int));
	}
	
	for (i = 0, j = numberOfRightChilds, k = numberOfLeftChilds ; i < (numberOfLeftChilds + numberOfRightChilds) ; i++) {
		if (j != 0) {
			patterns[0][i] = 1;
			j--;
			continue;
		}
		if (k != 0) {
			patterns[0][i] = 0;
			k--;
			continue;
		}
	}
	
	long long int maxNumber = 0;
	for (i = (numberOfLeftChilds + numberOfRightChilds), j = 0 ; i > 0 ; i--) {
		maxNumber += patterns[0][i - 1] * pow(2, j++);
	}
	
	for (i = 1 ; i < maxNumber ; i++) {
		int binaryNumber[numberOfLeftChilds + numberOfRightChilds];
		
		int n = i;
		j = 0;
		while (n > 0) { // Convert decimal to binary
			binaryNumber[j++] = n % 2;
			n = n / 2;
		}
		while (j < (numberOfLeftChilds + numberOfRightChilds)) {
			binaryNumber[j++] = 0;			
		}
		
		reverseArray(binaryNumber, (numberOfLeftChilds + numberOfRightChilds));
		
		int counter = 0;
		for (k = 0 ; k < (numberOfLeftChilds + numberOfRightChilds) ; k++) {
			if (binaryNumber[k] == 1) {
				counter++;
			}
		}
		
		if (counter == numberOfRightChilds) {
			int iter;
			for (iter = 0 ; iter < (numberOfLeftChilds + numberOfRightChilds) ; iter++) {
				patterns[m][iter] = binaryNumber[iter];
			}
			m++;
		}
	}
	return patterns;
}

int** findSequences(binaryTreeNodePtr node) {
	if (node == NULL) {
		int** results = (int**) malloc(sizeof(int*));
		results[0] = (int*) malloc(sizeof(int));
		return results;
	}
	
	if(node->numbersOfLeftChilds == 0 && node->numbersOfRightChilds == 0) { //If it is leaf
		int** results = (int**) malloc(sizeof(int*));
		results[0] = (int*) malloc(sizeof(int));
		results[0][0] = node->data;
		return results;
	}
	
	int** results = (int**) malloc(node->sizeOfResults * sizeof(int*));
	int** leftResults;
	int** rightResults;
	
	int i;
	for (i = 0 ; i < node->sizeOfResults ; i++) {
		results[i] = (int*) malloc((node->numbersOfLeftChilds + node->numbersOfRightChilds + 1) * sizeof(int));
	}
	
	if (node->left == NULL) {
		leftResults = (int**) malloc(sizeof(int*));
		leftResults[0] = (int*) malloc(sizeof(int));
	} else {
		leftResults = (int**) malloc(node->left->sizeOfResults * sizeof(int*));
		for (i = 0 ; i < node->left->sizeOfResults ; i++) {
			leftResults[i] = (int*) malloc((node->left->numbersOfLeftChilds + node->left->numbersOfRightChilds + 1) * sizeof(int));
		}
	}
	
	if (node->right == NULL) {
		rightResults = (int**) malloc(sizeof(int*));
		rightResults[0] = (int*) malloc(sizeof(int));
	} else {
		rightResults = (int**) malloc(node->right->sizeOfResults * sizeof(int*));
		for (i = 0 ; i < node->right->sizeOfResults ; i++) {
			rightResults[i] = (int*) malloc((node->right->numbersOfLeftChilds + node->right->numbersOfRightChilds + 1) * sizeof(int));
		}
	}
	
	leftResults = findSequences(node->left);
	rightResults = findSequences(node->right);
	
	int numberOfPatterns = findPermutationWithRepetition(node->numbersOfLeftChilds, node->numbersOfRightChilds);
	int** patterns = (int**) malloc(numberOfPatterns * sizeof(int*));
	for (i = 0 ; i < numberOfPatterns ; i++) {
		patterns[i] = (int*) malloc((node->numbersOfLeftChilds + node->numbersOfRightChilds) * sizeof(int));
	}
	patterns = findPatterns(node->numbersOfLeftChilds, node->numbersOfRightChilds);
	
	for (i = 0 ; i < node->sizeOfResults ; i++) {
		results[i][0] = node->data;
	}
	
	int sizeOfLeftResults, sizeOfRightResults;
	if (node->left == NULL) {
		sizeOfLeftResults = 1;
	} else { 
		sizeOfLeftResults = node->left->sizeOfResults;
	}
	if (node->right == NULL) {
		sizeOfRightResults = 1;
	} else {
		sizeOfRightResults = node->right->sizeOfResults;
	}
	
	int j, k, m, a = 0, b = 1, l = 0, r = 0;
	for (i = 0 ; i < sizeOfLeftResults ; i++) {
		for (j = 0 ; j < sizeOfRightResults ; j++) {
			for (k = 0 ; k < numberOfPatterns ; k++) {
				for (m = 0 ; m < (node->numbersOfLeftChilds + node->numbersOfRightChilds) ; m++) {
					if (patterns[k][m] == 0) {
						results[a][b++] = leftResults[i][l++];
					}
					if (patterns[k][m] == 1) {
						results[a][b++] = rightResults[j][r++];
					}
				}
				a++;
				l = r = 0;
				b = 1;
			}
		}
	}
	return results;
}
