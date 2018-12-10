#include "clientTCP.h"
#include "clientFTP.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 21

int writeFTP(char *cmd, char *arg){
	int size = strlen(cmd) + strlen(arg);

	// alloc space for msg
	char *msg = malloc(size);

	strcat(msg, cmd);
	strcat(msg, " ");
	strcat(msg, arg);

	// write full msg to tcp socket
	int r = writeTCP(msg) > 0 ? 0 : -1;

	// free msg space
	free(msg);

	// error writing message
	if (r){
		return -1;
	}

	return readTCP() > 0 ? 0 : -1;
}

int auth(char *user, char* password){
	// send user
	if (writeFTP("USER", user) < 0){
		return -1;
	} 

	// send password
	if (writeFTP("PASS", password) < 0){
		return -1;
	}

	return 0;
}

int downloadFile(char *filePath){
	// write download file command
	return writeFTP("RETR", filePath == NULL ? "." : filePath);
}

int setup (char *ip, char *user, char *password) {
	// connect to socket
	if (openSocket(ip, PORT)){
		perror("Error opening socket.");
		return -1;
	} 

	// login to ftp server
	if (auth(user, password)){
		perror("Auth error: ");
		return -1;
	}

	return 0;
}

int closeFTP () {
	return closeSocket();
}