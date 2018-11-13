/*      (C)2000 FEUP  */
#include <stdio.h>
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

int	sockfd;

int openSocket(char *ip, int port){
	struct	sockaddr_in server_addr;

	/*server address handling*/
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip);	/*32 bit Internet address network byte ordered*/
	server_addr.sin_port = htons(port);		/*server TCP port must be network byte ordered */

	/*open an TCP socket*/
	if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
		perror("socket()");
		return -1;
	} else {
		printf("TCP socket opened to %s:%d!\n", ip, port);
	}

	/*connect to the server*/
	if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
		perror("connect()");
		return -1;
	} else {
		printf("Connected to the server successfully!\n");
		return 0;
	}
}

int readTCP(){
	int bytes;
	char response[516];

	/* read server response */
	bytes = read(sockfd, response, sizeof(char) * 516);
	printf("READ: %s, size: %d\n", response, bytes);

	return bytes;
}

int writeTCP(char *msg){
	int	bytes;

	/* send a string to the server */
	bytes = write(sockfd, msg, strlen(msg));
	printf("WRITE: %s, size: %d\n", msg, bytes);

	return bytes;
}

int closeSocket(){
	return close(sockfd);
}
