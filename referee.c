#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>			// wait
#include <sys/types.h>			// sockets
#include <sys/socket.h>			// sockets
#include <netinet/in.h>			// Internet sockets
#include <netdb.h>				// gethostbyname
#include <arpa/inet.h>			// inet_ntoa

#define READ		0
#define WRITE		1
#define PORT		5000
#define BUF_SIZE 	500

void doprocessing(int newsocket);
void readstr(int fd, char *buffer, size_t len);
int writestr(int fd, char *message);
int thewinneris(int *score1, int *score2, char *p1, char *p2);

const char hostname[] = "localhost";

int main(int argc, char *argv[])
{
	int i = 0, j = 0, n = 0, sockfd = 0, newsock1 = 0, newsock2 = 0,
		 player1_pid = 0, player2_pid = 0;
	int score1 = 0, score2 = 0, thewinner = 0;
	char buf[BUF_SIZE], *play[2];
	char *message[] = { "ROCK", "PAPER", "SCISSORS" };			  // prs options

	unsigned int serverlen, clientlen1, clientlen2;
	struct sockaddr_in serv_addr, client1, client2;
	struct sockaddr *serverptr, *clientptr1, *clientptr2;
	struct hostent *he;

	n = atoi(argv[1]);										  // number of turns
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)		// create socket
	{							
		perror("FATAL: referee socket");
		exit(-1);
	}
	if ((he = gethostbyname(hostname)) == NULL)
	{
		perror("\n ERROR: Could not get host by name \n");
		exit(-1);
	}

	memset(&serv_addr, '\0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;	// Internet domain
	serv_addr.sin_addr = *((struct in_addr *)he->h_addr);
	serv_addr.sin_port = htons(PORT);	// bind port
	serverptr = (struct sockaddr *)&serv_addr;
	serverlen = sizeof(serv_addr);

	if (bind(sockfd, serverptr, serverlen) < 0)		   // bind socket to address
	{
		perror("FATAL: referee bind");
		exit(-1);
	}
	if (listen(sockfd, 10) == 0)
	{
		printf("Written by: Pete Akey\n");
		printf("Paper, Scissors, Rock: %d iterations\n", n);
	}
	else
	{
		perror("FATAL: referee listen");
	}
							// up to this point, the server is bound and waiting
	clientptr1 = (struct sockaddr *)&client1;
	clientlen1 = sizeof(&client1);
	clientptr2 = (struct sockaddr *)&client2;
	clientlen2 = sizeof(&client2);

	// accept the connection
	if ((newsock1 =
		 accept(sockfd, (struct sockaddr *)&client1, &clientlen1)) < 0)
	{
		perror("Player1 accept");
		exit(-1);
	}
	else
	{
		// fprintf(stderr, "Connection accepted from %s:%d\n",
		// inet_ntoa(client1.sin_addr), ntohs(client1.sin_port));
	}
	if ((newsock2 =
		 accept(sockfd, (struct sockaddr *)&client1, &clientlen1)) < 0)
	{
		perror("Player2 accept");
		exit(-1);
	}
	else
	{
		// fprintf(stderr, "Connection accepted from %s:%d\n",
		// inet_ntoa(client1.sin_addr), ntohs(client1.sin_port));
	}
	for (int k = 1; k <= n; k++)
	{
		bzero(buf, BUF_SIZE);							// initialize the buffer
		j = read(newsock1, buf, BUF_SIZE);
		if (j < 0)
		{
			perror("ERROR reading from socket");
			exit(-1);
		}
		if (strncmp(buf, "READY", 5) == 0)
		{
			if (k == 0)
				fprintf(stderr, "\tPlayer 1: Ready\n");
			j = write(newsock1, "GO\n", 2);
			if (j < 0)
			{
				perror("ERROR writing to socket");
				exit(-1);
			}
		}
		bzero(buf, BUF_SIZE);							// initialize the buffer
		j = read(newsock2, buf, BUF_SIZE);
		if (j < 0)
		{
			perror("ERROR reading from socket");
			exit(-1);
		}
		if (strncmp(buf, "READY", 5) == 0)
		{
			if (k == 0)
				fprintf(stderr, "\tPlayer 2: Ready\n");
			j = write(newsock2, "GO\n", 2);
			if (j < 0)
			{
				perror("ERROR writing to socket");
				exit(-1);
			}
		}
		fprintf(stderr, "Go Players [%d]\n", k);
		bzero(buf, BUF_SIZE);							// initialize the buffer
		j = read(newsock1, buf, BUF_SIZE);
		if (j < 0)
		{
			perror("ERROR reading from socket");
			exit(-1);
		}
		if (strncmp(buf, "ROCK", 4) == 0)
		{
			play[0] = "ROCK";
		}
		else if (strncmp(buf, "PAPER", 5) == 0)
		{
			play[0] = "PAPER";
		}
		else if (strncmp(buf, "SCISSORS", 8) == 0)
		{
			play[0] = "SCISSORS";
		}
		else
		{
			fprintf(stderr, "INFO: message seen is not message I understand\n");
			fprintf(stderr, "player1 buf=%s\n", buf);
			exit(-1);
		}
		fprintf(stderr, "\tPlayer 1: %s\n", play[0]);
		bzero(buf, BUF_SIZE);							// initialize the buffer
		j = read(newsock2, buf, BUF_SIZE);
		if (j < 0)
		{
			perror("ERROR reading from socket");
			exit(-1);
		}
		if (strncmp(buf, "ROCK", 4) == 0)
		{
			play[1] = "ROCK";
		}
		else if (strncmp(buf, "PAPER", 5) == 0)
		{
			play[1] = "PAPER";
		}
		else if (strncmp(buf, "SCISSORS", 8) == 0)
		{
			play[1] = "SCISSORS";
		}
		else
		{
			fprintf(stderr,
					"INFO: message seen is not message I understand\n");
			fprintf(stderr, "player2 buf=%s\n", buf);
			exit(-1);
		}
		fprintf(stderr, "\tPlayer 2: %s\n", play[1]);
		// find out who wins
		thewinner = thewinneris(&score1, &score2, play[0], play[1]);
		if (thewinner != 0)
			fprintf(stderr, "\tPlayer %d Wins\n", thewinner);
		else
			fprintf(stderr, "\tPlayers Draw\n");
	}
	fprintf(stderr, "Final Score:\n");
	fprintf(stderr, "\tPlayer 1: %d\n", score1);
	fprintf(stderr, "\tPlayer 2: %d\n", score2);
	if (score1 > score2)
		fprintf(stderr, "Player 1 Wins\n");
	else if (score2 > score1)
		fprintf(stderr, "Player 2 Wins\n");
	else if (score1 == score2)
		fprintf(stderr, "Players Draw");
	close(newsock1);
	close(newsock2);
	return 0;
}

char readchar(int fd)
{
	char buf;
	if (recv(fd, &buf, 1, 0)) return buf;
	else return '\0';
}

void readstr(int fd, char *buffer, size_t len)
{
	char ch;
	int result;
	size_t offset = 0;
	while (result = read(fd, &buffer, sizeof &buffer))
	{
		fprintf(stderr, "result: %s\n", result);
		if (ch != '\n')
		{
			buffer += ch;
		}
		else
		{
			*buffer = '\0';
			break;
		}
	}
}

int writestr(int fd, char *message)
{								/* Write formated data */
	char temp;
	int length = 0;
	length = strlen(message) + 1;	/* Find length of string */
	temp = length;
	if (write(fd, &temp, 1) < 0)	/* Send length first */
		exit(-2);
	if (write(fd, message, length) < 0)	/* Send string */
		exit(-2);
	return length;				/* Return size of string */
}

int thewinneris(int *score1, int *score2, char *play1, char *play2)
{
	if ((play1 == "ROCK" && play2 == "SCISSORS") ||
		(play1 == "PAPER" && play2 == "ROCK") ||
		(play1 == "SCISSORS" && play2 == "PAPER"))
	{							// 1st player wins
		(*score1)++;
		return 1;
	}
	else if ((play2 == "ROCK" && play1 == "SCISSORS") ||
			 (play2 == "PAPER" && play1 == "ROCK") ||
			 (play2 == "SCISSORS" && play1 == "PAPER"))
	{							// 2nd player wins
		(*score2)++;
		return 2;
	}
	else if (play1 == play2)
		return 0;
}
