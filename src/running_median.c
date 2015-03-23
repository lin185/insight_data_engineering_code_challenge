/* 
 * Another common problem is the Running Median - which keeps track of the median 
 * for a stream of numbers, updating the median for each new number. 
 *
 * The program implements a running median for the number of words per line of text. 
 *
 * Algorithm:
 * 		The running median is computed using two heaps - Max Heap and Min Heap. 
 *		All the numbers less than or equal to the current median are in the Max Heap, 
 *		which is arranged so that the maximum number is at the root of the heap. 
 *		All the numbers greater than or equal to the current median are in the Min Heap, 
 *		which is arranged so that the minimum number is at the root of the heap. 
 *		Then current median can be calculate based on the roots of both heaps.
 *
 *
 * Author: Binhao Lin
 * Email:  lin185@purdue.edu
 */

#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

void running_median(char* intput_filepath, char *outpur_filepath);
float updateHeap(int val);
void insertValToHeap(int type, int val);
void deleteValFromHeap(int type, int val);
void min_heapify(int i);
void max_heapify(int i);
void printHeap();


int compare (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}



/* A Heap Node */
 typedef struct Node {
 	int val;	//number of words in a line
 	int count;	//count how many lines have the same number of words
 } Node;

/* 
 * This function creates a heap node with a given value,
 * return the memory location of the node 
 */
 Node createNode(int val){
 	//printf("Create Node (%d)\n", val);
 	Node *n = (Node *)malloc(sizeof(Node));
 	n->val = val;
 	n->count = 1;
 	return *n;
 }


//Some global variables for Max Heap and Min Heap
Node *maxHeap; //maxHeap (left heap) contains all the numbers less than or equal to the current median
Node *minHeap; //minHeap (right heap) contains all the numbers greater than or equal to the current median
int MAX_HEAP_SIZE;
int MIN_HEAP_SIZE;
int maxHeap_node_count; //count how many nodes are allocated for Max Heap
int minHeap_node_count; //count how many nodes are allocated for Min Heap
int maxHeap_total_count;//count how many values are inserted into Max Heap
int minHeap_total_count;//count how many values are inserted into Min Heap
//Note: heap_node_count is NOT necessarily equal to heap_total_count. 
float current_median;



/* 
 * This function tries to update both Min Heap and Max Heap, 
 * and returns a current median.
 * 
 * Please see to the code comments for detail algorithms.
 */
float updateHeap(int val){
	//The first value is always added to maxHeap
	if(maxHeap_total_count == 0) {
		maxHeap[maxHeap_node_count++] = createNode(val);
		maxHeap_total_count++;
		current_median = val;
	}
	//The second value is always added to minHeap 
	else if(minHeap_total_count == 0) {
		minHeap[minHeap_node_count++] = createNode(val);
		minHeap_total_count++;
		//swap maxHeap with minHeap if necessary
		if(maxHeap[0].val > minHeap[0].val) {
			int temp = maxHeap[0].val;
			maxHeap[0].val = minHeap[0].val;
			minHeap[0].val = temp;
		}
		current_median = (maxHeap[0].val + minHeap[0].val) / 2.0;
	}
	//Starting from the third value...
	else {
		//add val to maxHeap if it is less than the current median 
		if(val < current_median) {
			insertValToHeap(1, val);
			maxHeap_total_count++;
		} 
		//or add val to the minHeap if it is greater than the current median
		else if(val > current_median) {
			insertValToHeap(0, val);
			minHeap_total_count++;
		} 
		//if the val is equal to the current median, 
		//it is added to whichever heap has the smaller count,
		//or to either heap arbitrarily if they have the same count. 
		else {
			if(minHeap_total_count <= maxHeap_total_count) {
				insertValToHeap(0, val);
				minHeap_total_count++;
			} else {
				insertValToHeap(1, val);
				maxHeap_total_count++;
			}
		}
	}

	//If inserting val causes the counts of the two heaps to differ by more than 1, 
	//the root of the larger heap is removed and inserted in the smaller heap. 
	int remove_val;
	if(minHeap_total_count - maxHeap_total_count > 1) {
		remove_val = minHeap[0].val;
		deleteValFromHeap(0, remove_val);
		minHeap_total_count--;
		insertValToHeap(1, remove_val);
		maxHeap_total_count++;
	} else if (maxHeap_total_count - minHeap_total_count > 1){
		remove_val = maxHeap[0].val;
		deleteValFromHeap(1, remove_val);
		maxHeap_total_count--;
		insertValToHeap(0, remove_val);
		minHeap_total_count++;
	}

	//Calculate Median
	//The average of the roots of the two heaps is the current median, if they are the same size.
	//Otherwise, the current median is computed as the root of the larger heap.
	if(minHeap_total_count == maxHeap_total_count)
		current_median = (maxHeap[0].val + minHeap[0].val) / 2.0;
	else if(minHeap_total_count > maxHeap_total_count)
		current_median = minHeap[0].val;
	else
		current_median = maxHeap[0].val;

	//printHeap();
	return current_median;
}


/*
 * This function inserts a value into heap,
 * if the value already exists, increament the counter,
 * otherwise, create a heap node and add to heap.
 * 
 * int type - indicates which heap to insert the value into,
 * 			  0 for minHeap, 1 for maxHeap
 */
void insertValToHeap(int type, int val){

	Node *heap;
	int *element_count;
	if(type == 0) {
		//printf("Add %d to minHeap. Current median: %.1lf\n", val, current_median);
		heap = minHeap;
		element_count = &minHeap_node_count;
	} else {
		//printf("Add %d to maxHeap. Current median: %.1lf\n", val, current_median);
		heap = maxHeap;
		element_count = &maxHeap_node_count;
	}

	//Check if the val already exsits in the heap
	int i;
	for(i=0; i<*element_count; i++) {
		if(heap[i].val == val) {
			heap[i].count++;
			return; 
		}
	}

	//Val is not found in the heap, insert the node into heap
	//Expand the heap by reallocating memory if necessary
	if(type == 0 && *element_count == MIN_HEAP_SIZE) {
		MIN_HEAP_SIZE *= 2;
		minHeap = (Node *)realloc(minHeap, sizeof(Node) * MIN_HEAP_SIZE);
		heap = minHeap;
	} else if(type == 1 && *element_count == MAX_HEAP_SIZE) {
		MAX_HEAP_SIZE *= 2;
		maxHeap = (Node *)realloc(maxHeap, sizeof(Node) * MAX_HEAP_SIZE);
		heap = maxHeap;
	}
	//Add the node to the last slot
	heap[*element_count] = createNode(val);
	(*element_count)++;

	//Swap the newly added node to a correct position
	int child = *element_count - 1;
	int parent = (child - 1) / 2;
	if(type == 0) {
		while(child > 0 && heap[child].val < heap[parent].val) {
			Node n = heap[parent];
			heap[parent] = heap[child];
			heap[child] = n;

			child = parent;
			parent = (child - 1) / 2;
		}
	} else {
		while(child > 0 && heap[child].val > heap[parent].val) {
			Node n = heap[parent];
			heap[parent] = heap[child];
			heap[child] = n;

			child = parent;
			parent = (child - 1) / 2;
		}
	}	
}


/*
 * This function deletes a value from heap,
 * if the value has count > 1, increament the counter,
 * otherwise, remove the node contains value and do a heapify.
 * 
 * int type - indicates which heap to remove the value from,
 * 			  0 for minHeap, 1 for maxHeap
 */
void deleteValFromHeap(int type, int val) {
	//determine whether to delete from minHeap or maxHeap
	Node *heap;
	int *element_count;
	if(type == 0) {
		heap = minHeap;
		element_count = &minHeap_node_count;
	} else {
		heap = maxHeap;
		element_count = &maxHeap_node_count;
	}

	int i;
	//find the node contains the value
	for(i=0; i<*element_count; i++) {
		if(heap[i].val == val) {
			//if the node has count > 1, simply decrement the count
			if(heap[i].count > 1) {
				heap[i].count--;
				return;
			} 
			//otherwise, delete the node
			else {
				heap[i] = heap[*element_count - 1];
				(*element_count)--;
				break;
			} 
		}
	}

	//heapify
	if(type == 0) {
		min_heapify(0);
	} else {
		max_heapify(0);
	}

}


/* 
 * This function, given a tree that is a heap except for node i, 
 * arranges node i and it's subtrees to satisfy the min heap property.
 */
void min_heapify(int i) {
	int l = 2 * i + 1;
	int r = 2 * i + 2;
	int smallest;
	if(l < minHeap_node_count && minHeap[l].val <  minHeap[i].val)
		smallest = l;
	else
		smallest = i;

	if(r < minHeap_node_count && minHeap[r].val < minHeap[smallest].val)
		smallest = r;

	if(smallest != i) {
		Node n = minHeap[i];
		minHeap[i] = minHeap[smallest];
		minHeap[smallest] = n;
		min_heapify(smallest);
	}
}

/* 
 * This function, given a tree that is a heap except for node i, 
 * arranges node i and it's subtrees to satisfy the max heap property.
 */
void max_heapify(int i) {
	int l = 2 * i + 1;
	int r = 2 * i + 2;
	int largest;
	if(l < maxHeap_node_count && maxHeap[l].val >  maxHeap[i].val)
		largest = l;
	else
		largest = i;

	if(r < maxHeap_node_count && maxHeap[r].val > maxHeap[largest].val)
		largest = r;

	if(largest != i) {
		Node n = maxHeap[i];
		maxHeap[i] = maxHeap[largest];
		maxHeap[largest] = n;
		max_heapify(largest);
	}
}


void printHeap(){
	int i;
	printf("Max Heap: \n");
	for(i=0; i<maxHeap_node_count; i++) {
		printf("(%d, %d) ", maxHeap[i].val, maxHeap[i].count);
	}
	printf("\n");

	printf("Min Heap: \n");
	for(i=0; i<minHeap_node_count; i++) {
		printf("(%d, %d) ", minHeap[i].val, minHeap[i].count);
	}
	printf("\n\n");

}




/*
 * This function opens a input file, counts words for each line of the file,
 * then adds the word_count value to heap to calculate current median,
 * finally writes the current median to an output file.
 */
void running_median(char* intput_filepath, char *outpur_filepath) {
	int line_count = 0;
	FILE * fd = fopen(intput_filepath, "r");
	FILE * fout = fopen(outpur_filepath, "a");
	if(fd == NULL || fout == NULL){
		perror("Open file error");
		return;
	}
	printf("Process %s\n", intput_filepath);

	char c = fgetc(fd);
	int word_count = 0;
	int word_len = 0;
	float median; 
	while(c != EOF ){
		//end of the line
		if(c == '\n') {
			if(word_len != 0) {
				//finish up counting word of a line,
				//call updateHeap, and get current median
				word_count++;
				word_len = 0;
			}
			line_count++;
			//Add the word_count to the heap, and get a returned current median
			median = updateHeap(word_count);
			fprintf(fout, "%.1f\n", median);
			//printf("%.1f\n", median);
			//printf("line: %d, word count: %d\n", line_count, word_count);
			word_count = 0;
		} 
		//not end of the line
		else {
			//count how many words in the current line
			if(isalpha(c) || c == '\'' || c == '-') {
				word_len++;
			} else {
				if(word_len != 0) {
					word_count++;
					word_len = 0;
				}
			}
		}

		c = fgetc(fd);
		//last iteration
		if(c == EOF) {
			if(word_len != 0) {
				word_count++;
				word_len = 0;
			}
			median = updateHeap(word_count);
			fprintf(fout, "%.1f\n", median);
			//printf("%.1f\n", median);
		}
	}
	fclose(fd);
	fclose(fout);
}


int main(int argc, char* argv[]) {
	//check arguments
	if(argc != 3) {
		printf("Usage: ./running_median ./input_directory ./output_directory/output_file\n");
		return 0;
	}


	/*srand(time(NULL));
	int input_size = 100000;
	int input[input_size];
	int i;
	for(i=0; i<input_size; i++) {
		input[i] = rand() % 5000;
		//printf("%d, ", input[i]);
	}
	//printf("\n\n\n");
	*/

	//1. Initialization
	MAX_HEAP_SIZE = 256;
	MIN_HEAP_SIZE = 256;
	maxHeap = (Node *)malloc(sizeof(Node) * MAX_HEAP_SIZE);
	minHeap = (Node *)malloc(sizeof(Node) * MIN_HEAP_SIZE);
	maxHeap_node_count = 0;
	minHeap_node_count = 0;
	maxHeap_total_count = 0;
	minHeap_total_count = 0;
	current_median = 0;


	//2. Open the wc_input directory, and process each file in it
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (argv[1])) != NULL) {
  		while ((ent = readdir (dir)) != NULL) {
  			if((ent->d_name)[0] == '.') //hidden files
  				continue;
  			//get filepath of each file, process it
  			char *filepath = (char*)malloc(sizeof(char) * 256);
			strcpy(filepath, argv[1]);
			if(filepath[strlen(filepath) - 1] != '/') strcat(filepath, "/");
			strcat(filepath, ent->d_name);

    		//analyze running median on this file
    		running_median(filepath, argv[2] /*output file*/);

    		free(filepath);
  		}
  		closedir (dir);
	} else {
  		perror ("Open directory error");
  		return 0;
	}

	/*
	int array[input_size];
	int array_len = 0;

	FILE *f1 = fopen("./1", "w");
	FILE *f2 = fopen("./2", "w");
	float m1;
	float m2;
	for(i=0; i<input_size; i++) {
		if(i%10000 == 0)
			printf("%d processed...\n", i);
		array[array_len++] = input[i];
		qsort(array, array_len, sizeof(int), compare);

		if(array_len%2 == 1) {
			//odd
			m1 = (float)array[array_len/2];
			//fprintf(f1, "%.1f\n", (float)array[array_len/2]);
			//printf("%.1f, ", (float)array[array_len/2]);
		} else {
			//even
			m1 = (array[array_len/2 - 1] + array[array_len/2 ])/2.0;
			//fprintf(f1, "%.1f\n", (array[array_len/2 - 1] + array[array_len/2 ])/2.0);
			//printf("%.1f, ", (array[array_len/2 - 1] + array[array_len/2 ])/2.0);
		}
		fprintf(f1, "%.1f\n", m1);
	}
	printf("method 1 done...\n\n\n");
	fclose(f1);


	for(i=0; i<input_size; i++) {
		float median = updateHeap(input[i]);
		fprintf(f2, "%.1f\n", median);
		//printf("%.1f, ", median);
	}
	fclose(f2);
	printf("method 2 done...\n\n\n");*/
	return 0;
}
