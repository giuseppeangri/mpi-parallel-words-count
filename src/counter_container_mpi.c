#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include <counter.h>
#include <counter_container.h>

int CounterContainer_calculateSendPackSize(CounterContainer * entriesContainer) {

    int sendPackSize  = 0;
    int toAddPackSize = 0;

    MPI_Pack_size(1, MPI_INT, MPI_COMM_WORLD, &toAddPackSize);
    sendPackSize += toAddPackSize;

    MPI_Pack_size(entriesContainer->size, MPI_UNSIGNED_LONG, MPI_COMM_WORLD, &toAddPackSize);
    sendPackSize += toAddPackSize;
    
    for(int i=0; i<entriesContainer->size; i++) {
        MPI_Pack_size(entriesContainer->entries[i]->length, MPI_CHAR, MPI_COMM_WORLD, &toAddPackSize);
        sendPackSize += toAddPackSize;
    }

    MPI_Pack_size(entriesContainer->size, MPI_INT, MPI_COMM_WORLD, &toAddPackSize);
    sendPackSize += toAddPackSize;

    return sendPackSize;

}

int CounterContainer_calculateRecvPacksBufferSize(int num_processes, int master_rank, int * packsSizes) {

    int recvBufferSize = 0;

    for(int i=0; i<num_processes; i++) {
        if(i != master_rank) {
            recvBufferSize += packsSizes[i];
        }
    }

    return recvBufferSize;

}

void CounterContainer_calculateDisplacements(int num_processes, int master_rank, int * packsSizes, int * displacements) {

    int alreadyDisplaced = 0;
        
    for(int i=0; i<num_processes; i++) {
        displacements[i]  = alreadyDisplaced;
        alreadyDisplaced += (i == master_rank) ? 0 : packsSizes[i];
    }

}

void * CounterContainer_makeSendPackBuffer(CounterContainer * entriesContainer, int sendPackSize) {

    void * packedBuffer = malloc(sendPackSize);
    int    position     = 0;
    
    MPI_Pack(&entriesContainer->size, 1, MPI_INT, packedBuffer, sendPackSize, &position, MPI_COMM_WORLD); 

    for(int i=0; i<entriesContainer->size; i++) {
        MPI_Pack(&entriesContainer->entries[i]->length, 1, MPI_UNSIGNED_LONG, packedBuffer, sendPackSize, &position, MPI_COMM_WORLD);            
        MPI_Pack(entriesContainer->entries[i]->word, entriesContainer->entries[i]->length, MPI_CHAR, packedBuffer, sendPackSize, &position, MPI_COMM_WORLD);            
        MPI_Pack(&entriesContainer->entries[i]->count, 1, MPI_INT, packedBuffer, sendPackSize, &position, MPI_COMM_WORLD);  
    }

    return packedBuffer;

}

void CounterContainer_merge(CounterContainer * entriesContainer, void * recvBuffer, int recvBufferSize, int numContainers) {

    int position = 0;

    for(int i=0; i<numContainers; i++) {

        int recvCountersSize;

        MPI_Unpack(recvBuffer, recvBufferSize, &position, &recvCountersSize, 1, MPI_INT, MPI_COMM_WORLD);

        for(int j=0; j<recvCountersSize; j++) {

            Counter * entry = malloc(sizeof(Counter));

            MPI_Unpack(recvBuffer, recvBufferSize, &position, &entry->length, 1, MPI_UNSIGNED_LONG, MPI_COMM_WORLD);
            
            entry->word = malloc(entry->length * sizeof(char));
            
            MPI_Unpack(recvBuffer, recvBufferSize, &position, entry->word, entry->length, MPI_CHAR, MPI_COMM_WORLD);
    
            MPI_Unpack(recvBuffer, recvBufferSize, &position, &entry->count, 1, MPI_INT, MPI_COMM_WORLD);
            
            CounterContainer_findAndAddEntry(entriesContainer, entry);

        }

    }

}