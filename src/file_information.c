#include <stdlib.h>
#include <sys/stat.h>

#include <file_information.h>

FileInformation * FileInformation_constructor(char * path) {

	FileInformation * fileInformation = (FileInformation *) malloc(sizeof(FileInformation));

	fileInformation->path = path;

	struct stat buffer;
	int status = stat(path, &buffer);

	fileInformation->size = (status == 0) ? buffer.st_size : -1;

	return fileInformation;

}