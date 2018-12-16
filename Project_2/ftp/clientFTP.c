#include "clientTCP.h"
#include "clientFTP.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 21
#define USER "USER"
#define PASS "PASS"

int writeFTP(char *cmd, char *arg)
{
	char buffer[256] = "";

	strlcat(buffer, cmd, sizeof(buffer));
	strlcat(buffer, " ", sizeof(buffer));
	strlcat(buffer, arg, sizeof(buffer));

	// write msg to tcp socket
	int r = writeTCP(buffer) > 0 ? 0 : -1;

	// error writing message
	if (r)
		return -1;
	else
		return readTCP() > 0 ? 0 : -1;
}

int auth(char *user, char *password)
{
	// send user
	if (writeFTP(USER, user) < 0)
	{
		return -1;
	}

	// send password
	if (writeFTP(PASS, password) < 0)
	{
		return -1;
	}

	return 0;
}

int downloadFile(char *filePath)
{
	// write download file command
	return writeFTP("RETR", filePath == NULL ? "" : filePath);
}

int setup(char *ip, char *user, char *password)
{
	// connect to socket
	if (openSocket(ip, PORT))
	{
		perror("Error opening socket.");
		return -1;
	}

	printf("\n");

	// login to ftp server
	if (auth(user, password))
	{
		perror("Auth error: ");
		return -1;
	}

	// set passive mode
	if (writeFTP("PASV", ""))
	{
		perror("Error entering passive mode.");
		return -1;
	}

	return 0;
}

int closeFTP()
{
	return closeSocket();
}