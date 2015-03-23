# insight_data_engineering_code_challenge
Insight Data Engineering Code Challenge


Question 1, Word Count

This is question, I implemented a trie data structure to count word occurrences. For example, a text file "A cat eat a big apple." will result in a trie looks like:

	 	  root
	/    |     |	\
   	a(2)  b(0)  c(0)  e(0)
   	|     |     |	 |
   	p(0)  i(0)  a(0)  a(0)
   	|     |     |     |
	 p(0)  g(1)  t(1)  t(1)
   	|
	 l(0)
   	|
   	e(0)

Each node in the trie has two fields, char letter and int count. The integer value in each node next to the letter indicates the word occurrence (i.e. the 2 next to 'a' means 'a' occurs twice, and 1 next to 'e' tells 'apple' occurs once).

At the end of parsing files, a complete trie is built. Then by recursively traversing the trie, we can list the words and occurrences in alphabetical order. 

The trie data structure is very efficient, insert one word costs O(word_length), to parse all files costs O(word_length * N) where N is the total number of words and word_length is the average length of words. This is faster than a binary search tree which costs O(word_length * logN * N). The trade-off is that trie costs more memory.

In order to test the performance of trie, I implemented a brute force algorithm, and run both algorithms on a large text file. The followings are some statistics:
- Text file size: 6.5MB
- Total words count: 1115049 words.

Brute fource algorithm:
	- Execution time: 30.757596 seconds.
	- The amount of memory used: 0.4645 MB.
	
Trie data structure algorithm:
	- Execution time: 0.678882 seconds.
	- The amount of memory used: 16.5517 MB.



Question 2, running median

This this problem, I implemented Heap data structure. The running median is computed using two heaps - Max Heap and Min Heap. All the numbers less than or equal to the current median are in the Max Heap, which is arranged so that the maximum number is at the root of the heap. All the numbers greater than or equal to the current median are in the Min Heap, which is arranged so that the minimum number is at the root of the heap. Also, instead of inserting duplicate numbers to heap, I create a Node structure which has a integer field that counts the occurrences of the numbers. Thus, every node in the heap has distinct value (the value represents words count for a line). This saves a hugh ammount of memory space. 

For example, if a file has 5 lines of 10 words, 8 lines of 20 words, and 7 lines of 30 words. After parsing the file, heaps will look like the following:

	Max Heap		
	(20, 5)						
	/							
(10, 5)	
       
       	Min Heap
       (20, 3)
       /
(30, 7)
       
       
The algorithm complexity is:
O(n) for inserting value into heap, since I need to linearly check whether the heap already contains the value.
O(1) for calculating current median.
Memory space cost is O(M) where M is the number of distinct values.



