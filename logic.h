#include <stddef.h>

#define PACKAGE_DATA_SIZE 1024
#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define TRANSMITTER 1
#define RECEIVER 2
#define ERROR -1
#define BUFFER_MAX_SIZE 255
#define L2 0x08
#define XOR(a, b) a^b
#define ERROR_PROBABILITY_DATA 50     //percentage
#define ERROR_PROBABILITY_HEADER 30   //percentage

#define NUMBER_OF_TRIES 3

#define F 0x7e
#define A1 0x03
#define A2 0x01

#define SET 1
#define SET_C 0x03
#define SET_BCC1 0x00

#define DISC 2
#define DISC_C 0x0b

#define UA 3
#define UA_C 0x07
#define UA_BCC1 A1 ^ UA_C

#define RR0 4
#define RR0_C 0x05

#define RR1 5
#define RR1_C 0x85

#define REJ0 6
#define REJ0_C 0x01

#define REJ1 7
#define REJ1_C 0x81

#define I0 8
#define I0_C 0x00

#define I1 9
#define I1_C 0x40

typedef struct {
    char* msg;
    size_t length;
} Frame;

typedef struct {
    char *port; /*Dispositivo /dev/ttySx, x = 0, 1*/
    int baudRate; /*Velocidade de transmissão*/
    unsigned int timeout; /*Valor do temporizador: 1 s*/
    unsigned int numTransmissions; /*Número de tentativas em caso defalha*/
} LinkLayer;

typedef struct {
    size_t sent;
    size_t received;
    size_t time_outs;
    size_t sentRR;
    size_t receivedRR;
    size_t sentREJ;
    size_t receivedREJ;

    size_t framesTotalTime;
    size_t framesCounter;
    float errorProbability_data;
    float errorProbability_header;
} Statistics;

int setup(char *port);
int llopen(int type);
int llwrite(char *buffer, int length);
int llread(char **buffer);
int llclose();
void copyBuffer(char *dest, char *source, int length);
void printBuffer(char *buff, int finalLength);
void alarm_function();
void printProgressBar(int sizeReceived, int fileSize, size_t packageNumber);
void connectionStatistics();
