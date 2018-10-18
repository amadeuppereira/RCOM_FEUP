#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define TRANSMITTER 1
#define RECEIVER 2
#define ERROR -1
#define BUFFER_MAX_SIZE 255
#define F 0x7e
#define SETE_D 0x7d
#define SETE_E 0x7e
#define UA_A 0x03
#define UA_C 0x07
#define SET_C 0x03
#define SET_A 0x03
#define L2 0x08
#define XOR(a, b) a^b
#define UA_BCC1 XOR(UA_A, UA_C)
#define RR0 0x05
#define RR1 0x85
#define REJ0 0x01
#define REJ1 0x81
#define PACKAGE_DATA_SIZE 260

int setup(char *port);
int llopen(int type);
int llwrite(char *buffer, int length);
int llread(char *buffer);
int llclose();
void copyBuffer(char *dest, char *source, int length);
void printBuffer(char *buff, int finalLength);
