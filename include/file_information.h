typedef struct FileInformation {

	char * path;
	double size;

} FileInformation;

extern FileInformation * FileInformation_constructor(char * path);