#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE* openFile(char * filePath, char * mode) {

	FILE * file = fopen(filePath, mode);

    if(file == NULL) {
    	printf("ERROR: opening file %s\n", filePath);
    	exit(EXIT_FAILURE);
    }

    return file;

}

char * readLine(FILE * file) {

	char * line = NULL;
	size_t len  = 0;

	getline(&line, &len, file);
	free(line);
	
	return line;

}

char ** readAllLines(FILE * file) {

	char ** allLines 			= (char **) malloc(sizeof(char *));
	char * 	singleLine 			= (char *) malloc(sizeof(char));
	size_t 	singleLine_size 	= 0;
	size_t 	singleLine_length 	= 0;
	int 	linesCount 			= 0;

	while( (singleLine_length = getline(&singleLine, &singleLine_size, file)) != (size_t) -1 ) {

		// New line found
		// Increment counter and realloc allLines
		linesCount++;
		allLines = (char **) realloc(allLines, sizeof(char *) * linesCount);

		// Clean the line removing '/n'
		char * singleLineCleaned;
		size_t singleLineCleaned_length;

		if(singleLine[singleLine_length-1] == '\n') {
			singleLineCleaned_length = singleLine_length-1;
		}
		else {
			singleLineCleaned_length = singleLine_length;
		}

		singleLineCleaned = (char *) malloc(singleLineCleaned_length * sizeof(char));
		strncpy(singleLineCleaned, singleLine, singleLineCleaned_length);
		singleLineCleaned[singleLineCleaned_length] = '\0';
		
		// Add cleaned line to allLines
		allLines[linesCount-1] = (char *) malloc((singleLineCleaned_length) * sizeof(char));
		strncpy(allLines[linesCount-1], singleLineCleaned, singleLineCleaned_length);
		allLines[linesCount-1][singleLineCleaned_length] = '\0';

		// Free singleLine and allocate new memory for next line
		free(singleLine);
		singleLine = (char *) malloc(sizeof(char));

		// Free singleLineCleaned
		free(singleLineCleaned);

	}

	// Add NULL terminator
	allLines = (char **) realloc(allLines, sizeof(char *) * (linesCount+1));
	allLines[linesCount] = NULL;

	return allLines;

}