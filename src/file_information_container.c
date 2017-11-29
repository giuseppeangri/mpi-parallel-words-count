#include <stdio.h>
#include <stdlib.h>

#include <file_utils.h>

#include <file_information.h>
#include <file_information_container.h>

FileInformationContainer FileInformationContainer_constructor() {

	FileInformationContainer fileInformationContainer;

	fileInformationContainer.files = (FileInformation **) malloc(sizeof(FileInformation *));
	fileInformationContainer.num_files = 0;
	fileInformationContainer.total_size = 0;

	return fileInformationContainer;

}

void FileInformationContainer_add(FileInformationContainer * fileInformationContainer, FileInformation * fileInformation) {

	fileInformationContainer->num_files++;
	fileInformationContainer->files = (FileInformation **) realloc(fileInformationContainer->files, sizeof(FileInformation *) * fileInformationContainer->num_files);

	fileInformationContainer->files[fileInformationContainer->num_files-1] = fileInformation;

	fileInformationContainer->total_size += fileInformation->size;

}

void FileInformationContainer_addByPath(FileInformationContainer * fileInformationContainer, char * filePath) {

	FileInformation * fileInformation = FileInformation_constructor(filePath);

	FileInformationContainer_add(fileInformationContainer, fileInformation);

}

FileInformationContainer FileInformationContainer_buildByMasterFile(char * masterFilePath) {

	FILE * masterFile = fopen(masterFilePath, "r");

	FileInformationContainer filesContainer = FileInformationContainer_constructor();

    char **pathList = readAllLines(masterFile);

    char **singlePath = pathList;

    while(*singlePath != NULL) {
    	FileInformationContainer_addByPath(&filesContainer, *singlePath);
    	++singlePath;
    }

    fclose(masterFile);

	return filesContainer;

}