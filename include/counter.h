typedef struct Counter {

	char *	word;
	int		count;

} Counter;

extern Counter * Counter_constructor(char * word);