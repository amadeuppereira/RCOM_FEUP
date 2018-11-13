#include "clientTCP.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv){

	if (argc != 3){
		perror("Not enought arguments.\n");
		return -1;
	}

	if (openSocket(argv[1], atoi(argv[2]))){
		perror("Error opening socket.");
		return -1;
	} else {
		// read server response
		readTCP();
	}

	// write username
	writeTCP("USER anonymous");
	// read response
	readTCP();
	
	// write password
	writeTCP("PASS anonymous");
	// read response
	readTCP();

	return 0;
}
