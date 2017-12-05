#include <stdio.h>
#include <stdarg.h>

void MPI_Print(int rank, const char * format, ...) {

	printf("RANK %02d - ", rank);
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
	printf("\n");

}

void MPI_PrintIndented(int rank, const char * format, ...) {
	
	for(int i=0; i<rank; i++)
		printf("\t");

	printf("RANK %02d - ", rank);
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
	printf("\n");

}

void MPI_Print_To_File(FILE * log_file, int rank, const char * format, ...) {
	
	fprintf(log_file, "RANK %02d - ", rank);
	va_list args;
	va_start(args, format);
	vfprintf(log_file, format, args);
	va_end(args);
	fprintf(log_file, "\n");

}