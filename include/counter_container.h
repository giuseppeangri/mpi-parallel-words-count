typedef struct CounterContainer {

	Counter **	entries;
	int			size;

} CounterContainer;

extern CounterContainer CounterContainer_constructor();
extern int CounterContainer_IndexOfWord(CounterContainer * entriesContainer, char * word);
extern int CounterContainer_addEntry(CounterContainer * entriesContainer, Counter * entry);
extern int CounterContainer_addWord(CounterContainer * entriesContainer, char * word);
extern void CounterContainer_print(CounterContainer * entriesContainer);
extern void CounterContainer_printToFile(CounterContainer * entriesContainer, FILE * log_file);