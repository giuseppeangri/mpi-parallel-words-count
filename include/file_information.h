typedef struct FileInformation {

	size_t pathLength;
	char * path;
	double size;

} FileInformation;

extern FileInformation * FileInformation_constructor(char * path);