/*      (C)2000 FEUP  */
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
#include <stdio.h>
#include <errno.h>
#include "clientTCP.h"

char *getIP(char *hostname)
{
	struct hostent *h;

	if ((h = gethostbyname(hostname)) == NULL)
	{
		perror("gethostbyname");
		return NULL;
	}

	printf("\nHost name  : %s\n", h->h_name);
	printf("IP Address : %s\n", inet_ntoa(*((struct in_addr *)h->h_addr)));

	return inet_ntoa(*((struct in_addr *)h->h_addr));
}
char *readTcp(int socket)
{
	char *msg = malloc(sizeof(char));
	int bytes;
	char buffer;
	char line[256] = "";
	int state = 0;
	int count = 0, msgLength = 0;

	/* read server response */
	do
	{
		bytes = recv(socket, &buffer, sizeof(buffer), 0);

		if(bytes == -1){
			printf("Error on reading TCP\n");
			return NULL;
		}
		line[count++] = buffer;

		// copy buffer to final message
		msg = realloc(msg, sizeof(msg) + sizeof(char) * (msgLength + 1));
		msg[msgLength++] = buffer;

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
				if (line[3] == ' ')
				{
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

	return msg;
}

int openTcpSocket(char *hostname, int port)
{
	int tcpSocket = 0;
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
	if ((tcpSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket()");
		return -1;
	}
	else
	{
		printf("TCP socket opened to %s:%d!\n\n", ip, port);
	}

	/*connect to the server*/
	if (connect(tcpSocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		perror("connect()");
		return -1;
	}

	return tcpSocket;
}

int writeTcp(int socket, char *msg)
{
	int bytes;

	strcat(msg, "\r\n");

	/* send a string to the server */
	bytes = send(socket, msg, strlen(msg) * sizeof(char), 0);

	printf("%s", msg);

	/* read response */
	return bytes;
}

int closeTcpSocket(int socket)
{
	return close(socket);
}
