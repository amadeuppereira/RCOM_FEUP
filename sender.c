#include "logic.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#define PACKAGE_DATA_SIZE 260
#define START_C 0x02
#define START_T_FILESIZE 0x00
#define START_T_FILENAME 0x01
#define RIGHT_SHIFT_CALC(size, index) 8*(size - index - 1)

int fd;

size_t getFileSize(const char* filename);
void startPackageHandler(const char* filename, size_t fileSize);
int generateStartPackage(const char* filename, size_t fileSize, char** startPackage);

int main(int argc, char** argv){

  // parse arguments
  if ((argc < 3) ||
      ((strcmp("/dev/ttyS0", argv[1])!=0) &&
      (strcmp("/dev/ttyS1", argv[1])!=0) )) {
    printf("Usage:%s [serial port] [file path]\n", argv[0]);
    exit(-1);
  }

  // 1 ler dados do ficheiro
  size_t fileSize = getFileSize(argv[2]);
  if(fileSize == -1) {
    printf("Error: could not get file size\n");
    exit(-1);
  }
  else if(fileSize == -2) {
    printf("Error: %s is not a file\n", argv[2]);
    exit(-1);
  }
  else {
    printf("\n\tfile: %s (0x%lx bytes)\n\n", argv[2], fileSize);
  }

  // setup serial port
  if ((fd = setup(argv[1])) == ERROR){
    printf("Error: could not setup serial port %s.\n", argv[1]);
    exit(-1);
  }

  if(llopen(TRANSMITTER) == ERROR){
    printf("Error: could not write to the receiver\n");
    exit(-1);
  }
  else {
      printf("Connection Successful\n");
  }

  // FILE *file;
  // file = fopen(argv[2], "r");

  // 2 gerar pacote start
  startPackageHandler(argv[2], fileSize);

  //   // 3 gerar n pacotes com k dados lidos do ficheiro
  //   generateFPackages();

  //   // 4 invocar llwrite(buff) passando em buf: start ou f on end (de D1 a Dn)

  // } else {
  //   printf("Error: could not read file %s\n", argv[2]);
  //   return -1;
  // }

  return 0;
}

size_t getFileSize(const char* filename){

  struct stat fileStat;

  if (stat(filename, &fileStat) < 0 ){
    return -1;
  }

  if (!S_ISREG(fileStat.st_mode)) {
    return -2;
  }

  return fileStat.st_size;

}

void startPackageHandler(const char* filename, size_t fileSize) {
    char* startPackage = NULL;
    int startPackageSize = generateStartPackage(filename, fileSize, &startPackage);

    // printf("handler: %d\n", startPackageSize);
    //   int w;
    // for(w = 0; w < startPackageSize; w++) {
    //   printf("0x%x (%c) | ", (unsigned char)startPackage[w], (unsigned char)startPackage[w]);
    // }
    // printf("\n");
    printf("Start package size: %d\n", startPackageSize);

    llwrite(startPackage, startPackageSize);

    free(startPackage);
}

int generateStartPackage(const char* filename, const size_t filesize, char** start){
  char* temp;
  int i = 0, j;
  int startPackageSize = 1;

  //filesize tlv
  int filesize_s = sizeof(filesize);
  startPackageSize += 2 + filesize_s;

  //filename tlv
  int filename_s = strlen(filename) * sizeof(char);
  startPackageSize += 2 + filename_s;

  temp = malloc(startPackageSize);
  temp[i++] = START_C;               //C
  temp[i++] = START_T_FILESIZE;      //T
  temp[i++] = filesize_s;            //L

  for(j = 0; j < filesize_s; j++, i++) {
    //temp[i] = (filesize >> (8*(filesize_s - j - 1))) & 0xFF; //V
    temp[i] = (filesize >> RIGHT_SHIFT_CALC(filesize_s, j)) & 0xFF; //V
  }

  temp[i++] = START_T_FILENAME;  //T
  temp[i++] = filename_s;        //L

  for(j = 0; j < filename_s; j++, i++) {
    temp[i] = *(filename + j);   //V
  }

  *start = temp;
  return startPackageSize;
}
