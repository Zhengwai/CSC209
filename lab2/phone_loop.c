#include <stdio.h>
#include <stdlib.h>

int main() {
	char phone[11];
    	int user_int;
	int errors = 0;
	
	//Get the user phone number
	scanf("%s", &phone[0]);
	
	do{
		//Get the user integer
		int result = scanf("%d", &user_int);
		
		//Breaks if ctrl d is hit marking the end of standard input
		if(result == EOF) break;
		
		//Print according to specifications
		if (user_int == -1){
			printf("%s\n", phone);
		}else if (user_int >= 0 && user_int <= 9){
			printf("%c\n", phone[user_int]);
		}else{
			printf("ERROR\n");
			errors = errors + 1;
		}
		
	}while (1);
	
	if (errors != 0){
		return 1;
	}else{
		return 0;
	}
}