typedef struct FileInformationContainer {

	FileInformation ** 	files;
	int 				num_files;
	double 				total_size;

} FileInformationContainer;

extern FileInformationContainer FileInformationContainer_constructor();
extern void FileInformationContainer_add(FileInformationContainer * fileInformationContainer, FileInformation * fileInformation);
extern void FileInformationContainer_addByPath(FileInformationContainer * fileInformationContainer, char * filePath);