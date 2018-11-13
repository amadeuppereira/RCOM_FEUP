#ifndef _CLIENT_TCP_
#define _CLIENT_TCP_

int openSocket(char *ip, int port);

int writeTCP(char *msg);

int closeSocket();

#endif

