#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <filereader.h>

int main(int argc, char* argv[]) {

// MPI VARIABLES

	int master_rank;
	int my_rank;
	int num_processes;
	MPI_Status mpi_status;

	int source_rank;
	int dest_rank;

// MPI INIZIALIZE

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
	
	master_rank = 0;

// OTHER VARIABLES

// MAIN

	printf("Hello, I'm the processor: %d\n", my_rank);

// MPI CLOSE
	
	MPI_Finalize(); 	
	return 0;

}
