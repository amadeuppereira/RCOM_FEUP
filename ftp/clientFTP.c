#include "clientTCP.h"
#include <stdio.h>
#include <stdlib.h>

int login(char *username, char *password) {
	//writeTCP("USER " + username);
	//writeTCP("PASS " + password);
}

int main(int argc, char** argv){

	if (argc != 3){
		perror("Not enought arguments.\n");
		return -1;
	}

	if (openSocket(argv[1], atoi(argv[2]))){
		perror("Error opening socket.");
		return -1;
	}

	char login[] = "USER";

	
	writeTCP("USER anonymous");
	writeTCP("PASS anonymous");

	return 0;
}
