#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define TRANSMITTER 1
#define RECEIVER 2
#define ERROR -1
#define BUFFER_MAX_SIZE 255
#define F 0x7e
#define UA_A 0x03
#define UA_C 0x07
#define XOR(a, b) a^b
#define UA_BCC1 XOR(UA_A, UA_C)

int fd, res;
char buf[255];

struct termios oldtio, newtio;

void alarm_function();
int setup(char *port);
int llopen_Receiver();
int llopen_Sender();
int llopen(int type);