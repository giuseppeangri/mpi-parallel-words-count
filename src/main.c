#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include <mpi_utils.h>
#include <file_utils.h>

#include <file_information.h>
#include <file_information_container.h>

#include <counter.h>
#include <counter_container.h>

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

	// CounterContainer_print(&entriesContainer);

	// CounterContainer_addWord(&entriesContainer, "Lorem");

	// CounterContainer_print(&entriesContainer);

	// CounterContainer_addWord(&entriesContainer, "ipsum");

	// CounterContainer_print(&entriesContainer);

	// CounterContainer_addWord(&entriesContainer, "ipsum");

	// CounterContainer_print(&entriesContainer);

	// exit(0);

	startRead(my_rank, split_size, filesContainer, entriesContainer, log_file);

    int rec = 1;
    int rv[10];
    MPI_Gather(&rec, 1, MPI_INT, &rv, 1, MPI_INT, master_rank, MPI_COMM_WORLD);

    if(my_rank == master_rank)
    	MPI_Print_To_File(log_file, my_rank, "END");

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