#include "clientTCP.h"
#include "clientFTP.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 21
#define USER "USER"
#define PASS "PASS"

int primaryFtpSocket;
int secondaryFtpSocket;

int writeFTP(int ftpSocket, char *cmd, char *arg)
{
	char buffer[256] = "";

	strlcat(buffer, cmd, sizeof(buffer));
	strlcat(buffer, " ", sizeof(buffer));
	strlcat(buffer, arg, sizeof(buffer));

	// write msg to tcp socket
	int r = writeTcp(ftpSocket, buffer) > 0 ? 0 : -1;

	// error writing message
	if (r)
		return -1;
	else
	{
		char *msg = readTcp(ftpSocket);
		printf("%s\n", msg);
	}
}

int auth(char *user, char *password)
{
	// send user
	if (writeFTP(primaryFtpSocket, USER, user) < 0)
	{
		return -1;
	}

	// send password
	if (writeFTP(primaryFtpSocket, PASS, password) < 0)
	{
		return -1;
	}

	return 0;
}

int downloadFile(char *filePath)
{
	// write download file command
	return writeFTP(primaryFtpSocket, "RETR", filePath == NULL ? "" : filePath);
}

int setup(char *ip, char *user, char *password)
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

	// TODO: connect to secondary socket

	return 0;
}

int closeFTP()
{
	closeTcpSocket(primaryFtpSocket);
	closeTcpSocket(secondaryFtpSocket);
	return 0;
}