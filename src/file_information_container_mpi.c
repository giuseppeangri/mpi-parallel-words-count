#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include <file_information.h>
#include <file_information_container.h>

int FileInformationContainer_calculateSendPackSize(FileInformationContainer * filesContainer) {

    int sendPackSize  = 0;
    int toAddPackSize = 0;

    // FileInformationContainer.num_files
    MPI_Pack_size(1, MPI_INT, MPI_COMM_WORLD, &toAddPackSize);
    sendPackSize += toAddPackSize;

    // FileInformation.pathLength
    MPI_Pack_size(filesContainer->num_files, MPI_UNSIGNED_LONG, MPI_COMM_WORLD, &toAddPackSize);
    sendPackSize += toAddPackSize;

    // FileInformation.path
    for(int i=0; i<filesContainer->num_files; i++) {
        MPI_Pack_size(filesContainer->files[i]->pathLength, MPI_CHAR, MPI_COMM_WORLD, &toAddPackSize);
        sendPackSize += toAddPackSize;
    }

    // FileInformation.size
    MPI_Pack_size(filesContainer->num_files, MPI_DOUBLE, MPI_COMM_WORLD, &toAddPackSize);
    sendPackSize += toAddPackSize;

    return sendPackSize;

}

void * FileInformationContainer_makeSendPackBuffer(FileInformationContainer * filesContainer, int sendPackSize) {

    void * packedBuffer = malloc(sendPackSize);
    int    position     = 0;
    
    MPI_Pack(&filesContainer->num_files, 1, MPI_INT, packedBuffer, sendPackSize, &position, MPI_COMM_WORLD); 

    for(int i=0; i<filesContainer->num_files; i++) {
        MPI_Pack(&filesContainer->files[i]->pathLength, 1, MPI_UNSIGNED_LONG, packedBuffer, sendPackSize, &position, MPI_COMM_WORLD);            
        MPI_Pack(filesContainer->files[i]->path, filesContainer->files[i]->pathLength, MPI_CHAR, packedBuffer, sendPackSize, &position, MPI_COMM_WORLD);            
        MPI_Pack(&filesContainer->files[i]->size, 1, MPI_DOUBLE, packedBuffer, sendPackSize, &position, MPI_COMM_WORLD);  
    }

    return packedBuffer;

}

void FileInformationContainer_unpack(FileInformationContainer * filesContainer, void * recvBuffer, int recvBufferSize) {

    int numFiles = 0;
    int position = 0;

    MPI_Unpack(recvBuffer, recvBufferSize, &position, &numFiles, 1, MPI_INT, MPI_COMM_WORLD);

    for(int i=0; i<numFiles; i++) {

        FileInformation * fileInformation = (FileInformation *) malloc(sizeof(FileInformation));

        MPI_Unpack(recvBuffer, recvBufferSize, &position, &fileInformation->pathLength, 1, MPI_UNSIGNED_LONG, MPI_COMM_WORLD);

        fileInformation->path = malloc(fileInformation->pathLength * sizeof(char));
        MPI_Unpack(recvBuffer, recvBufferSize, &position, fileInformation->path, fileInformation->pathLength, MPI_CHAR, MPI_COMM_WORLD);

        MPI_Unpack(recvBuffer, recvBufferSize, &position, &fileInformation->size, 1, MPI_DOUBLE, MPI_COMM_WORLD);
        
        FileInformationContainer_add(filesContainer, fileInformation);

    }

}