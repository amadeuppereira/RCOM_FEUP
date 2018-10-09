//receiver -- application layer

#include "logic.h"
//states
#define START 0
#define PACKAGE 1
#define END 2
//----

int state;

void receiver(int fd);

int main(int argc, char** argv) {
    if ( (argc < 2) ||
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) &&
  	      (strcmp("/dev/ttyS1", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }

    if(setup(argv[1]) == ERROR) {
        printf("Error: Could not setup serial port.\n");
        exit(1);
    }

    if(llopen(RECEIVER) == ERROR){
        printf("Unable to connect to the sender!\n");
        exit(-1);
    }
    else {
        printf("Connection Successful\n");
    }

    // receiver(fd);
    return 0;
}

// void receiver(int fd) {
//     char buf[BUFFER_MAX_SIZE];
//     state = START;

// }
