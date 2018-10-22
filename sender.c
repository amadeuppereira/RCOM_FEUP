#include "logic.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <signal.h>


#define PACKAGE_DATA_SIZE 260
#define START_C 0x02
#define START_T_FILESIZE 0x00
#define START_T_FILENAME 0x01
#define PACKAGE_C 0x01
#define END_C 0x03
#define END_T_FILESIZE 0x00
#define END_T_FILENAME 0x01
#define RIGHT_SHIFT_CALC(size, index) 8*(size - index - 1)

int fd;

size_t getFileSize(const char* filename);
int sendStartPackage(const char* filename, size_t fileSize);
int generateStartPackage(const char* filename, size_t fileSize, char** startPackage);
int sendFPackages(const char* filename);
int generateFPackages(char * filedata, char ** fPackage, int packageCounter, int size_package);
int sendEndPackage(const char* filename, size_t fileSize);
int generateEndPackage(const char* filename, size_t fileSize, char** endPackage);

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
    printf("Error: could not connect to the receiver\n");
    exit(-1);
  }
  else {
      printf("Connection Successful\n");
  }

  // //2 gerar pacote start
  // if(sendStartPackage(argv[2], fileSize) == ERROR){
  //   printf("Error: could not send Start package\n");
  //   return ERROR;
  // }

  // // 3 gerar n pacotes com k dados lidos do ficheiro
  // if(sendFPackages(argv[2]) == ERROR){
  //   printf("Error: could not send F packages\n");
  //   return ERROR;
  // }

  // // 4 gerar pacote start
  // if(sendEndPackage(argv[2], fileSize) == ERROR){
  //   printf("Error: could not send End package\n");
  //   return ERROR;
  // }

  return 0;
}

// size_t getFileSize(const char* filename){

//   struct stat fileStat;

//   if (stat(filename, &fileStat) < 0 ){
//     return -1;
//   }

//   if (!S_ISREG(fileStat.st_mode)) {
//     return -2;
//   }

//   return fileStat.st_size;

// }

// int sendStartPackage(const char* filename, size_t fileSize) {
//     char* startPackage = NULL;
//     int startPackageSize = generateStartPackage(filename, fileSize, &startPackage);

//     // printf("handler: %d\n", startPackageSize);
//     //   int w;
//     // for(w = 0; w < startPackageSize; w++) {
//     //   printf("0x%x (%c) | ", (unsigned char)startPackage[w], (unsigned char)startPackage[w]);
//     // }
//     // printf("\n");
//     // printf("Start package size: %d\n", startPackageSize);

//     int ret = llwrite(startPackage, startPackageSize);

//     free(startPackage);
//     return ret;
// }

// int generateStartPackage(const char* filename, const size_t filesize, char** start){
//   char* temp;
//   int i = 0, j;
//   int startPackageSize = 1;

//   //filesize tlv
//   int filesize_s = sizeof(filesize);
//   startPackageSize += 2 + filesize_s;

//   //filename tlv
//   int filename_s = strlen(filename) * sizeof(char);
//   startPackageSize += 2 + filename_s;

//   temp = malloc(startPackageSize);
//   temp[i++] = START_C;               //C
//   temp[i++] = START_T_FILESIZE;      //T
//   temp[i++] = filesize_s;            //L

//   for(j = 0; j < filesize_s; j++, i++) {
//     temp[i] = (filesize >> RIGHT_SHIFT_CALC(filesize_s, j)) & 0xFF; //V
//   }

//   temp[i++] = START_T_FILENAME;  //T
//   temp[i++] = filename_s;        //L

//   for(j = 0; j < filename_s; j++, i++) {
//     temp[i] = *(filename + j);   //V
//   }

//   *start = temp;
//   return startPackageSize;
// }

// int sendFPackages(const char* filename){
//   FILE *file;
//   char str[PACKAGE_DATA_SIZE];
//   char * fPackage = NULL;
//   int counter = 0;

//   file = fopen(filename, "rb");
//   if(file == NULL)
//     return ERROR;

//   long filelength;
//   fseek(file,0,SEEK_END);
//   filelength = ftell(file);
//   rewind(file);
//   printf("file size: %ld\n", filelength);
//   char * buffer;
//   buffer = (char*) malloc((filelength+1)*sizeof(char));
//   fread(buffer, filelength, 1, file);

//   int i = 0, j;
//   int STOP = FALSE;

//   while(STOP == FALSE){
//     for(j = 0; j < PACKAGE_DATA_SIZE; j++, i++){
//       if(i == filelength){
//         STOP = TRUE;
//         break;
//       }
//       str[j] = buffer[i];
//     }
//     printBuffer(str, j);

//     generateFPackages(str, &fPackage, counter, j);

//     if(llwrite(fPackage, j+4) == ERROR){
//       free(fPackage);
//       return ERROR;
//     }

//     counter++;
//     free(fPackage);
//   }

//   fclose(file);
//   return 0;
// }

// int generateFPackages(char * filedata, char ** fPackage, int packageCounter, int size_package){
//   char* temp;
//   int i = 0, j;

//   temp = malloc(size_package + 4);
//   temp[i++] = PACKAGE_C;
//   temp[i++] = packageCounter;
//   temp[i++] = size_package / PACKAGE_DATA_SIZE;
//   temp[i++] = size_package % PACKAGE_DATA_SIZE;

//   for(j = 0; j < size_package; j++, i++) {
//     temp[i] = filedata[j];
//   }

//   *fPackage = temp;
//   return 0;
// }

// int sendEndPackage(const char* filename, size_t fileSize) {
//     char* endPackage = NULL;
//     int endPackageSize = generateEndPackage(filename, fileSize, &endPackage);

//     // printf("handler: %d\n", endPackageSize);
//     //   int w;
//     // for(w = 0; w < endPackageSize; w++) {
//     //   printf("0x%x (%c) | ", (unsigned char)endPackage[w], (unsigned char)endPackage[w]);
//     // }
//     // printf("\n");
//     // printf("End package size: %d\n", endPackageSize);

//     int ret = llwrite(endPackage, endPackageSize);

//     free(endPackage);
//     return ret;
// }

// int generateEndPackage(const char* filename, const size_t filesize, char** end){
//   char* temp;
//   int i = 0, j;
//   int endPackageSize = 1;

//   //filesize tlv
//   int filesize_s = sizeof(filesize);
//   endPackageSize += 2 + filesize_s;

//   //filename tlv
//   int filename_s = strlen(filename) * sizeof(char);
//   endPackageSize += 2 + filename_s;

//   temp = malloc(endPackageSize);
//   temp[i++] = END_C;               //C
//   temp[i++] = END_T_FILESIZE;      //T
//   temp[i++] = filesize_s;            //L

//   for(j = 0; j < filesize_s; j++, i++) {
//     temp[i] = (filesize >> RIGHT_SHIFT_CALC(filesize_s, j)) & 0xFF; //V
//   }

//   temp[i++] = END_T_FILENAME;  //T
//   temp[i++] = filename_s;        //L

//   for(j = 0; j < filename_s; j++, i++) {
//     temp[i] = *(filename + j);   //V
//   }

//   *end = temp;
//   return endPackageSize;
// }
