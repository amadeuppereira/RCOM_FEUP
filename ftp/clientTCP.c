/*      (C)2000 FEUP  */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <strings.h>
#include <string.h>

#define CR 0x0d
#define LF 0x0a

int sockfd;

char *getIP(char *hostname)
{
	struct hostent *h;

	if ((h = gethostbyname(hostname)) == NULL)
	{
		herror("gethostbyname");
		return NULL;
	}

	printf("Host name  : %s\n", h->h_name);
	printf("IP Address : %s\n", inet_ntoa(*((struct in_addr *)h->h_addr)));

	return inet_ntoa(*((struct in_addr *)h->h_addr));
}

int readTCP()
{
	int bytes;
	char buffer;
	char line[256] = "";
	int state = 0;
	int count = 0;

	/* read server response */
	do
	{
		bytes = recv(sockfd, &buffer, sizeof(buffer), 0);
		line[count++] = buffer;
		//printf("%c", buffer);

		switch (state)
		{
		case 0:
			if (buffer == '\r')
			{
				state = 1;
			}
			break;
		case 1:
			if (buffer == '\n')
			{
				printf("%s", line);

				if (line[3] == ' ')
				{
					printf("\t->reached state 2");
					state = 2;
				}
				else
				{
					state = 0;
					strcpy(line, "");
					count = 0;
				}
			}
			else
			{
				state = 0;
			}
			break;
		default:
			break;
		}

	} while (state != 2);

	printf("\t->Finished reading message.\n");

	return bytes;
}

int openSocket(char *hostname, int port)
{
	struct sockaddr_in server_addr;

	// get ip from hostname
	char *ip = getIP(hostname);

	if (ip == NULL)
	{
		return -1;
	}

	/*server address handling*/
	bzero((char *)&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip); /*32 bit Internet address network byte ordered*/
	server_addr.sin_port = htons(port);			 /*server TCP port must be network byte ordered */

	/*open an TCP socket*/
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket()");
		return -1;
	}
	else
	{
		printf("TCP socket opened to %s:%d!\n", ip, port);
	}

	/*connect to the server*/
	if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		perror("connect()");
		return -1;
	}
	else
	{
		return readTCP() > 0 ? 0 : 1;
	}
}

int writeTCP(char *msg)
{
	int bytes;

	strcat(msg, "\r\n");

	/* send a string to the server */
	bytes = send(sockfd, msg, strlen(msg) * sizeof(char), 0);

	printf("%s", msg);

	/* read response */
	return bytes;
}

int closeSocket()
{
	return close(sockfd);
}
