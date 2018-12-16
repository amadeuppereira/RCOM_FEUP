#ifndef _CLIENT_TCP_
#define _CLIENT_TCP_

int openSocket(char *hostname, int port);

int writeTCP(char *msg);

int readTCP();

int closeSocket();

#endif
