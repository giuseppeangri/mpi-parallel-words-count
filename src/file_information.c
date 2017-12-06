#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <file_information.h>

FileInformation * FileInformation_constructor(char * path) {

	FileInformation * fileInformation = (FileInformation *) malloc(sizeof(FileInformation));

	fileInformation->pathLength = strlen(path)+1;

    fileInformation->path = malloc(fileInformation->pathLength * sizeof(char));
    strcpy(fileInformation->path, path);

	struct stat buffer;
	int status = stat(path, &buffer);

	fileInformation->size = (status == 0) ? buffer.st_size : -1;

	return fileInformation;

}