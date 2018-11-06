#include "clientTCP.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv){

	if (argc != 2){
		perror("Not enought arguments.\n");
	}

	exit(0);

  /*
	setup();

	char	buf[] = "Mensagem de teste na travessia da pilha TCP/IP\n";
	int	bytes;

	/*send a string to the server
	bytes = write(sockfd, buf, strlen(buf));
	printf("Bytes escritos %d\n", bytes);

	return closeSocket();



	exit(0);*/
}
