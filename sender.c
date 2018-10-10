#include "logic.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#define PACKAGE_DATA_SIZE 260
#define START_C 0x02
#define START_T_FILESIZE 0x00
#define START_T_FILENAME 0x01


int getFileSize(const char* filename);
void generateStartPackage(const char* filename, int fileSize);

int main(int argc, char** argv){

  // parse arguments
  if ( (argc < 3) ||
       ((strcmp("/dev/ttyS0", argv[1])!=0) &&
        (strcmp("/dev/ttyS1", argv[1])!=0) )) {
    printf("Usage:%s [serial port] [file path]\n", argv[0]);
    exit(-1);
  }

  // setup serial port
  if (setup(argv[1]) == ERROR){
    printf("Error: could not setup serial port %s.\n", argv[1]);
    exit(-1);
  }

  // if(llopen(TRANSMITTER) == ERROR){
  //   printf("Error: could not write to the receiver\n");
  //   exit(-1);
  // }
  // else {
  //     printf("Connection Successful\n");
  // }

  // 1 ler dados do ficheiro
  int fileSize = getFileSize(argv[2]);
  if(fileSize == -1) {
    printf("Error: could not get file size\n");
    exit(-1);
  }
  else if(fileSize == -2) {
    printf("Error: %s is not a file\n", argv[2]);
    exit(-1);
  }
  else {
    printf("\n\tfile: %s (0x%x bytes)\n\n", argv[2], fileSize);
  }
  // FILE *file;
  // file = fopen(argv[2], "r");

  // 2 gerar pacote start
  generateStartPackage(argv[2], fileSize);

  //   // 3 gerar n pacotes com k dados lidos do ficheiro
  //   generateFPackages();

  //   // 4 invocar llwrite(buff) passando em buf: start ou f on end (de D1 a Dn)

  // } else {
  //   printf("Error: could not read file %s\n", argv[2]);
  //   return -1;
  // }

  return 0;
}

int getFileSize(const char* filename) {

  struct stat fileStat;
  
  if (stat(filename, &fileStat) < 0 ){
    return -1;
  }

  if (!S_ISREG(fileStat.st_mode)) {
    return -2;
  }

  return fileStat.st_size;
  
}

void generateStartPackage(const char* filename, const int filesize){
  char* start;
  int i = 0, j, temp;

  int startPackageSize = 1;

  //filesize tlv
  int filesize_s = sizeof(filesize);
  startPackageSize += 2 + filesize_s;

  //filename tlv
  int filename_s = strlen(filename) * sizeof(char);
  startPackageSize += 2 + filename_s;

  start = malloc(startPackageSize); 

  start[i++] = START_C;               //C
  start[i++] = START_T_FILESIZE;      //T
  start[i++] = filesize_s;            //L

   printf("---- 0x%x\n", filesize);

//   start[i++] = (filesize >> 24) & 0xFF; 
//   printf("%x\n", start[i-1]);
//   start[i++] = (filesize >> 16) & 0xFF;
//  printf("%x\n", start[i-1]);
//   start[i++] = (filesize >> 8) & 0xFF;
//   printf("%x\n", start[i-1]);
//   start[i++] = (filesize) & 0xFF;
//   printf("%x\n", start[i-1]);
  
  for(j = 0; j < filesize_s; j++) {
    i+=j;
    start[i] = (filesize >> (8*(filesize_s - j - 1))) & 0xFF; //V
    printf("0x%x - %d\n", start[i], (filesize_s - j - 1));
  }

  start[i++] = START_T_FILENAME; //T
  start[i++] = filename_s;

  temp = i;
  for(; i < filename_s + temp; i++) {
    start[i] = *(filename + i - temp);
  }

  printf("\n");


  free(start);
}
