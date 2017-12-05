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

int main(int argc, char * argv[]) {

// MPI INIZIALIZE

	int my_rank;
	int master_rank;
	int num_processes;
	double timeStart, timeEnd;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
	
	master_rank = 0;
	timeStart = MPI_Wtime();

// MAIN

	// Log File
	char * log_file_name;
	FILE * log_file;

	asprintf(&log_file_name, "../logs/log_%02d.txt", my_rank);
	log_file = openFile(log_file_name, "w");

	// Welcome message
	fprintf(log_file, "MPI Words Count Collective - Giuseppe Angri\n");
	(my_rank == master_rank) ? fprintf(log_file, "Rank %d (MASTER)\n\n", my_rank) : fprintf(log_file, "Rank %d\n\n", my_rank);

	printf("MPI Words Count Collective - Giuseppe Angri\n");
	(my_rank == master_rank) ? printf("Rank %d (MASTER)\n\n", my_rank) : printf("Rank %d\n\n", my_rank);

	// Get information about all files to read
	FileInformationContainer filesContainer = FileInformationContainer_buildByMasterFile(argv[1] ? argv[1] : "../data/00_master.txt");

	// Calculate split size that each process will have to read
	double split_size = (int) (filesContainer.total_size / num_processes);

	if(my_rank == master_rank) {
		fprintf(log_file, "Number of files: %d\n", filesContainer.num_files);
		fprintf(log_file, "Total size of files: %.2f bytes\n", filesContainer.total_size);
		fprintf(log_file, "Single split size: %f bytes\n\n", split_size);
	}

	// Words Count Process

	CounterContainer entriesContainer = CounterContainer_constructor();

	startReader(my_rank, split_size, &filesContainer, &entriesContainer, log_file);

	// Calculate Send Pack Size

	int sendPackSize = 0;

    if(my_rank != master_rank) {
		sendPackSize = CounterContainer_calculateSendPackSize(&entriesContainer);
	}

    // Gather Send Pack Size

    int packsSizes[num_processes];

	MPI_Gather(&sendPackSize, 1, MPI_INT, packsSizes, 1, MPI_INT, master_rank, MPI_COMM_WORLD);

	// Allocate Recv Pack Buffer and Calculate Displacements

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

    MPI_Gatherv(packedBuffer, sendPackSize, MPI_PACKED, 
        recvBuffer, packsSizes, displacements, MPI_PACKED, 
        master_rank, MPI_COMM_WORLD);

    // Merge all packs

    if(my_rank == master_rank) {
        CounterContainer_merge(&entriesContainer, recvBuffer, recvBufferSize, num_processes-1);

		fprintf(log_file, "\nGlobal Histogram\n\n");
		CounterContainer_printToFile(&entriesContainer, log_file);

		printf("Global Histogram\n\n");
        CounterContainer_print(&entriesContainer);
    }

    fclose(log_file);

// MPI CLOSE

	timeEnd = MPI_Wtime();
	MPI_Print(my_rank, "Execution Time: %f seconds", timeEnd-timeStart);
	
	MPI_Finalize(); 	
	exit(EXIT_SUCCESS);

}