extern int    FileInformationContainer_calculateSendPackSize(FileInformationContainer * filesContainer);
extern void * FileInformationContainer_makeSendPackBuffer(FileInformationContainer * filesContainer, int sendPackSize);
extern void   FileInformationContainer_unpack(FileInformationContainer * filesContainer, void * recvBuffer, int recvBufferSize);