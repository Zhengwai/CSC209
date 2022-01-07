#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    int i;
    int iterations;

    if (argc != 2) {
        fprintf(stderr, "Usage: forkloop <iterations>\n");
        exit(1);
    }

	//Get the parent id
	int parentPID = getpid();
    iterations = strtol(argv[1], NULL, 10);

    for (i = 0; i < iterations; i++) {
		//Only fork if the PID matches the parent PID
        if (getpid() == parentPID){
			int n = fork();
			if (n < 0) {
				perror("fork");
				exit(1);
			}
		}
        printf("ppid = %d, pid = %d, i = %d\n", getppid(), getpid(), i);
    }

    return 0;
}
