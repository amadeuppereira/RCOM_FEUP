#include "clientTCP.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 22


int writeFTP(char *cmd, char *arg){
	int size = strlen(cmd) + strlen(arg) + 1;

	// alloc space for msg
	char *msg = malloc(size);

	strcpy(msg, cmd);
	strcpy(msg, " ");
	strcpy(msg, arg);

	// write full msg to tcp socket
	int r = writeTCP(msg);

	// free msg space
	free(msg);

	return r;
}

int auth(char *user, char* password){
	// read server msg
	readTCP();

	// write username
	writeFTP("USER", user);

	// read response
	readTCP();
	
	// write password
	writeFTP("PASS", password);

	// read response
	readTCP();

	return 0;
}

int downloadFile(char *filePath){
	// write download file command
	writeFTP("GET", filePath);

	// read response
	readTCP();

	return 0;
}

void removeSubstr (char *string, char *sub) {
    char *match;
    int len = strlen(sub);
    while ((match = strstr(string, sub))) {
        *match = '\0';
        strcat(string, match+len);
    }
}

int main(int argc, char** argv){
	char *urlPath, *ip, *user, *password, *pch, *url;

	if (argc != 2){
		printf("Not enought arguments.\n");
		printf("Usage: ftp://[<user>:<password>@]<host>/<url-path>\n");
		return -1;
	}

	// remove ftp tag from full argument
	url = malloc(strlen(argv[1]) - 5);
	strncpy(url, &(argv[1][6]), strlen(argv[1]) - 5);

	// parse info from url argument
	pch = strtok(url, "/:@");
	int count = 0;

	while (pch != NULL){
		int size = sizeof(char) * strlen(pch);

		// user
		if (count == 0){
			user = malloc(size);
			strcpy(user, pch);
		}

		// password
		else if (count == 1){
			password = malloc(size);
			strcpy(password, pch);
		}

		// ip
		else if (count == 2){
			ip = malloc(size);
			strcpy(ip, pch);
		}

		// file path
		else if (count == 3){
			urlPath = malloc(size);
			strcpy(urlPath, pch);
		}

		pch = strtok (NULL, "/:@");
		count++;
  	}

	printf("------------------------------------\n");
	printf("Connecting to ftp://%s ...\n", ip);
	printf("User: %s, pass: %s.\n", user, password);
	printf("File path: %s.\n", urlPath);
	printf("------------------------------------\n");

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

	// TODO: download file

	// free alloc vars
	free(user);
	free(password);
	free(ip);
	free(urlPath);
	free(url);

	return 0;
}
