#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXLINE 256
#define MAX_PASSWORD 10
#define SUCCESS "Password verified\n"
#define INVALID "Invalid password\n"
#define NO_USER "No such user\n"

int main(void) {
  char user_id[MAXLINE];
  char password[MAXLINE];
  int fork_check;
  int status;
  int fd[2];

  if(fgets(user_id, MAXLINE, stdin) == NULL) {
      perror("fgets");
      exit(1);
  }
  if(fgets(password, MAXLINE, stdin) == NULL) {
      perror("fgets");
      exit(1);
  }
  //Pipe
   if (pipe(fd)==-1){
	  perror("pipe");
	  exit(1);
  }
  //Fork
  fork_check = fork();
  //Error check
  if (fork_check == -1){
	perror("fork");
	exit(1);
  }else if (fork_check == 0){
	 //Child does work here
	//Child reads so close the write end
	close(fd[1]);
	dup2(fd[0], fileno(stdin));
	//Close reading after
	close(fd[0]);
	execl("./validate", "validate", NULL);
	//Check for execl errors
	perror("execl");
    return 1;
  }else{
	//Parent's work
	//close reading
	close(fd[0]);
	//Write the password and user id
	if (write(fd[1], user_id, MAX_PASSWORD) == -1) {
		perror("writing");
		exit(1);
    }
    if (write(fd[1], password, MAX_PASSWORD) == -1) {
		perror("writing");
		exit(1);
    }
	//close writing
    close(fd[1]);
  }
  
  //Check the exit codes
  if (wait(&status) == -1){
	perror("Wait");
	exit(1);
  }
  
  if (WIFEXITED(status)){
	int exit_status = WEXITSTATUS(status);
	if (exit_status == 0){
		printf("%s", SUCCESS);
	}else if (exit_status == 2){
		printf("%s", INVALID);
	}else if (exit_status == 3){
		printf("%s", NO_USER);	
	}
  }
  
  return 0;
}
