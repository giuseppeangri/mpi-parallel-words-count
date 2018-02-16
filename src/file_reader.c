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

void checkStartingPoint(int my_rank, double split_size, FileInformationContainer * filesContainer, FILE ** file_to_read, int * file_to_read_index, int * offset) {

	char current_char;
	double already_read_by_others = 0;

	// Calculate the size already read by previous processes
    already_read_by_others = my_rank * split_size;

	if(my_rank > 1) {
		already_read_by_others += my_rank-1;
	}
    // Calculate the file to read and the remaining part of size already read by previous processes
    *file_to_read_index = 0;
	
    while(already_read_by_others >= filesContainer->files[*file_to_read_index]->size) {
    	already_read_by_others -= (filesContainer->files[*file_to_read_index]->size);
    	(*file_to_read_index)++;
    }

	// MPI_PrintIndented(my_rank, "file_to_read_index %d", *file_to_read_index);

	// Open the first file that my process must read
    *file_to_read = openFile(filesContainer->files[*file_to_read_index]->path, "r");

    // Move in the part of file that my process must read
    fseek(*file_to_read, already_read_by_others, SEEK_SET);

    *offset = 0;

	if(already_read_by_others > 0) {

		// non parto dall'inizio

		current_char = fgetc(*file_to_read);

		// MPI_Print(my_rank, "file %d", file_to_read_index);
		// MPI_Print(my_rank, "already_read_by_others %f", already_read_by_others);
		// MPI_PrintIndented(my_rank, "start %c", current_char);

		if(isalnum(current_char)) {

			// se è un char torna indietro fino a spazio

			while(isalnum(current_char)) {

				(*offset)++;
				already_read_by_others--;

				if(already_read_by_others >= 0) {
					fseek(*file_to_read, already_read_by_others, SEEK_SET);
					current_char = fgetc(*file_to_read);
				}
				else {
					// sono tornato all'inizio del file
					rewind(*file_to_read);
					break;
				}
				
			}

		}

	}
	
}

void resetWordBuffer(char * word, int * word_index) {
	// Clear current word and reset current word index
	memset(word, 0, 100 * (sizeof word[0]) );
	*word_index = 0;
}

void addWordToContainer(CounterContainer * entriesContainer, int * words_found, char * word, int * word_index) {

	CounterContainer_findAndAddWord(entriesContainer, word, 1);
	(*words_found)++;

	resetWordBuffer(word, word_index);

}

void startReader(int my_rank, double split_size, FileInformationContainer * filesContainer, CounterContainer * entriesContainer, FILE * log_file) {

	FILE * 	file_to_read;
	int 	file_to_read_index; 

	int     offset;
	double  already_read_by_me;
	int 	words_found;
	
	char 	current_char;

	char 	current_word[100];
	int 	current_word_index = 0;

	// Check Starting Point
	file_to_read = NULL;
	checkStartingPoint(my_rank, split_size, filesContainer, &file_to_read, &file_to_read_index, &offset);

    // Start reading
    already_read_by_me = 0;
    words_found         = 0;

	resetWordBuffer(current_word, &current_word_index);

    // Read my split size
    while(already_read_by_me < (split_size+offset) ) {    	

		current_char = fgetc(file_to_read);

		if(current_char == EOF) {

			if(current_word_index > 0) {
				// In this case i read a space
				// So i read a whole word

				// MPI_Print_To_File(log_file, my_rank, "Word readed: %s", current_word);
				addWordToContainer(entriesContainer, &words_found, current_word, &current_word_index);

			}

			// In this case the file is finished
			// So i need to read the next available file

			fclose(file_to_read);
			file_to_read_index++;

			// Check if all files was readed
			if(file_to_read_index >= filesContainer->num_files) {
				file_to_read = NULL;
				break;
			}

			// MPI_Print_To_File(log_file, my_rank, "Skip to file: %d", file_to_read_index);

			file_to_read = openFile(filesContainer->files[file_to_read_index]->path, "r");

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
				addWordToContainer(entriesContainer, &words_found, current_word, &current_word_index);

			}

		}

		// Anyway i must increase the part readed by me
		already_read_by_me++;

    }

    // MPI_PrintIndented(my_rank, "end %c", current_char);
	
	if(file_to_read != NULL) {

		current_char = fgetc(file_to_read);

		// MPI_PrintIndented(my_rank, "next end %c", current_char);

		if( (!isalnum(current_char)) ) {

			if(current_word_index > 0) {

				// In this case i read a space
				// So i read a whole word

				// MPI_Print_To_File(log_file, my_rank, "Final Word readed: %s", current_word);
				addWordToContainer(entriesContainer, &words_found, current_word, &current_word_index);

			}

		}

	}

	fprintf(log_file, "Size Readed: %f bytes\n", already_read_by_me);
    // MPI_Print(my_rank, "I read this size: %f", already_read_by_me);

    fprintf(log_file, "Words Found: %d\n\n", words_found);
    // MPI_Print(my_rank, "Words Found: %d", words_found);

    // CounterContainer_print(&entriesContainer);
    fprintf(log_file, "Local Histogram\n\n");
    CounterContainer_printToFile(entriesContainer, log_file);

}