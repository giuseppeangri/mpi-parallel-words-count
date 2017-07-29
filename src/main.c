#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#include <mpi_utils.h>
#include <file_reader.h>
#include <file_information.h>
#include <file_information_container.h>

#include <ctype.h>

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

	FileInformationContainer container;

	double 	split_size;

	FILE * 	file_to_read;
	int 	file_to_read_index; 
	double 	already_read_by_others;
	double 	already_read_by_me;

	char 	current_char;

	char 	current_word[100];
	int 	current_word_index;

// MAIN

	// Log File
	asprintf(&log_file_name, "../logs/log_%02d.txt", my_rank);

	log_file = openFile(log_file_name, "w");

	// Welcome message
	MPI_Print_To_File(log_file, my_rank, "MPI Words Count Collective - Giuseppe Angri");

	// Get information about all files to read
	container = makeContainer(argv[1] ? argv[1] : "../data/00_master.txt");

	// Calculate split size that each process will have to read
	// split_size = round( (container.total_size / num_processes)*10 ) / 10;
	split_size = (int) (container.total_size / num_processes);

	if(my_rank == master_rank) {
		MPI_Print_To_File(log_file, my_rank, "Number of files: %d", container.num_files);
		MPI_Print_To_File(log_file, my_rank, "Total size of files: %.2f bytes", container.total_size);
		MPI_Print_To_File(log_file, my_rank, "Single split size: %f bytes\n\n", split_size);

		MPI_Print(my_rank, "Number of files: %d", container.num_files);
		MPI_Print(my_rank, "Total size of files: %.2f bytes", container.total_size);
		MPI_Print(my_rank, "Single split size: %f bytes\n\n", split_size);
	}

	// Calculate the size already read by previous processes
    already_read_by_others = my_rank * split_size;

    // Calculate the file to read and the remaining part of size already read by previous processes
    file_to_read_index = 0;
	
    while(already_read_by_others >= container.files[file_to_read_index]->size) {
    	already_read_by_others -= (container.files[file_to_read_index]->size);
    	file_to_read_index++;
    }

    already_read_by_others += my_rank-1;

    // Open the first file that my process must read
    file_to_read = openFile(container.files[file_to_read_index]->path, "r");

    // Move in the part of file that my process must read
    fseek(file_to_read, already_read_by_others, SEEK_SET);

    int offset = 0;

	if(already_read_by_others > 0) {

		// non parto dall'inizio

		current_char = fgetc(file_to_read);

		MPI_Print(my_rank, "file %d", file_to_read_index);
		MPI_Print(my_rank, "already_read_by_others %f", already_read_by_others);
		MPI_Print(my_rank, "start %c", current_char);

		if(isalnum(current_char)) {

			// se è un char torna indietro fino a spazio

			while(isalnum(current_char)) {

				offset++;
				already_read_by_others--;

				if(already_read_by_others >= 0) {
					fseek(file_to_read, already_read_by_others, SEEK_SET);
					current_char = fgetc(file_to_read);
				}
				else {
					// sono tornato all'inizio del file
					rewind(file_to_read);
					break;
				}
				
			}


		}

	}    

    // Start reading
    already_read_by_me = 0;
    current_word_index = 0;

    int word_founded = 0;

    // Read my split size
    while(already_read_by_me < (split_size+offset) ) {    	

		current_char = fgetc(file_to_read);

		if(current_char == EOF) {

			if(current_word_index > 0) {
				// In this case i read a space
				// So i read a whole word

				MPI_Print_To_File(log_file, my_rank, "Word readed: %s", current_word);
				word_founded++;

				// Clear current word and reset current word index
				memset(&current_word[0], 0, sizeof(current_word));
				current_word_index = 0;
			}

			// In this case the file is finished
			// So i need to read the next available file

			fclose(file_to_read);
			file_to_read_index++;

			// Check if all files was readed
			if(file_to_read_index >= container.num_files)
				break;

			MPI_Print_To_File(log_file, my_rank, "Skip to file: %d", file_to_read_index);

			file_to_read = openFile(container.files[file_to_read_index]->path, "r");

			continue;
		}

		if( isalnum(current_char) ) {

			// In this case the current char is an alphanumeric character
			// So i need to add this to current word

			current_word[current_word_index] = current_char;
			current_word_index++;

		}
		else if( isspace(current_char) || iscntrl(current_char) ) {

			if(current_word_index > 0) {
				// In this case i read a space
				// So i read a whole word

				MPI_Print_To_File(log_file, my_rank, "Word readed: %s", current_word);
				word_founded++;

				// Clear current word and reset current word index
				memset(&current_word[0], 0, sizeof(current_word));
				current_word_index = 0;
			}

		}

		// Anyway i must increase the part readed by me
		already_read_by_me++;

    }

    MPI_Print(my_rank, "end %c", current_char);

    MPI_Print_To_File(log_file, my_rank, "I read this size: %f", already_read_by_me);
    MPI_Print(my_rank, "I read this size: %f", already_read_by_me);

    current_char = fgetc(file_to_read);

    MPI_Print(my_rank, "next end %c", current_char);

    if( (!isalnum(current_char)) ) {

    	if(current_word_index > 0) {

	    	// In this case i read a space
			// So i read a whole word

			MPI_Print_To_File(log_file, my_rank, "Final Word readed: %s", current_word);
			word_founded++;

			// Clear current word and reset current word index
			memset(&current_word[0], 0, sizeof(current_word));
			current_word_index = 0;

		}

    }

    MPI_Print_To_File(log_file, my_rank, "Word Founded: %d", word_founded);
    MPI_Print(my_rank, "Word Founded: %d", word_founded);

    int rec = 1;
    int rv[10];
    MPI_Gather(&rec, 1, MPI_INT, &rv, 1, MPI_INT, master_rank, MPI_COMM_WORLD);

    if(my_rank == master_rank)
    	MPI_Print_To_File(log_file, my_rank, "END");


    fclose(file_to_read);
    fclose(log_file);

// MPI CLOSE
	
	MPI_Finalize(); 	
	exit(EXIT_SUCCESS);

}

FileInformationContainer makeContainer(char * masterFilePath) {

	FILE * masterFile = fopen(masterFilePath, "r");

	FileInformationContainer container = FileInformationContainer_constructor();

    char **pathList = readAllLines(masterFile);

    char **singlePath = pathList;

    while(*singlePath != NULL) {
    	FileInformationContainer_addByPath(&container, *singlePath);
    	++singlePath;
    }

    fclose(masterFile);

	return container;

}