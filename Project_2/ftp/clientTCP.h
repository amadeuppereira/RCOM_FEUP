#ifndef _CLIENT_TCP_
#define _CLIENT_TCP_

/**
 * @brief Open tcp socket.
 * 
 * @param hostname host url (ex: fe.up.pt).
 * @param port port (ex: 21).
 * @return int socket in success, -1 in error.
 */
int openTcpSocket(char *hostname, int port);

int writeTcp(int socket, char *msg);

int readTcp(int socket);

int closeTcpSocket(int socket);

#endif
