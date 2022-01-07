// int nbytes = read(cur_fd, p->in_ptr, MAX_NAME);
						////Error on read because the socket has been closed
						// if (nbytes == 0){
							// fprintf(stderr, "Read from client %s failed\n", inet_ntoa(q.sin_addr));
							// remove_player(&new_players, p->fd);
							// break;
						// }
						
						// p->in_ptr += nbytes;
						// *(p->in_ptr) = '\0';
						
						////Checks if the buffer contains the network newline
						// char *checker = strstr(p->inbuf, "\r\n");
						
						////Completes the string if we have read the network newline
						// if (checker != NULL){
							// *checker = '\0';
							// char *str = malloc(sizeof(char) * (strlen(p->inbuf)+1));
							// if (str == NULL) {
								// perror("malloc");
								// exit(1);
							// }
							// strcpy(str, p->inbuf);
							// strcpy(p->name, str);
		
							// p->inbuf[0] = '\0';
							// p->in_ptr = p->inbuf;
							
						// }else{
							// break;
						// }
						/*===================END Read=================== */
						
						/*===================Read=================== FOR INPUT */
						/* int nbytes = read(cur_fd, p->in_ptr, MAX_NAME);
						//Error on read because the socket has been closed
						if (nbytes == 0){
							fprintf(stderr, "Read from client %s failed\n", inet_ntoa(q.sin_addr));
							//We advance the turn if the user disconnects midgame
							sprintf(msg, "%s has left the game.\r\n", p->name);	
							broadcast(&(game), msg);
							free(msg);
							
							//Only advance the game if the current player leaves the game
							if (p == game.has_next_turn){
								advance_turn(&game);
							}
							
							announce_turn(&game);
							remove_player(&(game.head), p->fd);
						}
						
						char *buf;
						
						p->in_ptr += nbytes;
						*(p->in_ptr) = '\0';
						
						//Checks if the buffer contains the network newline
						char *checker = strstr(p->inbuf, "\r\n");
						
						//Completes the string if we have read the network newline
						if (checker != NULL){
							*checker = '\0';
							char *str = malloc(sizeof(char) * (strlen(p->inbuf)+1));
							if (str == NULL) {
								perror("malloc");
								exit(1);
							}
							fprintf(stdout, "%s\n", p->inbuf);
							strcpy(str, p->inbuf);
							buf = str;
							reset_buffer(p);
						//Re-prompts otherwise	
						}else{
							break;
						} */
						// /*===================END Read=================== */
						
						//Goes at the end of the read name
						
						/* //Display to everyone who has just joined the game
						char *msg = malloc(sizeof(char) * MAX_BUF);
						msg = malloc(sizeof(char) * MAX_BUF);
						sprintf(msg, "%s has just joined\r\n", game.head->name);		
						broadcast(&game, msg);
						free(msg);
				
						//Player's turn
						if (game.has_next_turn == p){
							//Broadcast the game status
							msg = malloc(sizeof(char) * MAX_BUF);
							status_message(msg, &game);  		
							broadcast(&game, msg);         
							free(msg);	
						//Not players turn
						}else{
							//Display game status
							msg = malloc(sizeof(char) * MAX_BUF);
							status_message(msg, &game);  
							if(write(cur_fd, msg, strlen(msg)) == -1) {
								fprintf(stderr, "Write to client %s failed\n", inet_ntoa(q.sin_addr));
								remove_player(&new_players, p->fd);
							} 							
							free(msg);
						}
						//Announce the turn and prompt the appropriate person for the prompt
						announce_turn(&game); */
						
						
								/* //Reset the game
								game.dict.fp = NULL;
								game.dict.size = get_file_length(argv[1]);
								init_game(&game, argv[1]);
								
								//Tell everyone a new game has started
								msg = malloc(sizeof(char) * MAX_BUF);
								sprintf(msg, "\r\nLets start a new game!\r\n");  		
								broadcast(&game, msg);             
								free(msg); */