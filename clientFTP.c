#include "clientTCP.h"
#include "clientFTP.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define PORT 21
#define USER "USER"
#define PASS "PASS"

int primaryFtpSocket;
int secondaryFtpSocket;

int writeFTP(int ftpSocket, char *cmd, char *arg)
{
	char buffer[256] = "";

	strcat(buffer, cmd);
	strcat(buffer, " ");
	strcat(buffer, arg);
	// write msg to tcp socket
	return writeTcp(ftpSocket, buffer) > 0 ? 0 : -1;
}

int auth(char *user, char *password)
{
	// send user
	if (writeFTP(primaryFtpSocket, USER, user) < 0)
	{
		return -1;
	}

	char *msg = readTcp(primaryFtpSocket);
	printf("%s\n", msg);

	// send password
	if (writeFTP(primaryFtpSocket, PASS, password) < 0)
	{
		return -1;
	}

	msg = readTcp(primaryFtpSocket);
	printf("%s\n", msg);

	free(msg);
	return 0;
}

int downloadFtp(char *filePath)
{
	// write download file command
	if (writeFTP(primaryFtpSocket, "RETR", filePath == NULL ? "" : filePath))
	{
		printf("Error writing to server.");
		return -1;
	}

	char *msg = readTcp(primaryFtpSocket);

	printf("%s\n", msg);

	char *token = "";

	token = strtok(msg, " ");

	int code = strtol(token, NULL, 10);

	switch (code)
	{
	case 550:
		token = strtok(NULL, "");
		free(msg);
		return -1;
	default:
		break;
	}

	free(msg);

	return 0;
}

int parsePasvMsg(char *msg)
{
	char *token = "";
	int firstNumber, secondNumber;

	token = strtok(msg, "(,)");
	int i=  0;
	for (; i < 7; i++)
	{
		token = strtok(NULL, "(,)");

		if (i == 4)
		{
			firstNumber = strtol(token, NULL, 10);
		}

		else if (i == 5)
		{
			secondNumber = strtol(token, NULL, 10);
		}
	}
	return firstNumber * 256 + secondNumber;
}

// TODO: retornar nome do ficheiro
char *getFileName(char *filePath)
{
	char *token = strtok(filePath, "/");
	char *previousToken = token;
	
	/* walk through other tokens */
	while( token != NULL ) {
		previousToken = token;
		token = strtok(NULL, "/");
	}

	return previousToken;
}

int receiveFile(char *filePath)
{
	// receive file
	FILE *f = fopen(getFileName(filePath), "w");

	if (f == NULL)
	{
		return -1;
	}

	char buffer[1024];

	int bytes = recv(secondaryFtpSocket, &buffer, sizeof(buffer), 0);

	// Write strting received to file
	fprintf(f, "%s", buffer);

	fclose(f);

	return 0;
}

int downloadFile(char *ip, char *user, char *password, char *filePath)
{
	// connect to socket
	primaryFtpSocket = openTcpSocket(ip, PORT);
	if (primaryFtpSocket < 0)
	{
		perror("Error opening socket.");
		return -1;
	}

	char *msg = readTcp(primaryFtpSocket);

	printf("Welcoming message:\n%s\n", msg);

	// login to ftp server
	if (auth(user, password))
	{
		perror("Auth error: ");
		return -1;
	}

	// set passive mode
	if (writeFTP(primaryFtpSocket, "PASV", ""))
	{
		perror("Error entering passive mode.");
		return -1;
	}

	msg = readTcp(primaryFtpSocket);
	printf("%s\n", msg);

	// Connect to secondary socket
	int secondaryPort = parsePasvMsg(msg);

	int pchild, status;

	pchild = fork();

	if (pchild == 0)
	{
		// child process
		secondaryFtpSocket = openTcpSocket(ip, secondaryPort);

		if (secondaryFtpSocket < 0)
		{
			perror("Error opening secondary socket.");
			return -1;
		}

		if (receiveFile(filePath))
		{
			printf("Error receiving file.\n");
		}

		printf("Finished child process.\n");
	}
	else if (pchild < 0)
	{
		status = -1;
		free(msg);
		return -1;
	}

	// parent process
	sleep(2);

	// download file
	if (downloadFtp(filePath))
	{
		printf("Error downloading file.");
		free(msg);
		return -1;
	}

	// wait for child process to finish (download process)
	waitpid(pchild, &status, 0);

	free(msg);
	return 0;
}

int closeFTP()
{
	closeTcpSocket(primaryFtpSocket);
	closeTcpSocket(secondaryFtpSocket);
	return 0;
}
