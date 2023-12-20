/***********************************************************************
name: Kevin Kinney
	lineNum -- see if a word is in the online dictionary
description:	
	See CS 360 IO lecture for details.
***********************************************************************/

/* Includes and definitions */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h> 

// helper function (do not use on buffer)
int len(char * word) {
	int length = 0;
	while (word[length] != '\0') {
		length++;
	}
	return length;
}

/**********************************************************************
Search for given word in dictionary using file descriptor fd.
Return the line number the word was found on, negative of the last line searched
if not found, or the error number if an error occurs.
**********************************************************************/
int lineNum(char *dictionaryName, char *word, int dictWidth) {
	
	// open file
	int fd = open(dictionaryName, O_RDONLY, 0);
	// error check
	if (fd < 0) {
		//fprintf(stderr, "Can't open %s for reading -- %s\n", dictionaryName, strerror(errno));
		write(2, strerror(errno), strlen(strerror(errno)));
		return errno;
	}

	// turn searched word into comparable entry in dictionary
	char newWord[dictWidth];
	int wordLength = len(word);

	for (int i = 0; i < dictWidth-1; i++) {
		if (i < wordLength) {
			newWord[i] = word[i];
		} else {
			newWord[i] = ' ';
		}
	}
	newWord[dictWidth-1] = '\n';
	newWord[dictWidth] = '\0';
	
	// var inits
	char buffer[dictWidth];
	int bRead = 0;
	int cmp = 0;

	memset(buffer, '\0', dictWidth);

	// binary search vars (offset is start of dictionary entry)
	int min = 1;
	int mid = 0;
	int max = lseek(fd, 0, SEEK_END);
	int offset = 0;

	int checkedArray[500]; // could change size (log2 would be best)
	int caIndex = 0;
	
	mid = (max + min)/2;
	offset = mid - (mid%(dictWidth));
	offset = lseek(fd, offset, SEEK_SET);

	while ((bRead = read(fd, buffer, dictWidth)) > 0) {

		// base case (was the entry previously checked?)
		for (int i = 0; i < caIndex; i++) {
			if (checkedArray[i] == offset) {
				close(fd);
				return -((checkedArray[i]/dictWidth)+1);
			}
		}

		cmp = strncmp(buffer, newWord, dictWidth); 
		checkedArray[caIndex] = offset;
		caIndex++;
		memset(buffer, '\0', dictWidth);
		
		// binary search logic
		if (cmp < 0) {
			min = mid + 1;
			mid = (min + max)/2;
		} else {
			if (cmp > 0) {
				max = mid - 1;
				mid = (min + max)/2;

			} else {
				// cmp == 0
				//printf("Found it at line %d\n", (mid/dictWidth)+1);
				close(fd);
				return (mid/dictWidth)+1;
			}
		}

		offset = mid - (mid%(dictWidth));
		offset = lseek(fd, offset, SEEK_SET);
	}

	// error check
	if (bRead < 0) {
		//fprintf(stderr, "Cannot safely read from file -- %s\n", strerror(errno));
		write(2, strerror(errno), strlen(strerror(errno)));
		return errno;
	}

	close(fd);

	return 0;
}

/*
// for debugging
int main() {
	printf("word should be on line %d\n", lineNum("tiny_9", "000", 9));
	printf("word should be on line %d\n", lineNum("webster_16", "mellow", 16));
	return 0;
}*/

