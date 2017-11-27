#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include <mpi_utils.h>
#include <file_utils.h>

#include <file_information.h>
#include <file_information_container.h>

#include <counter.h>
#include <counter_container.h>
#include <counter_container_mpi.h>

#include <file_reader.h>

FileInformationContainer makeContainer(char * masterFilePath);

int main(int argc, char * argv[]) {

// MPI VARIABLES

	int master_rank;
	int my_rank;
	int source_rank;
	int dest_rank;
	int num_processes;

	MPI_Status 	mpi_status;

// MPI INIZIALIZE

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
	
	master_rank = 0;

// OTHER VARIABLES

	FILE * log_file;
	char * log_file_name;

	FileInformationContainer filesContainer;

	CounterContainer entriesContainer;

	double split_size;

// MAIN

	// Log File
	asprintf(&log_file_name, "../logs/log_%02d.txt", my_rank);

	log_file = openFile(log_file_name, "w");

	// Welcome message
	MPI_Print_To_File(log_file, my_rank, "MPI Words Count Collective - Giuseppe Angri");

	// Get information about all files to read
	filesContainer = makeContainer(argv[1] ? argv[1] : "../data/00_master.txt");

	// Calculate split size that each process will have to read
	// split_size = round( (filesContainer.total_size / num_processes)*10 ) / 10;
	split_size = (int) (filesContainer.total_size / num_processes);

	if(my_rank == master_rank) {
		MPI_Print_To_File(log_file, my_rank, "Number of files: %d", filesContainer.num_files);
		MPI_Print_To_File(log_file, my_rank, "Total size of files: %.2f bytes", filesContainer.total_size);
		MPI_Print_To_File(log_file, my_rank, "Single split size: %f bytes\n\n", split_size);
		
		MPI_Print(my_rank, "Number of files: %d", filesContainer.num_files);
		MPI_Print(my_rank, "Total size of files: %.2f bytes", filesContainer.total_size);
		MPI_Print(my_rank, "Single split size: %f bytes\n\n", split_size);
	}

	entriesContainer = CounterContainer_constructor();

	startReader(my_rank, split_size, &filesContainer, &entriesContainer, log_file);

	// Calculate Send Pack Size

    int sendPackSize = 0;

    if(my_rank != master_rank) {
		sendPackSize = CounterContainer_calculateSendPackSize(&entriesContainer);
	}

	printf("Rank: %d - SEND PACK SIZE: %d\n", my_rank, sendPackSize);

    // Gather Send Pack Size

    int packsSizes[num_processes];

	MPI_Gather(&sendPackSize, 1, MPI_INT, packsSizes, 1, MPI_INT, master_rank, MPI_COMM_WORLD);

	// Prepare Recv Pack Buffer and Displacements

	int     recvBufferSize;
	void *  recvBuffer = NULL;
	int 	displacements[num_processes];

	if(my_rank == master_rank) {
		recvBufferSize = CounterContainer_calculateRecvPacksBufferSize(num_processes, master_rank, packsSizes);
		recvBuffer = malloc(recvBufferSize);
		CounterContainer_calculateDisplacements(num_processes, master_rank, packsSizes, displacements);
	}

	// Make Send Pack Buffer

	void * packedBuffer;

	if(my_rank != master_rank) {
		packedBuffer = CounterContainer_makeSendPackBuffer(&entriesContainer, sendPackSize);
	}

	// Gather Send Pack

    printf("R: %d - sendCount:  %d\n", my_rank, sendPackSize);

    MPI_Gatherv(packedBuffer, sendPackSize, MPI_PACKED, 
        recvBuffer, packsSizes, displacements, MPI_PACKED, 
        master_rank, MPI_COMM_WORLD);

    // Merge all packs

    if(my_rank == master_rank) {
        CounterContainer_merge(&entriesContainer, recvBuffer, recvBufferSize, num_processes-1);
        CounterContainer_print(&entriesContainer);
    }

    fclose(log_file);

// MPI CLOSE
	
	MPI_Finalize(); 	
	exit(EXIT_SUCCESS);

}

FileInformationContainer makeContainer(char * masterFilePath) {

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