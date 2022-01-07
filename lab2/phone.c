#include <stdio.h>
#include <stdlib.h>

/*
 * Sample usage:
 * $ gcc -Wall -std=gnu99 -g -o phone phone.c
 * ./phone
 * Enter these when prompted:
 * 0123456789 
 * 1
 */
int main() {
	char phone[11];
    int user_int;
	
	//Get the user phone number
	scanf("%s", &phone[0]);
	
	//Get the user integer
	scanf("%d", &user_int);
	

	//Print according to specifications
	if (user_int == -1){
		printf("%s\n", phone);
		return 0;
	}else if (user_int >= 0 && user_int <= 9){
		printf("%c\n", phone[user_int]);
		return 0;
	}else{
		printf("ERROR\n");
		return 1;
	}
}