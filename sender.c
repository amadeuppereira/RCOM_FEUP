#include "logic.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <signal.h>

#define START_C 0x02
#define START_T_FILESIZE 0x00
#define START_T_FILENAME 0x01
#define PACKAGE_C 0x01
#define END_C 0x03
#define END_T_FILESIZE 0x00
#define END_T_FILENAME 0x01
#define RIGHT_SHIFT_CALC(size, index) 8*(size - index - 1)

size_t getFileSize(const char* filename);
int sendStartPackage(const char* filename, size_t fileSize);
int generateStartPackage(const char* filename, size_t fileSize, char** startPackage);
int sendFPackages(const char* filename);
int generateFPackages(char * filedata, char ** fPackage, int packageCounter, int size_package);
int sendEndPackage(const char* filename, size_t fileSize);
int generateEndPackage(const char* filename, size_t fileSize, char** endPackage);

int fd;
size_t fileSize;
size_t packageCounter = 0;
int sizeSend = 0;

int main(int argc, char** argv){

  // parse arguments
  if ((argc < 3) ||
      ((strcmp("/dev/ttyS0", argv[1])!=0) &&
      (strcmp("/dev/ttyS1", argv[1])!=0))) {
    printf("Usage:%s [serial port] [file path]\n", argv[0]);
    exit(-1);
  }

  struct sigaction action;
	action.sa_handler = alarm_function;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;
	sigaction(SIGALRM, &action, NULL);

  // 1 ler dados do ficheiro
  fileSize = getFileSize(argv[2]);
  if(fileSize == -1) {
    printf("Error: could not get file size\n");
    exit(-1);
  }
  else if(fileSize == -2) {
    printf("Error: %s is not a file\n", argv[2]);
    exit(-1);
  }
  else {
    printf("\nFile: %s (%ld bytes)\n", argv[2], fileSize);
  }

  // setup serial port
  if ((fd = setup(argv[1])) == ERROR){
    printf("Error: could not setup serial port %s.\n", argv[1]);
    exit(-1);
  }

  if(llopen(TRANSMITTER) == ERROR){
    printf("Error: could not connect to the receiver\n");
    exit(-1);
  }
  else {
      printf("*** Connection Successful ***\n\n");
  }

  //2 gerar pacote start
  if(sendStartPackage(argv[2], fileSize) == ERROR){
    printf("\nError: could not send Start package\n");
    return ERROR;
  }

  // 3 gerar n pacotes com k dados lidos do ficheiro
  if(sendFPackages(argv[2]) == ERROR){
    printf("\nError: could not send F packages\n");
    return ERROR;
  }

  // 4 gerar pacote start
  if(sendEndPackage(argv[2], fileSize) == ERROR){
    printf("\nError: could not send End package\n");
    return ERROR;
  }

  if(llclose() == ERROR) {
    printf("\nError: could not disconnect\n");
    return ERROR;
  }
  else{
    printf("*** Disconnection Successful ***\n\n");
  }

  printf("File sent with success\n");

  connectionStatistics();
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

int sendStartPackage(const char* filename, size_t fileSize) {
    char* startPackage = NULL;
    int startPackageSize = generateStartPackage(filename, fileSize, &startPackage);

    int ret;
    do{
      ret = llwrite(startPackage, startPackageSize);
    }while(ret == ERROR);

    free(startPackage);
    return ret;
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

  memcpy(temp+i, &filesize, filesize_s);
  i = i + filesize_s;

  temp[i++] = START_T_FILENAME;  //T
  temp[i++] = filename_s;        //L

  for(j = 0; j < filename_s; j++, i++) {
    temp[i] = *(filename + j);   //V
  }

  *start = temp;
  return startPackageSize;
}

int sendFPackages(const char* filename){
  FILE *file;
  char str[PACKAGE_DATA_SIZE];
  char * fPackage = NULL;
  int counter = 0;

  file = fopen(filename, "rb");
  if(file == NULL)
    return ERROR;

  char * buffer;
  buffer = (char*) malloc((fileSize+1)*sizeof(char));
  fread(buffer, fileSize, 1, file);

  int i = 0, j;
  int STOP = FALSE;

  while(STOP == FALSE){
    for(j = 0; j < PACKAGE_DATA_SIZE; j++, i++){
      if(i == fileSize){
        STOP = TRUE;
        break;
      }
      str[j] = buffer[i];
    }

    generateFPackages(str, &fPackage, counter, j);

    sizeSend += j;
    packageCounter++;

    printProgressBar(sizeSend, fileSize, packageCounter);

    int ret;
    ret = llwrite(fPackage, j+4);
    free(fPackage);
    if(ret == ERROR) return ERROR;

    counter++;
  }

  fclose(file);
  return 0;
}

int generateFPackages(char * filedata, char ** fPackage, int packageCounter, int size_package){
  char* temp;
  int i = 0, j;

  temp = malloc(size_package + 4);
  temp[i++] = PACKAGE_C;
  temp[i++] = packageCounter % 255;
  temp[i++] = size_package / 256;
  temp[i++] = size_package % 256;

  for(j = 0; j < size_package; j++, i++) {
    temp[i] = filedata[j];
  }

  *fPackage = temp;
  return 0;
}

int sendEndPackage(const char* filename, size_t fileSize) {
    char* endPackage = NULL;
    int endPackageSize = generateEndPackage(filename, fileSize, &endPackage);

    int ret;
    do{
      ret = llwrite(endPackage, endPackageSize);
    }while(ret == ERROR);

    free(endPackage);
    return ret;
}

int generateEndPackage(const char* filename, const size_t filesize, char** end){
  char* temp;
  int i = 0, j;
  int endPackageSize = 1;

  //filesize tlv
  int filesize_s = sizeof(filesize);
  endPackageSize += 2 + filesize_s;

  //filename tlv
  int filename_s = strlen(filename) * sizeof(char);
  endPackageSize += 2 + filename_s;

  temp = malloc(endPackageSize);
  temp[i++] = END_C;               //C
  temp[i++] = END_T_FILESIZE;      //T
  temp[i++] = filesize_s;            //L

  memcpy(temp+i, &filesize, filesize_s);
  i = i + filesize_s;

  temp[i++] = END_T_FILENAME;  //T
  temp[i++] = filename_s;        //L

  for(j = 0; j < filename_s; j++, i++) {
    temp[i] = *(filename + j);   //V
  }

  *end = temp;
  return endPackageSize;
}
