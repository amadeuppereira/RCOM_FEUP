//receiver -- application layer

#include "logic.h"
#define BUFFER_MAX_SIZE 255
#define FALSE 0
#define TRUE 1
//states
#define START 0
#define PACKAGE 1
#define END 2
//----

int state;
volatile int STOP=FALSE;

void receiver(int fd);

int main(int argc, char** argv) {
    if ( (argc < 2) ||
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) &&
  	      (strcmp("/dev/ttyS1", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }

    int fd;
    if((fd = setup()) == -1) {
        printf("Error: Could not setup serial port.\n");
        exit(1);
    }

    if(llopen(fd, RECEIVER)) printf("Connected!\n");
    else {
        printf("Unable to make connection!\n");
        exit(1);
    }

    receiver(fd);
    return 0;
}

void receiver(int fd) {
    char buf[BUFFER_MAX_SIZE];
    state = START;

}
