#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "helper.h"

/* A function created for the merging of the different pipes and writing to the output file */
void Merge(int num_records, int number_process, char **outfile, int pipe_fd[][2], int pipe_status[]){

	//Need these to write to outfile
	int num_write = 0;
	struct rec records[number_process];
	struct rec smallest_rec;
	int smallest_index;

	//Open the file using our wrapper function
	FILE *words = Fopen(outfile, "wb");

	//Begin reading from the pipes, we know we're not done until we've written num_records records
	while (num_write != num_records){
		for (int j = 0; j < number_process; j ++){
			if (pipe_status[j] == 1){
				pipe_status[j] = 0;			
				int read_code = read(pipe_fd[j][0], &records[j], sizeof(struct rec));
				if(read_code == -1){
					perror("read");
					exit(1);			
				}else if(read_code == 0){
					pipe_status[j] = -1;			
				}	
			}			
		}
		
		//Finds the first available smallest record to start the search for the smallest element
		for (int j = 0; j < number_process; j++){
			if (pipe_status[j] != -1){
				smallest_rec = records[j];
				smallest_index = j;
				break;
			}			
		}
		
		//Find the smallest element out of the current available records
		for (int j = 0; j < number_process; j ++){
			if (pipe_status[j] != -1){
				if (records[j].freq < smallest_rec.freq){
					smallest_rec = records[j];
					smallest_index = j;				
				}
			}			
		}
		pipe_status[smallest_index] = 1;		

		if (fwrite(&smallest_rec, sizeof(struct rec), 1, words) != 1){
			fprintf(stderr, "error writing struct to file\n");
			exit(1);		
		}	
		//Get here if we successfully write		
		num_write += 1;
	}
	Fclose(words);
}

void WaitForChildren(int number_process){
	int status;
	
	//Waiting for the children
	for (int i = 0; i < number_process; i ++){
			//Wait
			if (wait(&status) == -1){
				perror("wait");
				exit(1);			
			}else{
				if (WIFEXITED(status)){
					//do nothing if the child exits with a call on exit()
				}
				else if (WIFSIGNALED(status)){
					fprintf(stderr, "Child terminated abnormally\n");
				}else{
					//Catches anything else that is abnormal
					fprintf(stderr, "Child terminated abnormally\n");
				}
			}
	}
}

/* A function created to set the number of processes needed */
void set_num_process(int *number_process, int num_records){
	//If the file is empty, we do not want any processes created
	if (num_records <= 0){
		*number_process = 0;	
	}

	//If the number of processes specified is more than the number of records, we will take the mod which 
	//will allocate mod number of processes, instead of the specified 
	if (*number_process > num_records){
		*number_process = num_records % *number_process;	
	}
}

/* A function created to help with the piping of children */
void Pipe(int *pipe_fd) {
	if (pipe(pipe_fd) == -1) {
		perror("pipe");
		exit(1);
	}
}

/* A function created to be a wrapper function for fclose */
void Fclose(FILE *file_to_close) {
	int error;
	error = fclose(file_to_close);
	
	if (error != 0) {
		fprintf(stderr, "fclose failed\n");
		exit(1);
	}
	
	return;
}

/* A function created to be a wrapper function for fopen */
FILE *Fopen(char **file_name, char *file_type) {
	
	FILE *file;
	file = fopen(*file_name, file_type);
	
	if (file == NULL) {
		fprintf(stderr, "Cannot open file\n");
		exit(1);
	}
	
	return file;	
}

/* A function created to help with the reading of the arguments */
void read_arguments(int argc, char **argv, int *num_process, char **infile, char **outfile){
	extern char *optarg;
    	int ch;

	//Get the options provided by the user
	while ((ch = getopt(argc, argv, "n:f:o:")) != -1) {
        	switch(ch) {
        	case 'n':
            		*num_process = atoi(optarg);
            		break;
        	case 'f':
            		*infile = optarg;
            		break;
		case 'o':
			*outfile = optarg;
			break;
        	default:
            		fprintf(stderr, "Usage: psort -n <number of processes> -f <inputfile> -o <outputfile>\n");
            		exit(1);
        	}
    	}
}

/* A function created to help with reading specified chunks of a binary file */
void read_records(char *f_name, int num_records_before, int num_to_read, struct rec my_records[]){

	FILE *words = fopen(f_name, "rb");

	//Check if file can be opened
	if (words == NULL){
		fprintf(stderr, "Cannot open file\n");
		exit(1);	
	}
	
	//Jump if its not the first child process
	if (num_records_before != 0){	
		fseek(words, sizeof(struct rec) * num_records_before, SEEK_SET);	
	}

	//Read the specified number of elements
	for (int j = 0; j < num_to_read; j++){
		int error = fread(&my_records[j], sizeof(struct rec), 1, words);
		//Check for fread failure
		if (error != 1) {
			if (feof(words)) {
				break;
			}else if (ferror(words)) {
				fprintf(stderr, "couldn't read from file\n");
				exit(1);
			}
		}
	}

	//Close
	int error = fclose(words);
	if (error != 0){
		fprintf(stderr, "fclose failed\n");
		exit(1);
	}
}

int get_file_size(char *filename) {
    struct stat sbuf;

    if ((stat(filename, &sbuf)) == -1) {
       perror("stat");
       exit(1);
    }

    return sbuf.st_size;
}

/* A comparison function to use for qsort */
int compare_freq(const void *rec1, const void *rec2) {

    struct rec *r1 = (struct rec *) rec1;
    struct rec *r2 = (struct rec *) rec2;

    if (r1->freq == r2->freq) {
        return 0;
    } else if (r1->freq > r2->freq) {
        return 1;
    } else {
        return -1;
    }
}
