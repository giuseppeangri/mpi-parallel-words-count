#include <stdlib.h>
#include <string.h>

#include <counter.h>

Counter * Counter_constructor(char * word) {

	Counter * entry = (Counter *) malloc(sizeof(Counter));

	entry->word = malloc(sizeof(char) * strlen(word));
	strcpy(entry->word, word);

	entry->count = 1;

	return entry;

}