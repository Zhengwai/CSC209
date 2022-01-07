#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void print_state(char *state, int size);
void update_state(char *state, int size);


int main(int argc, char **argv) {

    if (argc != 3) {
    	fprintf(stderr, "Usage: USAGE: life initial n\n");
    	return 1;
    }

    int size = strlen(argv[1]);
	char state[size];
	int n = strtol(argv[2], NULL, 10);
	
	//Declare the array
	for (int i = 0; i < size; i++){
		//We access it like this because it is a string
		state[i] = argv[1][i];
	}
    
	//Do the cycles
	for (int j = 0; j < n; j ++){
		print_state(state, size);
		update_state(state, size);
	}
}