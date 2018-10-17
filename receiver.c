//receiver -- application layer

#include "logic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//states
#define START 0
#define PACKAGE 1
#define END 2
//----

int state;
FILE *file;

void handleRead(char *buffer, int size);

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

  int readResult;

  do {
    char buffer[255];
    readResult = llread(buffer);
    //printBuffer(buffer, 30);
    //handleRead(buffer, readResult);

  } while(readResult > 0);



  return 0;
}

void handleStartPkg(char *buffer, int size){
  // esta a receber pacote start, extrai informacao e abre ficheiro em disco em modo de escrita.

  file = fopen("file.txt", "w");

  if (file == NULL){
      printf("Error opening file!\n");
      exit(1);
  }
}

void handleIPkg(char *buffer, int size){
  // percebe que o pacote comtem dados do ficheiro e armazena em disco.

  // TODO: fprintf(file, buffer);
}

void handleEndPkg(char *buffer, int size){
  // percebe que e o pacote end e termina.
  fclose(file);

  // TODO: llclose se for pra fehcar apos 1 transferencia.
}

void handleRead(char *buffer, int size){

  printBuffer(buffer, size);

  //lidar com o c2 do pacote de comando
  char c2 = buffer[0];

  switch(c2){
    case 0x02:
      handleStartPkg(buffer, size);
    break;
    case 0x01:
      handleIPkg(buffer, size);
    break;
    case 0x03:
      handleEndPkg(buffer, size);
    break;
    default:
    break;
  }
}

// void receiver(int fd) {
//     char buf[BUFFER_MAX_SIZE];
//     state = START;

// }
