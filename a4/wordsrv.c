#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>
#include <signal.h>

#include "socket.h"
#include "gameplay.h"


#ifndef PORT
    #define PORT 58740
#endif
#define MAX_QUEUE 5


void add_player(struct client **top, int fd, struct in_addr addr);
void remove_player(struct client **top, int fd);

/* These are some of the function prototypes that we used in our solution 
 * You are not required to write functions that match these prototypes, but
 * you may find the helpful when thinking about operations in your program.
 */
/* Send the message in outbuf to all clients */
void broadcast(struct game_state *game, char *outbuf);
/* A function created to advance the turn to the next appropriate person */
void announce_turn(struct game_state *game);
/* A function created to announce the appropriate message if someone wins the game */
void announce_winner(struct game_state *game, struct client *winner);
/* Move the has_next_turn pointer to the next active client */
void advance_turn(struct game_state *game);

/* A function created to reset input buffer */	
void reset_buffer(struct client *person);
/* A function created to read input */
int read_input(int cur_fd, struct client *p);
/* A function that removes client from the new_players linked list */
void remove_from_new_players(struct client **top, int fd);
/* A function created to check if a name is already taken */
int check_name_exists(struct client **cur_players, const char *buf);
/* A function created to display initial messages */
void display_join_messages(struct game_state *game, struct client *p, struct client *new_players);
/* A function created to start a new game */
void start_new_game(struct game_state *game, char *file_name);
/* A function created to check if an entire string is blank */
int check_if_blank(const char *s);
/* A malloc wrapper function */
void *Malloc(int size_t);

/* The set of socket descriptors for select to monitor.
 * This is a global variable because we need to remove socket descriptors
 * from allset when a write to a socket fails.
 */
fd_set allset;

int read_input(int cur_fd, struct client *p){
	int nbytes = read(cur_fd, p->in_ptr, MAX_NAME);
	//Logging message, how many bytes read
	fprintf(stdout, "[%d] Read %d bytes\n", p->fd, nbytes);
	//Error on read because the socket has been closed
	if (nbytes <= 0){
		return 1;
	}
					
	p->in_ptr += nbytes;
	*(p->in_ptr) = '\0';
					
	//Checks if the buffer contains the network newline
	char *checker = strstr(p->inbuf, "\r\n");
					
	//Completes the string if we have read the network newline
	if (checker != NULL){
		*checker = '\0';
	}else{
		return 2;
	}
	return 0;
}

void display_join_messages(struct game_state *game, struct client *p, struct client *new_players){
	//Display to everyone who has just joined the game
	char *msg = Malloc(sizeof(char) * MAX_BUF);
	//Log
	fprintf(stdout, "%s has just joined\n", (game->head)->name);
	fprintf(stdout, "It's %s's turn\n", (game->has_next_turn)->name);
	sprintf(msg, "%s has just joined\r\n", (game->head)->name);				
	broadcast(game, msg);

	//Display to the person who just joined the current game status
	status_message(msg, game);  
	if(write(p->fd, msg, strlen(msg)) <= 0) {
		fprintf(stdout, "Disconnect from %s\n", inet_ntoa(p->ipaddr));
		remove_player(&new_players, p->fd);
	} 							
	//}
	free(msg);
	//Announce the turn and prompt the appropriate person for the prompt
	announce_turn(game);
}

int check_if_blank(const char *s) {
	while (*s != '\0') {
		if (!isspace((unsigned char)*s)){
      			return 0;
		}
    		s++;
  	}
	//Return 1 if the entire string is blank	
	return 1;
}

void start_new_game(struct game_state *game, char *file_name){
	
	//Reset the game
	(game->dict).fp = NULL;
	(game->dict).size = get_file_length(file_name);
	init_game(game, file_name);

	//Tell everyone a new game has started
	char *msg = Malloc(sizeof(char) * MAX_BUF);
	fprintf(stdout, "New game\n");
	sprintf(msg, "\r\nLets start a new game!\r\n");  		
	broadcast(game, msg);             
	free(msg);

}

void broadcast(struct game_state *game, char *outbuf){
	struct client *p = game->head;
	struct client *temp;
	
	while (p != NULL){
		if (p->fd > -1) {
			temp = p->next;
			if(write(p->fd, outbuf, strlen(outbuf)) <= 0) {
				fprintf(stdout, "Disconnect from %s\n", inet_ntoa(p->ipaddr));
				remove_player(&(game->head), p->fd);
			}
		}
		p = temp;
	}
}

void announce_turn(struct game_state *game){
	struct client *p = game->head;
	struct client *temp;
	char *msg = Malloc(sizeof(char) * MAX_BUF);
	
	while (p != NULL){
		if (p->fd > -1) {
			//If it is not this players turn, let them know whose turn it is
			if (p != game->has_next_turn){
				temp = p->next;
				
				sprintf(msg, "It's %s's turn\r\n", (game->has_next_turn)->name);
				if(write(p->fd, msg, strlen(msg)) <= 0) {
					fprintf(stdout, "Disconnect from %s\n", inet_ntoa(p->ipaddr));
					remove_player(&(game->head), p->fd);
				}
			//If it is, prompt them to enter a letter
			}else{
				temp = p->next;
				sprintf(msg, "Your guess? \r\n");
				if(write(p->fd, msg, strlen(msg)) <= 0) {
					fprintf(stdout, "Disconnect from %s\n", inet_ntoa(p->ipaddr));
					remove_player(&(game->head), p->fd);
				}
			}
			p = temp;
		}
	}
	free(msg);
}

void announce_winner(struct game_state *game, struct client *winner){
	struct client *p = game->head;
	struct client *temp;
	char *msg = Malloc(sizeof(char) * MAX_BUF);
	
	//Log
	fprintf(stdout, "Game over! %s is the winner\n", winner->name);
	
	while (p != NULL){
		if (p->fd > -1) {
			//If this person is the winner, tell them that they won
			if (p->fd == winner->fd){
				temp = p->next;
				sprintf(msg, "The word was %s\r\nGame Over! You win!\r\n", game->word);
				if(write(p->fd, msg, strlen(msg)) <= 0) {
					fprintf(stdout, "Disconnect from %s\n", inet_ntoa(p->ipaddr));
					remove_player(&(game->head), p->fd);
				}
			//If not, let them the word and know who won
			}else{
				temp = p->next;
				sprintf(msg, "The word was %s\r\nGame Over! %s wins!\r\n", game->word, winner->name);
				if(write(p->fd, msg, strlen(msg)) <= 0) {
					fprintf(stdout, "Disconnect from %s\n", inet_ntoa(p->ipaddr));
					remove_player(&(game->head), p->fd);
				}
			}
			p = temp;
		}
	}
	free(msg);
}

void remove_from_new_players(struct client **top, int fd){
	struct client **p;
	//Walk the linked list and remove at the appropriate spot
	for (p = top; *p && (*p)->fd != fd; p = &(*p)->next)
        ;

   	if (*p) {
        struct client *t = (*p)->next;
		*p = t;
	}
}

void advance_turn(struct game_state *game){
	//If the current player is the end of the linked list, the next player is the head
	if ((game->has_next_turn)->next == NULL){
		game->has_next_turn = game->head;
	}else{
		game->has_next_turn = (game->has_next_turn)->next;
	}
}

int check_name_exists(struct client **cur_players, const char *buf){
	struct client **p;
	
	//Walk the linked list of players and check if the name buf entered is already taken
	for (p = cur_players; *p; p = &(*p)->next){
        if (!strcmp((*p)->name, buf)){
			return 1;		
		}
	}
	return 0;
}

void check_letter(struct game_state *game, char *buf, int guess_index){
	char *msg = Malloc(sizeof(char) * MAX_BUF);
	
	//Now check if the letter is in the word
	char *check = strchr(game->word, buf[0]);
	//Letter is in the word
	if (check != NULL){
		game->letters_guessed[guess_index] = 1;
		for (int i = 0; i < strlen(game->word); i ++){
			if (game->word[i] == buf[0]){
				game->guess[i] = buf[0];					
			}							
		}
	//Letter is not in the word
	}else{
		//Log
		fprintf(stdout, "Letter %s is not in the word\n", buf);
						
		sprintf(msg, "%c is not in the word \r\n", buf[0]);
		game->guesses_left -= 1;
		game->letters_guessed[guess_index] = 1;
		broadcast(game, msg);	
		//Move on to next player if the letter  is not valid				
		advance_turn(game);
	}
	free(msg);
}
				
void reset_buffer(struct client *person){
	person->in_ptr = person->inbuf;
	person->inbuf[0] = '\0';
}

void *Malloc(int size_t){
	void *return_address = malloc(size_t);
	if (return_address == NULL){
		perror("malloc");
		exit(1);	
	}
	return return_address;
}

/* Add a client to the head of the linked list
 */
void add_player(struct client **top, int fd, struct in_addr addr) {
    struct client *p = malloc(sizeof(struct client));

    if (!p) {
        perror("malloc");
        exit(1);
    }

    printf("Adding client %s\n", inet_ntoa(addr));

    p->fd = fd;
    p->ipaddr = addr;
    p->name[0] = '\0';
    p->in_ptr = p->inbuf;
    p->inbuf[0] = '\0';
    p->next = *top;
    *top = p;
}

/* Removes client from the linked list and closes its socket.
 * Also removes socket descriptor from allset 
 */
void remove_player(struct client **top, int fd) {
    struct client **p;

    for (p = top; *p && (*p)->fd != fd; p = &(*p)->next)
        ;
    // Now, p points to (1) top, or (2) a pointer to another client
    // This avoids a special case for removing the head of the list
    if (*p) {
        struct client *t = (*p)->next;
        printf("Removing client %d %s\n", fd, inet_ntoa((*p)->ipaddr));
        FD_CLR((*p)->fd, &allset);
        close((*p)->fd);
        free(*p);
        *p = t;
    } else {
        fprintf(stderr, "Trying to remove fd %d, but I don't know about it\n",
                 fd);
    }
}

int main(int argc, char **argv) {
    int clientfd, maxfd, nready;
    struct client *p;
    struct sockaddr_in q;
    fd_set rset;
    
    if(argc != 2){
        fprintf(stderr,"Usage: %s <dictionary filename>\n", argv[0]);
        exit(1);
    }
    
    // Create and initialize the game state
    struct game_state game;

    srandom((unsigned int)time(NULL));
    // Set up the file pointer outside of init_game because we want to 
    // just rewind the file when we need to pick a new word
    game.dict.fp = NULL;
    game.dict.size = get_file_length(argv[1]);

    init_game(&game, argv[1]);
    
    // head and has_next_turn also don't change when a subsequent game is
    // started so we initialize them here.
    game.head = NULL;
    game.has_next_turn = NULL;
    
    /* A list of client who have not yet entered their name.  This list is
     * kept separate from the list of active players in the game, because
     * until the new playrs have entered a name, they should not have a turn
     * or receive broadcast messages.  In other words, they can't play until
     * they have a name.
     */
    struct client *new_players = NULL;
    
    struct sockaddr_in *server = init_server_addr(PORT);
    int listenfd = set_up_server_socket(server, MAX_QUEUE);
    
    // initialize allset and add listenfd to the
    // set of file descriptors passed into select
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);
    // maxfd identifies how far into the set to search
    maxfd = listenfd;

	//Handles the SIGPIPE signal
	struct sigaction sa;
	sa.sa_handler = SIG_IGN;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	if(sigaction(SIGPIPE, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}
	
    while (1) {
        // make a copy of the set before we pass it into select
        rset = allset;
        nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
        if (nready == -1) {
            perror("select");
            continue;
        }

        if (FD_ISSET(listenfd, &rset)){
            printf("A new client is connecting\n");
            clientfd = accept_connection(listenfd, &q);

            FD_SET(clientfd, &allset);
            if (clientfd > maxfd) {
                maxfd = clientfd;
            }
            printf("Connection from %s\n", inet_ntoa(q.sin_addr));
            add_player(&new_players, clientfd, q.sin_addr);
            char *greeting = WELCOME_MSG;
            if(write(clientfd, greeting, strlen(greeting)) == -1) {
                fprintf(stderr, "Write to client %s failed\n", inet_ntoa(q.sin_addr));
                remove_player(&new_players, p->fd);
            };
        }
        
        /* Check which other socket descriptors have something ready to read.
         * The reason we iterate over the rset descriptors at the top level and
         * search through the two lists of clients each time is that it is
         * possible that a client will be removed in the middle of one of the
         * operations. This is also why we call break after handling the input.
         * If a client has been removed the loop variables may not longer be 
         * valid.
         */
        int cur_fd;
        for(cur_fd = 0; cur_fd <= maxfd; cur_fd++) {
            if(FD_ISSET(cur_fd, &rset)) {
		// Check if this socket descriptor is an active player
		for(p = game.head; p != NULL; p = p->next) {
			if(cur_fd == p->fd) {
				//A string created to hold messsages of size MAX_BUF
				char *msg = Malloc(sizeof(char) * MAX_BUF);
						
				int return_code = read_input(cur_fd, p);
				//Error on read because the socket has been closed
				if (return_code == 1){
					//Display message
					sprintf(msg, "%s has left the game.\r\n", p->name);	
					broadcast(&(game), msg);
					free(msg);
							
					//Only advance the game if the current player leaves the game
					if (p == game.has_next_turn){
						advance_turn(&game);
					}
					announce_turn(&game);							
					//remove_player(&(game.head), p->fd);
					break;
				}else if (return_code == 2){
					//We have more to read
					break;
				}

				//The entire string entered by the user
				char buf[MAX_BUF];
				//Log
				fprintf(stdout, "[%d] Found newline %s\n", p->fd, p->inbuf);
				strcpy(buf, p->inbuf);
				reset_buffer(p);

				//If it is your turn, we process the input and play the game
				if (p == game.has_next_turn){
					//Holds the index in the alphabet of the guess, based on ascii values
					int guess_index;
					guess_index = (int) buf[0] - 97;

					//Checks the validity of the letter string
					if ((strlen(buf) != 1) || !(buf[0] >= 'a' && buf[0] <= 'z')){
						//Log
						fprintf(stdout, "[%d] Invalid entry, not a valid letter\n", p->fd);

						sprintf(msg, "Invalid entry! Please enter a letter between a-z\r\n");
						if(write(cur_fd, msg, strlen(msg)) == -1) {
							fprintf(stdout, "Disconnect from %s\n", inet_ntoa(q.sin_addr));
							remove_player(&(game.head), p->fd);
						} 
						free(msg);
						break;
					}else{
						if (game.letters_guessed[guess_index] != 0){
							//Log
							fprintf(stdout, "[%d] Invalid entry, letter already guessed\n", p->fd);
									
							sprintf(msg, "Invalid entry! Please enter a letter not yet guessed\r\n");
							if(write(cur_fd, msg, strlen(msg)) == -1) {
								fprintf(stdout, "Disconnect from %s\n", inet_ntoa(q.sin_addr));
								remove_player(&(game.head), p->fd);
							}
							free(msg);
							break;
						}
					}
							
					//If we have reached here, the letter was valid, thus broadcast the user's guess
					sprintf(msg, "%s guesses: %s\r\n", (game.has_next_turn)->name, buf);  		
					broadcast(&game, msg);             
					
					//check if the letter is in the word
					check_letter(&game, buf, guess_index);
							
					//Log
					fprintf(stdout, "It's %s's turn\n", (game.has_next_turn)->name);
							
					//Check if the game has ended because we have ran out of guesses
					if (game.guesses_left == 0){
						//Tell everyone what the word was
						fprintf(stdout, "No more guesses\n");
						sprintf(msg, "No more guesses! \r\nThe word was %s\r\n", game.word);  		
						broadcast(&game, msg);       		
						start_new_game(&game, argv[1]);
					}
							
					//Check if the game has ended because we have guessed the word
					if (!strcmp(game.word, game.guess)){
						announce_winner(&game, p);
						start_new_game(&game, argv[1]);
					}
	
					status_message(msg, &game);  		
					broadcast(&game, msg);            
							
					//Free the msg variable we have been using
					free(msg);
							
					//Prompt the user whose turn it is for a guess, display whose turn for the rest
					announce_turn(&game);
				//This is the case if you enter a letter and its not your turn
				}else{
					fprintf(stdout, "Player %s tried to guess out of turn\n", p->name);
							
					char *msg = Malloc(sizeof(char) * MAX_BUF);
					sprintf(msg, "Its not your turn!\r\n");
					if(write(cur_fd, msg, strlen(msg)) == -1) {
						fprintf(stdout, "Disconnect from %s\n", inet_ntoa(q.sin_addr));
						remove_player(&(game.head), p->fd);
					} 
					free(msg);
				}
			}
		}
		// Check if any new players are entering their names
		for(p = new_players; p != NULL; p = p->next) {
			if(cur_fd == p->fd) {
				int return_code = read_input(cur_fd, p);
						
				//Input was not read properly / completely
				if (return_code == 1){
					//Log
					fprintf(stdout, "Disconnect from %s\n", inet_ntoa(q.sin_addr));
							
					remove_player(&new_players, p->fd);
					break;
				//Break to read more input
				}else if (return_code == 2){
					break;
				}
				fprintf(stdout, "[%d] Found newline %s\n", p->fd, p->inbuf);
				strcpy(p->name, p->inbuf);
				reset_buffer(p);					
						
				//Check if the length of the name is appropriate
				if (strlen(p->name) == 0 || check_if_blank(p->name) == 1){
					//Log
					fprintf(stdout, "[%d] Invalid entry, name is blank\n", p->fd);
							
					p->name[0] = '\0';
					reset_buffer(p);
					char *invalid_message = Malloc(sizeof(char) * MAX_BUF);
					sprintf(invalid_message, "Invalid Name! Please enter a non-empty name! \r\n");
					if(write(cur_fd, invalid_message, strlen(invalid_message)) == -1) {
							fprintf(stdout, "Disconnect from %s\n", inet_ntoa(q.sin_addr));
							remove_player(&new_players, p->fd);
					}
					free(invalid_message);
					break;
				}

				//Check if the name already exists, if it does, break and reset buf
				if (check_name_exists(&(game.head), p->name)){
					fprintf(stdout, "[%d] Invalid entry, name already taken\n", p->fd);
							
					p->name[0] = '\0';
					reset_buffer(p);
					char *invalid_message = Malloc(sizeof(char) * MAX_BUF);
					sprintf(invalid_message, "This name has already been taken! Please enter a different name! \r\n");
					if(write(cur_fd, invalid_message, strlen(invalid_message)) == -1) {
						fprintf(stdout, "Disconnect from %s\n", inet_ntoa(q.sin_addr));
						remove_player(&new_players, p->fd);
					}
					free(invalid_message);
					break;
				}
					
				//If the name is good, add the new name to the game head and change the next value to null
				remove_from_new_players(&new_players, p->fd);
						
				//If there is no player currently playing, set the turn to that first player
				if (game.has_next_turn == NULL || game.head == NULL){
					game.has_next_turn = p;				
				}

				//Adjust the linked list of players
				struct client *temp = game.head;	
				p->next = temp;
				game.head = p;
				

				display_join_messages(&game, p, new_players);
				break;
			} 
            	}
            }
        }
    }
    return 0;
}
