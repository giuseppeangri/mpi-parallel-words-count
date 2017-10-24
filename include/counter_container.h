typedef struct CounterContainer {

	int			size;
	Counter **	entries;

} CounterContainer;

extern CounterContainer CounterContainer_constructor();
extern int  CounterContainer_addEntry(CounterContainer * entriesContainer, Counter * entry);
extern int  CounterContainer_indexOfWord(CounterContainer * entriesContainer, char * word);
extern int  CounterContainer_findAndAddEntry(CounterContainer * entriesContainer, Counter * entry);
extern int  CounterContainer_findAndAddWord(CounterContainer * entriesContainer, char * word, int count);
extern void CounterContainer_print(CounterContainer * entriesContainer);
extern void CounterContainer_printToFile(CounterContainer * entriesContainer, FILE * log_file);