#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <time.h>														// srand

/* Copyright Peter Akey 2019 */
/* player for paper, rock, scissors */
/* sends commands to a socket attached to a referee */

#define PORT 5000
#define BUF_SIZE 500

long random_at_most(long m);

const char hostname[] = "localhost";

int main()
{
	int j = 0, sockfd = 0, newsock = 0, n = 0, choice = 0, option = 0;
	char opt[3], buf[BUF_SIZE];
	char *message[] = { "PAPER\n", "ROCK\n", "SCISSORS\n" };	  // psr options
	struct sockaddr_in serv_addr, serv_send;
	struct hostent *he;
	ssize_t nread;

	unsigned int serverlen;
	time_t t;
	srand((unsigned)time(&t) ^ (getpid() << 16));

	// create a socket
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("FATAL: Could not create socket"); exit(-1);
	}
	if ((he = gethostbyname(hostname)) == NULL)
	{
		perror("FATAL: Could not get host by name"); exit(-1);
	}

	memset(&serv_addr, '\0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr = *((struct in_addr *)he->h_addr);

	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("FATAL: Connect Failed"); exit(-1);
	}

	// enter a loop and continuously offer psr to the referee
	for (;;)
	{
		// send a READY
		char command[] = "READY\n";
		j = write(sockfd, command, sizeof command);
		// wait for a GO
		bzero(buf, BUF_SIZE);							  // zero out the buffer
		j = read(sockfd, buf, BUF_SIZE);
		if (j < 0)
		{
			perror("FATAL: reading from socket"); exit(-1);
		}
		if (strncmp(buf, "GO", 2) == 0)
		{
			choice = random_at_most(13411431);
			j = write(sockfd, message[choice], sizeof(message[choice]));
			if (j < 0)
			{
				perror("FATAL: writing to socket"); exit(-1);
			}
		}
		else if (strncmp(buf, "STOP", 4) == 0)
		{
			printf("INFO: We have been instructed to STOP\n");
			break;
		}
	}
	close(sockfd);
	return 0;
}

// Assumes 0 <= max <= RAND_MAX
// Returns in the closed interval [0, max]
long random_at_most(long max)
{
	unsigned long
	// max <= RAND_MAX < ULONG_MAX, so this is okay.
	num_bins = (unsigned long)max + 1,
	num_rand = (unsigned long)RAND_MAX + 1,
	bin_size = num_rand / num_bins, defect = num_rand % num_bins;
	long x;
	do { x = random(); }
	while (num_rand - defect <= (unsigned long)x);

	// Truncated division is intentional
	return x / bin_size % 3;
}