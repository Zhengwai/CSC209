#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "helper.h"

int main(int argc, char **argv){
	
	//Check number of arguments right away
	if (argc != 7) {
        	fprintf(stderr, "Usage: psort -n <number of processes> -f <inputfile> -o <outputfile>\n");
        	exit(1);
    }

	int number_process = 0;

	//Input and output file
    char *infile = NULL;
	char *outfile = NULL;

 	//Read in arguments
	read_arguments(argc, argv, &number_process, &infile, &outfile);

	int size_of_file = get_file_size(infile);
	int num_records = size_of_file / sizeof(struct rec);

	//Set the number of processes
	set_num_process(&number_process, num_records);	

	//Needed for reading from infile
	int work_distribution[number_process];
	int num_records_before = 0;

	/*My pipe status definition: 	
	-1 no more records to read
	1 means need to read another record from pipe
	0 means dont read anything from pipe as we have not used the last read record
	*/
	int pipe_status[number_process];

	//Distribute the work to the number of children and initialize our pipe status for writing
	for (int i = 0; i < number_process; i++){
		pipe_status[i] = 1;
		work_distribution[i] = num_records / number_process;	
	} 
	for (int i = 0; i < num_records % number_process; i ++){
		work_distribution[i] += 1;	
	}	

	int pipe_fd[number_process][2];

	for (int i = 0; i < number_process; i++){
		
		//Pipe wrapper function
		Pipe(pipe_fd[i]);

		int n = fork();
		if (n < 0){
			perror("fork");
			exit(1);	
		//Childs work
		}else if (n == 0){
			//Child only writes to the pipe so close the reading
			if (close(pipe_fd[i][0]) == -1){
				perror("couldn't close reading end from inside child");
				exit(1);
			}
			//Close all the previously forked children
			for (int child_num = 0; child_num < i; child_num ++){
				if (close(pipe_fd[child_num][0]) == -1){
					perror("couldn't close read from previously forked children");				
					exit(1);
				}
			}

			//Read our specified chunk
			struct rec records[work_distribution[i]];
			read_records(infile, num_records_before, work_distribution[i], records);		
			
			//Sort
			qsort(records, work_distribution[i], sizeof(struct rec), compare_freq);	

			//Write one by one and then close
			for (int j = 0; j < work_distribution[i]; j++){
				if (write(pipe_fd[i][1], &records[j], sizeof(struct rec)) != sizeof(struct rec)){
					perror("write error");
					exit(1);
				} 				
			}

			if (close(pipe_fd[i][1]) == -1){
					perror("couldn't close write end from forked children");				
					exit(1);
			}
			exit(0);
		//Parents work		
		}else{
			//Close write from parent 
			if (close(pipe_fd[i][1]) == -1){
				perror("couldn't close write from parent");
				exit(1);			
			}			
			
		}
		num_records_before += work_distribution[i];			
	}
	//Only the parent process gets here
	WaitForChildren(number_process);

	//Merge the data of the different pipes and write to the specified outfile
	Merge(num_records, number_process, &outfile, pipe_fd, pipe_status);
	
	//Close the reading portion of the pipes when done 
	for (int i = 0; i < number_process; i++){
		if (close(pipe_fd[i][0]) == -1){
			perror("close write from parent");
			exit(1);			
		}	
	} 
}

