//receiver -- application layer

#include "logic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
  (strcmp("/dev/ttyS1", argv[1])!=0))) {
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
    char buffer[PACKAGE_DATA_SIZE+4];
    readResult = llread(buffer);

    handleRead(buffer, readResult);

  } while(readResult > 0);



  return 0;
}

void handleStartPkg(char *buffer, int size){
  // esta a receber pacote start, extrai informacao e abre ficheiro em disco em modo de escrita.

  // TODO: extrair nome do ficheiro

  // TODO: extrair tamanho do ficheiro

  file = fopen("file.gif", "wb");

  if (file == NULL){
      printf("Error opening file!\n");
      exit(1);
  }
}

void handleIPkg(char *buffer, int size){
  // percebe que o pacote comtem dados do ficheiro e armazena em disco.);
  printf("\nData package received: \n");
  // TODO: extrair N
  printf("N: %d\n", buffer[1]);
  printf("L1: %d\n", buffer[2]);
  printf("L2: %d\n", buffer[3]);

  // parse L1  e L2
  // int length = 256 * buffer[2] + buffer[3];
  // printf("Tamanho dos dados: %d\n", length);
  // printf("Buffer size: %d\n", size);

  if (file == NULL){
    printf("Error: No file opened.\n");
    exit(1);
  }

  int length = size-4;
  char * temp;
  temp = malloc(length);
  int i;
  for(i = 0; i < length; i++) {
    temp[i] = buffer[i+4];
  }

  printBuffer(temp, length);
  
  write(fileno(file), temp, length);

}

void handleEndPkg(char *buffer, int size){
  // percebe que e o pacote end e termina.
  printf("Finished writing, closing file!\n");

  fclose(file);

  // TODO: llclose se for pra fechar apos 1 transferencia.
}

void handleRead(char *buffer, int size){

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
