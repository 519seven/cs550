#include <stdio.h>													  // printf
#include <stdlib.h>														// exit
#include <sys/types.h>													// fork
#include <unistd.h>														// fork
#include <sys/types.h>													// wait
#include <sys/wait.h>													// wait
#include <time.h>														// time

/* Copyright Peter Akey 2019                        						 */
/* play paper, rock, scissors game                  						 */
/* just like the real thing, only better            						 */

int main(int argc, char **argv)
{
	char *number_of_plays;
	int n,status,i;
	pid_t ref_pid, player1_pid, player2_pid;
	// argc value of 0 is the file;  argc value of 1 is the number of plays
	// anything beyond that is extraneous and will be ignored
	if (argc>2) {
		printf("INFO: Number of arguments: %d\n", argc);
		printf("INFO: Additional parameters will be ignored\n"); 
	}
	number_of_plays = argv[1];				     // argv is an array of strings
	if (atoi(number_of_plays) <= 0) {
		perror("FATAL: Number of plays must be positive. Exiting.\n");
		return 1;
	}
	// fork for the referee
	if ((ref_pid = fork()) == 0){
		printf("Forking referee");
		execl("referee", "referee", number_of_plays, (char*)NULL);
		perror("INFO: Referee has been forked");
		perror("execl() failure!\n\n");
	}
	// there seems to be a timing issue
	// the files alone will start and establish a connection
	// but when run all together, they don't
	// let's add a sleep
	sleep(1);
	// fork for player1
	if((player1_pid = fork()) == 0){
		printf("Forking player1");
		execl("player", "player1", (char*)NULL);
	}
	// fork for player2
	if((player1_pid = fork()) == 0){
		printf("Forking player2");
		execl("player", "player2", (char*)NULL);
	}

	for(n=1; n<=3; n++){
		wait(&status);
	}

	return 0;
}
