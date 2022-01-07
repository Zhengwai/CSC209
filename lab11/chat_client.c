#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#ifndef PORT
  #define PORT 30000
#endif
#define BUF_SIZE 128

int main(void) {
    // Create the socket FD.
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("client: socket");
        exit(1);
    }

    // Set the IP and port of the server to connect to.
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &server.sin_addr) < 1) {
        perror("client: inet_pton");
        close(sock_fd);
        exit(1);
    }

    // Connect to the server.
    if (connect(sock_fd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("client: connect");
        close(sock_fd);
        exit(1);
    }
	
	
	//Get the username from the user and write to socket
	char buf[BUF_SIZE + 1];

	printf("Enter your username: \n");
	int num_read = read(STDIN_FILENO, buf, BUF_SIZE);
	if (num_read == 0) {
		exit(1);
	}
	buf[num_read] = '\0';
	
	//Write to socket
	int num_written = write(sock_fd, buf, num_read);
	if (num_written != num_read) {
            perror("client: write");
            close(sock_fd);
            exit(1);
    }
	
	// First, we prepare to listen to multiple
    // file descriptors by initializing a set of file descriptors.
    int max_fd = sock_fd;
    fd_set all_fds;
    FD_ZERO(&all_fds);
    FD_SET(sock_fd, &all_fds);
	FD_SET(STDIN_FILENO, &all_fds);
	
	if (STDIN_FILENO > sock_fd){
		max_fd = STDIN_FILENO;
	}
	while (1) {
		// select updates the fd_set it receives, so we always use a copy and retain the original.
        fd_set listen_fds = all_fds;
        int nready = select(max_fd + 1, &listen_fds, NULL, NULL, NULL);
        if (nready == -1) {
            perror("server: select");
            exit(1);
        }
		
		//Echos if someone else typed something
		if (FD_ISSET(sock_fd, &listen_fds)) {
            int num_read = read(sock_fd, buf, 2*BUF_SIZE+3);
            if(num_read < 0){
                perror("read");
                exit(1);
            } else if(num_read == 0){
                printf("Server terminated\n");
				exit(0);
            }
            buf[num_read] = '\0';
			//which one?
			//printf("Received from server: %s", buf);
            printf("%s", buf);
        }
		
		//Read input from the user, send it to the server
		if (FD_ISSET(STDIN_FILENO, &listen_fds)){
			int num_read = read(STDIN_FILENO, buf, BUF_SIZE);
			if (num_read == 0) {
				break;
			}
			buf[num_read] = '\0';         

			int num_written = write(sock_fd, buf, num_read);
			if (num_written != num_read) {
				perror("client: write");
				close(sock_fd);
				exit(1);
			}
		}
	}

    close(sock_fd);
    return 0;
}
