/* 
 * This program takes in a text file 
 * or set of text files from a directory "wc_input"
 * and outputs the number of occurrences for each 
 * word to a file "wc_result.txt", ,which is placed 
 * in a directory named "wc_output".
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

#define NODE_CHILDREN_SIZE 26

typedef struct Node{
	char letter;
	int count;
	struct Node *children[NODE_CHILDREN_SIZE]; //26 children nodes for 'a'-'z'
	//For simplicity, assume that the text files do not contain any digits.
} Node;

//Some global variables
Node *root; //This root of a trie
int nodes_count; //Keep track of how many nodes are allocted through out the program


/* 
 * This function allocates a Node, and assigns it initial values. 
 */
Node* createNode(int letter){
	Node *n = (Node *)malloc(sizeof(Node));
	n->letter = letter;
	n->count = 0;
	int i;
	for(i=0; i<NODE_CHILDREN_SIZE; i++)
		n->children[i] = NULL;
	nodes_count++;
	return n;
}

/* 
 * This function recursively traverse the trie,
 * and prints words and occurrences in alphabetical order.
 */
char *word_to_print;
int word_to_print_size;
int len;
void printWordList(Node *n, FILE *fout){
	if(n == NULL) return;
	//if a word has more than 256 characters, realloc the memory
	if(len == word_to_print_size) {
		word_to_print_size *= 2;
		word_to_print = (char*)realloc(word_to_print, sizeof(char) * word_to_print_size);
	}
	word_to_print[len++] = n->letter;
	//print out the word if the occurrence counter > 0
	if(n->count > 0) {
		word_to_print[len] = 0;
		fprintf(fout, "%s %d\n", word_to_print, n->count);
		//printf("%s %d\n", word_to_print, n->count);
	}
	//go to the next level
	int i;
	for(i=0; i<NODE_CHILDREN_SIZE; i++) {
		if(n->children[i] != NULL) {
			printWordList(n->children[i], fout);
			len--;
		}
	}
}

void freeTrie(Node *n) {
	if(n == NULL) return;
	int i;
	for(i=0; i<NODE_CHILDREN_SIZE; i++)
		freeTrie(n->children[i]);
	free(n);
}


/*
 * This function gets one word from a file,
 * return NULL if it's end of the file,
 * otherwise return the word.
 */
char * nextword(FILE *fd) {
	int max_word_len = 256;
	char* word = (char*)malloc(sizeof(char) * max_word_len);
	int len = 0;

	char c;
	while((c = fgetc(fd)) != EOF) {
		//skip non-alphabetical chars 
		//we need to do this in order to handle the case like: word1 ,,.. ,,.. word2,
		//where there are leading spaces or scpecial chars in front of a word.
		while(c != EOF && !isalpha(c))
			c = fgetc(fd);

		//read concective letters or digits
		while(c != EOF && (isalpha(c) || c == '\'' || c == '-') ) {
			//For simplicity, remove hyphens and apostrophes.
			if(c == '\'' || c == '-') {
				c = fgetc(fd);
				continue; 
			}

			//convert to lower case
			if(c >= 'A' && c <='Z' ) c = c - 'A' + 'a'; 

			//make sure there's enough space for the word
			if(len == max_word_len) {
				max_word_len *= 2;
				word = (char *)realloc(word, max_word_len);
			}

			//now put the char into word array
			word[len++] = c;
			c = fgetc(fd);
		}
		word[len] = 0;
		return len == 0? NULL : word;
	}
	return NULL;
}


/*
 * This function opens a file, extract one word at a time from the file,
 * and then update the trie. At the end of the function, the file is 
 * completely processed, and the trie is up-to-date that contains
 * the words occurrences. 
 */
void wordcount(char* filepath) {
	FILE * fd = fopen(filepath, "r");
	if(fd == NULL){
		perror("Open file error");
		return;
	}
	//printf("Process %s\n", filepath);

	char *word;
	char *wordcopy;
	int index, i;
	Node *p;
	//The while loop gets one word at a time from the file
	while((word = nextword(fd)) != NULL ) {
		//printf("%s\n", word);
		//Insert the word into trie
		p = root;
		wordcopy = word;
		while(*word != '\0') {
			//convert 'a'-'z' to 0 - 25
			index = (*word) - 'a';

			//create node if the character is not in the trie
			if(p->children[index] == NULL) {
				Node *n = createNode(*word);
				p->children[index] = n;
			}
			p = p->children[index];
			word++;
		}
		free(wordcopy);
		//After inserting the word into trie, increment the occurrence counter. 
		p->count++;
	}
	fclose(fd);
}


int main(int argc, char* argv[]) {
	//check arguments
	if(argc != 3) {
		printf("Usage: ./wordcount ./input_directory ./output_directory/output_file\n");
		return 0;
	}

	//1. Initialization
	nodes_count = 0;
	root = createNode('\0');

	//2. Open the wc_input directory, and process each file in it
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (argv[1])) != NULL) {
  		while ((ent = readdir (dir)) != NULL) {
  			if((ent->d_name)[0] == '.') //hidden files
  				continue;
  			//get filepath of each file, process it, update trie
  			char *filepath = (char*)malloc(sizeof(char) * 256);
			strcpy(filepath, argv[1]);
			if(filepath[strlen(filepath) - 1] != '/') strcat(filepath, "/");
			strcat(filepath, ent->d_name);

    		wordcount(filepath);
    		free(filepath);
  		}
  		closedir (dir);
	} else {
  		perror ("Open directory error");
  		return 0;
	}

	//3. Print out the sorted word list
	FILE *fout = fopen(argv[2], "w");
	if(fout == NULL) {
		perror ("Open output file error");
  		return 0;
	}
	int i;
	for(i=0; i<NODE_CHILDREN_SIZE; i++) {
		if(root->children[i] != NULL){
			word_to_print_size = 256;
			len = 0;
			word_to_print = (char*)malloc(sizeof(char) * word_to_print_size);
			printWordList(root->children[i], fout);
			freeTrie(root->children[i]); //free memory
			free(word_to_print);
		}
	}
	fclose(fout);
	
	return 0;
}
