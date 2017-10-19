#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <mpi_utils.h>
#include <file_utils.h>

#include <file_information.h>
#include <file_information_container.h>

#include <counter.h>
#include <counter_container.h>

void startRead(int my_rank, double split_size, FileInformationContainer filesContainer, CounterContainer entriesContainer, FILE * log_file) {

	FILE * 	file_to_read;
	int 	file_to_read_index; 
	double 	already_read_by_others;
	double 	already_read_by_me;

	char 	current_char;

	char 	current_word[100];
	int 	current_word_index;

	// Calculate the size already read by previous processes
    already_read_by_others = my_rank * split_size;

    // Calculate the file to read and the remaining part of size already read by previous processes
    file_to_read_index = 0;
	
    while(already_read_by_others >= filesContainer.files[file_to_read_index]->size) {
    	already_read_by_others -= (filesContainer.files[file_to_read_index]->size);
    	file_to_read_index++;
    }

    already_read_by_others += my_rank-1;

	// Open the first file that my process must read
    file_to_read = openFile(filesContainer.files[file_to_read_index]->path, "r");

    // Move in the part of file that my process must read
    fseek(file_to_read, already_read_by_others, SEEK_SET);

    int offset = 0;

	if(already_read_by_others > 0) {

		// non parto dall'inizio

		current_char = fgetc(file_to_read);

		// MPI_Print(my_rank, "file %d", file_to_read_index);
		// MPI_Print(my_rank, "already_read_by_others %f", already_read_by_others);
		// MPI_Print(my_rank, "start %c", current_char);

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

    // Clear current word and reset current word index
	memset(&current_word[0], 0, sizeof(current_word));
	current_word_index = 0;

    int word_founded = 0;

    // Read my split size
    while(already_read_by_me < (split_size+offset) ) {    	

		current_char = fgetc(file_to_read);

		if(current_char == EOF) {

			if(current_word_index > 0) {
				// In this case i read a space
				// So i read a whole word

				// MPI_Print_To_File(log_file, my_rank, "Word readed: %s", current_word);
				CounterContainer_addWord(&entriesContainer, current_word);
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
			if(file_to_read_index >= filesContainer.num_files)
				break;

			MPI_Print_To_File(log_file, my_rank, "Skip to file: %d", file_to_read_index);

			file_to_read = openFile(filesContainer.files[file_to_read_index]->path, "r");

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

				// MPI_Print_To_File(log_file, my_rank, "Word readed: %s", current_word);
				CounterContainer_addWord(&entriesContainer, current_word);
				word_founded++;

				// Clear current word and reset current word index
				memset(&current_word[0], 0, sizeof(current_word));
				current_word_index = 0;
			}

		}

		// Anyway i must increase the part readed by me
		already_read_by_me++;

    }

    // MPI_Print(my_rank, "end %c", current_char);

    MPI_Print_To_File(log_file, my_rank, "I read this size: %f", already_read_by_me);
    // MPI_Print(my_rank, "I read this size: %f", already_read_by_me);

    current_char = fgetc(file_to_read);

    // MPI_Print(my_rank, "next end %c", current_char);

    if( (!isalnum(current_char)) ) {

    	if(current_word_index > 0) {

	    	// In this case i read a space
			// So i read a whole word

			// MPI_Print_To_File(log_file, my_rank, "Final Word readed: %s", current_word);
			CounterContainer_addWord(&entriesContainer, current_word);
			word_founded++;

			// Clear current word and reset current word index
			memset(&current_word[0], 0, sizeof(current_word));
			current_word_index = 0;

		}

    }

    MPI_Print_To_File(log_file, my_rank, "Word Founded: %d", word_founded);
    // MPI_Print(my_rank, "Word Founded: %d", word_founded);

    // CounterContainer_print(&entriesContainer);
    CounterContainer_printToFile(&entriesContainer, log_file);

    fclose(file_to_read);

}