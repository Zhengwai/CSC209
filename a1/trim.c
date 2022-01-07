#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Reads a trace file produced by valgrind and an address marker file produced
 * by the program being traced. Outputs only the memory reference lines in
 * between the two markers
 */

int main(int argc, char **argv) {
    
    if(argc != 3) {
         fprintf(stderr, "Usage: %s tracefile markerfile\n", argv[0]);
         exit(1);
    }

    // Addresses should be stored in unsigned long variables
    unsigned long start_marker, end_marker;
	
	//Open the marker file to read
	FILE* marker_fp = fopen(argv[2],"r");
	if(marker_fp == NULL ) {
		perror("Couldn't open marker file:");
		exit(1);
	}
	fscanf(marker_fp, "%lx %lx", &start_marker, &end_marker);
	fclose(marker_fp);

	char type;
	unsigned long instruction;
	int start = 0;
	int end_loop;
	
	//Open the trace file to read
	FILE* trace_fp = fopen(argv[1], "r");
	
	end_loop = fscanf(trace_fp, " %c %lx , %*d", &type, &instruction);
	while(end_loop != EOF){
		if (instruction == end_marker){
			start = 0;		
		}		
		//If we have started printing, print what is needed
		if (start){
			printf("%c,%#lx\n", type, instruction);			
		}	
		//If we have reached the start marker, start printing
		if (instruction == start_marker){
			start = 1;		
		}
		//If we have reached the end marker, start printing
		end_loop = fscanf(trace_fp, " %c %lx , %*d", &type, &instruction);	
    }

	fclose(trace_fp);
    return 0;
}
