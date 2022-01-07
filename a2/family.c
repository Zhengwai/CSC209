#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "family.h"

/* Number of word pointers allocated for a new family.
   This is also the number of word pointers added to a family
   using realloc, when the family is full.
*/
static int family_increment = 0;


/* Set family_increment to size, and initialize random number generator.
   The random number generator is used to select a random word from a family.
   This function should be called exactly once, on startup.
*/
void init_family(int size) {
    family_increment = size;
    srand(time(0));
}


/* Given a pointer to the head of a linked list of Family nodes,
   print each family's signature and words.

   Do not modify this function. It will be used for marking.
*/
void print_families(Family* fam_list) {
    int i;
    Family *fam = fam_list;
    
    while (fam) {
        printf("***Family signature: %s Num words: %d\n",
               fam->signature, fam->num_words);
        for(i = 0; i < fam->num_words; i++) {
            printf("     %s\n", fam->word_ptrs[i]);
        }
        printf("\n");
        fam = fam->next;
    }
}

/* Return a pointer to a new family whose signature is 
   a copy of str. Initialize word_ptrs to point to 
   family_increment+1 pointers, numwords to 0, 
   maxwords to family_increment, and next to NULL.
*/
Family *new_family(char *str) {
	//Allocate space for the struct
	Family *new_fam = malloc(sizeof(Family));
	//Error checking
	if (new_fam == NULL) {
        	perror("malloc");
        	exit(1);
    	}
	//Pointers array
	new_fam->word_ptrs = malloc((family_increment + 1) * sizeof(*(new_fam->word_ptrs)));
	//Error checking
	if (new_fam->word_ptrs == NULL) {
        	perror("malloc");
        	exit(1);
    	}
	//Allocate space for the signature
	new_fam->signature = malloc(sizeof(char) * (strlen(str) + 1));
	//Error checking
	if (new_fam->signature == NULL) {
        	perror("malloc");
        	exit(1);
    	}
	//Copy the signature in
	strcpy(new_fam->signature, str);
	new_fam->num_words = 0;
	new_fam->max_words = family_increment;
	new_fam->next = NULL;
	
    return new_fam;
}

/* Add word to the next free slot fam->word_ptrs.
   If fam->word_ptrs is full, first use realloc to allocate family_increment
   more pointers and then add the new pointer.
*/
void add_word_to_family(Family *fam, char *word) {
	if (fam->num_words == fam->max_words){
		//reallocate
		fam->word_ptrs = realloc(fam->word_ptrs, (family_increment + fam->max_words) * sizeof(char*));
		//Error checking
		if (fam->word_ptrs == NULL) {
	        	perror("realloc");
	        	exit(1);
    		}
		//We just increased the max words
		fam->max_words += family_increment;
		fam->word_ptrs[fam->num_words] = word;
		//Added one word, so increment
		fam->num_words += 1;
		//Last element is null
		fam->word_ptrs[fam->num_words] = NULL;
	}else{
		//reallocate
		fam->word_ptrs[fam->num_words] = word;
		fam->num_words += 1;
		//Last element is null
		//Check if the element just added fills the last spot, if so allocate one more spot for a null pointer
		//that marks the end of an array
		if (fam->num_words == fam->max_words){
			fam->word_ptrs = realloc(fam->word_ptrs, (1 + fam->max_words) * sizeof(char*));
			//Error checking
			if (fam->word_ptrs == NULL) {
	        		perror("realloc");
	        		exit(1);
    			}
			fam->word_ptrs[fam->num_words] = NULL;
		}else{
			fam->word_ptrs[fam->num_words] = NULL;
		}
		
		
	}
	return;
}

/* Return a pointer to the family whose signature is sig;
   if there is no such family, return NULL.
   fam_list is a pointer to the head of a list of Family nodes.
*/
Family *find_family(Family *fam_list, char *sig) {
	Family *next_element;
	
	//Case of the empty family
	if(fam_list == NULL){
		return NULL;
	}
	
	//Check the first element
	if (!(strcmp(fam_list->signature, sig))){
			return fam_list;
	}
	//Go to the next if the first element is not the proper
	next_element = fam_list->next;
	
	//Walk the list
	while (next_element){
		//Compare
		if (!(strcmp(next_element->signature, sig))){
			return next_element;
		}
		//Go to the next element
		next_element = next_element->next;
	}
	//Returns null if nothing was returned before
    	return NULL;
}

/* Return a pointer to the family in the list with the most words;
   if the list is empty, return NULL. If multiple families have the most words,
   return a pointer to any of them.
   fam_list is a pointer to the head of a list of Family nodes.
*/
Family *find_biggest_family(Family *fam_list) {
	Family *biggest;
	biggest = fam_list;
	
	if (biggest == NULL){
		return NULL;
	}
	
	Family *next_element;
	next_element = fam_list-> next;
	
	//Walk
	while (next_element){
		//compare
		if (next_element->num_words > biggest->num_words){
			biggest = next_element;
		}
		next_element = next_element->next;
	}
	
    return biggest;
}

/* Deallocate all memory rooted in the List pointed to by fam_list. */
void deallocate_families(Family *fam_list) {
	//Two pointers needed to walk the list
	Family *next_element;
	Family *prev_element;
	
	//If there is no family list, there is nothing to deallocate, so return
	if (fam_list == NULL){
		return;
	}
	
	//Get the next element
	next_element = fam_list->next;
	
	//Free the first element
	free(fam_list->word_ptrs);
	free(fam_list->signature);
	free(fam_list);
	
	//While there is a next element, walk down the list
	while (next_element){
		//Save the previous element so we can deallocate
		prev_element = next_element;
		//Move
		next_element = next_element->next;
		//Free
		free(prev_element->word_ptrs);
		free(prev_element->signature);
		free(prev_element);
	}
	
	return;
}

/* Generate and return a linked list of all families using words pointed to
   by word_list, using letter to partition the words.

   Implementation tips: To decide the family in which each word belongs, you
   will need to generate the signature of each word. Create only the families
   that have at least one word from the current word_list.
*/
Family *generate_families(char **word_list, char letter) {
	int i = 0;
	int num_fams = 0;
	Family *generated_family = NULL;
	Family *last;
	Family *check_family;
	//Create the most general family, words that do not contain the letter at all
	char signature[strlen(word_list[i]) + 1];
	while (word_list[i] != NULL){
		//Create the signature
		for (int j = 0; j < strlen(word_list[i]); j++){
			if (word_list[i][j] == letter){
				signature[j] = letter;
			}else{
				signature[j] = '-';
			}
		}
		//End the signature
		signature[strlen(word_list[i])] = '\0';
		//After creating the signature, check family
		check_family = find_family(generated_family, signature);
		//NULL means this family doesnt exist yet
		if (check_family == NULL){
			check_family = new_family(signature);
			num_fams += 1;
			
			//Check if we just created the first family, if so set it to the head
			if (num_fams == 1){
				generated_family = check_family;
				//Set the last element of the newly created family list to the first element.
				last = generated_family;
			}else{
				last->next = check_family;
				last = last->next;
			}

			//Add to the family now
			add_word_to_family(last, word_list[i]);	
		//If it already exists, avoid adding to the new family
		}else{
			add_word_to_family(check_family, word_list[i]);
		}
		i += 1;
	}
	//Return the head of the generated family
	return generated_family;
}

/* Return the signature of the family pointed to by fam. */
char *get_family_signature(Family *fam) {
    return fam->signature;
}


/* Return a pointer to word pointers, each of which
   points to a word in fam. These pointers should not be the same
   as those used by fam->word_ptrs (i.e. they should be independently malloc'd),
   because fam->word_ptrs can move during a realloc.
   As with fam->word_ptrs, the final pointer should be NULL.
*/
char **get_new_word_list(Family *fam) {
	char **word_list_to_return = malloc((fam->num_words + 1) * sizeof(*word_list_to_return));
	//Error checking
	if (word_list_to_return == NULL) {
        	perror("malloc");
        	exit(1);
    	}
	int i = 0;
	while(fam->word_ptrs[i] != NULL){
		//printf("tricked\n");
		word_list_to_return[i] = fam->word_ptrs[i];
		i += 1;
	}
	//Last slot is NULL, we know i is the last spot because the while loop ended
	word_list_to_return[i] = NULL;
    return word_list_to_return;
}

/* Return a pointer to a random word from fam. 
   Use rand (man 3 rand) to generate random integers.
*/
char *get_random_word_from_family(Family *fam) {
	int random_number = rand() % fam->num_words;
	return fam->word_ptrs[random_number];
}
