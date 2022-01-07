#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
//#include<wait.h> 

int main(int argc, char **argv) {
    int i;
    int iterations;

    if (argc != 2) {
        fprintf(stderr, "Usage: forkloop <iterations>\n");
        exit(1);
    }

	int n;
    iterations = strtol(argv[1], NULL, 10);

    for (i = 0; i < iterations; i++) {
		//Fork once for the parent
		if(i == 0){
			n = fork();
		    if(n < 0) {
				perror("fork");
				exit(1);
			}
		}
		else{
			//Fork once per child
			if(n == 0){
				n = fork();
				if(n < 0) {
					perror("fork");
					exit(1);
				}
			}
		}

        printf("ppid = %d, pid = %d, i = %d\n", getppid(), getpid(), i);
    }

    return 0;
}
