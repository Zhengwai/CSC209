/* The purpose of this program is to practice writing signal handling
 * functions and observing the behaviour of signals.
 */

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

/* Message to print in the signal handling function. */
#define MESSAGE "%ld reads were done in %ld seconds.\n"

/* Global variables to store number of read operations and seconds elapsed. 
 */
long num_reads, seconds;

void handler(int sig){
	fprintf(stdout, MESSAGE, num_reads, seconds);
	exit(1);
}

/* The first command-line argument is the number of seconds to set a timer to run.
 * The second argument is the name of a binary file containing 100 ints.
 * Assume both of these arguments are correct.
 */

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: time_reads s filename\n");
        exit(1);
    }
    seconds = strtol(argv[1], NULL, 10);

    FILE *fp;
    if ((fp = fopen(argv[2], "r")) == NULL) {
      perror("fopen");
      exit(1);
    }

	//Our signal handler
    struct sigaction act;
    act.sa_handler = handler;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGPROF,&act,NULL);
	
	//Our itimerval
	struct itimerval alarm;
	alarm.it_value.tv_sec = seconds;
	alarm.it_value.tv_usec = 0;
	alarm.it_interval.tv_sec = seconds;
	alarm.it_interval.tv_usec = 0;
	if ((setitimer(ITIMER_PROF, &alarm, NULL)) == -1){
		perror("setitimer");
		exit(1);
	}
	
	num_reads = 0;
	
    /* In an infinite loop, read an int from a random location in the file,
     * and print it to stderr.
     */
    for (;;) {
        int n = rand() % 100;
		//Jump to a random position
        fseek(fp, n * sizeof(int), SEEK_SET);

        // Read the number at the random index
        int num;
        if (fread(&num, sizeof(int), 1, fp) != 1) {
            fprintf(stderr, "Error reading from file\n");
            exit(1);
        }

        fprintf(stderr, "%d\n", num);
        num_reads += 1;
    }
    return 1; // something is wrong if we ever get here!
}
