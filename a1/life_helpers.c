#include <stdlib.h>
#include <stdio.h>

//Takes in a character array and the size of that array, and returns void.
//This function prints the characters of the given array followed by a single newline character.
void print_state(char *state, int size){
	for (int i = 0; i < size; i++){
		printf("%c", state[i]);
	}
	printf("\n");
}

//Takes in a character array and the size of that array, and returns void.
//This function updates the state of the array according to the rules given in the assignment.
void update_state(char *state, int size){
	
	//Create a copy of the old array so we can look back at it
	char old_state[size];
	
	for (int j = 0; j < size; j ++){
		old_state[j] = state[j];
	}
	
	//If the size is greater than two, then we can check for adjacent elements
	//If the size is < 2, we do nothing to the array because the first and last elements in the array
	//never change.																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																													
	if (size > 2){
		for (int i = 1; i < size - 1; i ++){
			//If immediate neighbours are both the same, change to .
			if ((old_state[i - 1] == '.' && old_state[i + 1] == '.') || (old_state[i - 1] == 'X' && old_state[i + 1] == 'X')){
				state[i] = '.';
			}else if ((old_state[i - 1] == '.' && old_state[i + 1] == 'X') || (old_state[i - 1] == 'X' && old_state[i + 1] == '.')){
				state[i] = 'X';
			}else{
				state[i] = state[i];
			}
		}
	}	
}

