typedef struct Counter {

    size_t  length;
    char *  word;
    int     count;
    
} Counter;

extern Counter * Counter_constructor(char * word, int count);