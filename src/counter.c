#include <stdlib.h>
#include <string.h>

#include <counter.h>

Counter * Counter_constructor(char * word, int count) {

    Counter * entry = (Counter *) malloc(sizeof(Counter));

    entry->length = strlen(word)+1;

    entry->word = malloc(entry->length * sizeof(char));
    strcpy(entry->word, word);

    entry->count = count;

    return entry;

}