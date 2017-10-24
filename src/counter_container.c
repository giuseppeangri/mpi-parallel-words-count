#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <counter.h>
#include <counter_container.h>

CounterContainer CounterContainer_constructor() {

	CounterContainer entriesContainer;

	entriesContainer.size 	 = 0;
	entriesContainer.entries = (Counter **) malloc(sizeof(Counter *));

	return entriesContainer;

}

int CounterContainer_addEntry(CounterContainer * entriesContainer, Counter * entry) {

	int index = entriesContainer->size;

	entriesContainer->size++;
	entriesContainer->entries = (Counter **) realloc(entriesContainer->entries, sizeof(Counter *) * entriesContainer->size);

	entriesContainer->entries[index] = entry;

	return index;

}

int CounterContainer_indexOfWord(CounterContainer * entriesContainer, char * word) {

	int size = entriesContainer->size;

	for(int index = 0; index < size; index++) {

		char * entryWord = entriesContainer->entries[index]->word;

		if(strcasecmp(word, entryWord) == 0) {
			return index;
		}

	}
	
	return -1;

}

int CounterContainer_findAndAddEntry(CounterContainer * entriesContainer, Counter * entry) {

    int foundIndex = CounterContainer_indexOfWord(entriesContainer, entry->word);

    if(foundIndex == -1) {

        // Word Not Found

        int addedIndex = CounterContainer_addEntry(entriesContainer, entry);

        return 	addedIndex;

    }
    else {

        // Word Found

        entriesContainer->entries[foundIndex]->count += entry->count;

        return foundIndex;

    }

}

int CounterContainer_findAndAddWord(CounterContainer * entriesContainer, char * word, int count) {

    Counter * entry = Counter_constructor(word, count);

    return CounterContainer_findAndAddEntry(entriesContainer, entry);

}

void CounterContainer_print(CounterContainer * entriesContainer) {

	int total_count = 0;

	for(int index = 0; index < entriesContainer->size; index++) {
		total_count += entriesContainer->entries[index]->count;
		printf("Word: %-25s- Count: %d\n", entriesContainer->entries[index]->word, entriesContainer->entries[index]->count);
	}

	printf("\nTotal Count: %d\n", total_count);

}

void CounterContainer_printToFile(CounterContainer * entriesContainer, FILE * log_file) {

	int total_count = 0;

	for(int index = 0; index < entriesContainer->size; index++) {
		total_count += entriesContainer->entries[index]->count;
		fprintf(log_file, "Word: %-25s- Count: %d\n", entriesContainer->entries[index]->word, entriesContainer->entries[index]->count);
	}

	fprintf(log_file, "\nTotal Count: %d\n", total_count);

}