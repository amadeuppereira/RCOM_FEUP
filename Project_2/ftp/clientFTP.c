#define _POSIX_SOURCE
#include "clientTCP.h"
#include "clientFTP.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>

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
		printf("Error on sending USER.");
		return -1;
	}

	char *msg = readTcp(primaryFtpSocket);
	printf("%s\n", msg);

	if(strncmp(msg, "331 Please specify the password.", 32) != 0){
		printf("Error on authentication: wrong USER\n");
		return -1;
	}

	// send password
	if (writeFTP(primaryFtpSocket, PASS, password) < 0)
	{
		printf("Error on sending PASS.");
		return -1;
	}

	msg = readTcp(primaryFtpSocket);
	printf("%s\n", msg);

	if(strncmp(msg, "230 Login successful.", 21) != 0){
		printf("Error on authentication: wrong PASS\n");
		return -1;
	}

	free(msg);
	return 0;
}

int downloadFTP(char *filePath)
{	
	// write download file command
	if (writeFTP(primaryFtpSocket, "RETR", filePath == NULL ? "" : filePath))
	{
		printf("Error writing to server.\n");
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
	char * fileName = getFileName(filePath);
	FILE *f = fopen(fileName, "w");

	if (f == NULL)
	{
		return -1;
	}

	char buffer[1];
	int bytes;
	int counter = 0;

	while((bytes = read(secondaryFtpSocket, &buffer, sizeof(buffer))) > 0){

		//Write to file
		fwrite(buffer, sizeof(char), sizeof(buffer), f);

		counter += bytes;

		if(bytes == -1){
			printf("Error reading socket.\n");
			return -1;
		}
	}

	if(counter == 0){
		printf("Nothing written in file.\n");
		return -1;
	}
	else{
		printf("File downloaded with success.\n");
		printf("File: %s\n", fileName);
		printf("Size: %d bytes\n", counter);
	}

	fclose(f);

	return 0;
}

int downloadFile(char *ip, char *user, char *password, char *filePath)
{
	// connect to primary socket
	primaryFtpSocket = openTcpSocket(ip, PORT);
	if (primaryFtpSocket < 0)
	{
		printf("Error opening socket.\n");
		return -1;
	}

	char *msg = readTcp(primaryFtpSocket);
	printf("%s\n", msg);

	// login to ftp server
	if (auth(user, password))
	{
		return -1;
	}

	// set passive mode
	if (writeFTP(primaryFtpSocket, "PASV", ""))
	{
		printf("Error entering passive mode.\n");
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
		// Child process
		secondaryFtpSocket = openTcpSocket(ip, secondaryPort);

		if (secondaryFtpSocket < 0)
		{
			printf("Error opening secondary socket.\n");
			return -1;
		}

		if (receiveFile(filePath))
		{
			printf("Error receiving file.\n");
			return -1;
		}

		printf("Finished child process.\n");
	}
	else if (pchild > 0){
		// Parent process

		// download file
		if (downloadFTP(filePath))
		{
			printf("Error downloading file.\n");
			kill(pchild, SIGKILL);
			free(msg);
			return -1;
		}

		// wait for child process to finish (download process)
		waitpid(pchild, &status, 0);
	}
	else
	{
		// Error
		status = -1;
		free(msg);
		return -1;
	}

	free(msg);
	return 0;
}

int closeFTP()
{
	closeTcpSocket(primaryFtpSocket);
	closeTcpSocket(secondaryFtpSocket);
	return 0;
}
