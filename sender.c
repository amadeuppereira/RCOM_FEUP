#include "logic.h"

// void generateStartPackage(char[] fileName, int fileSize){
//   char[] start = {0x02, 0x00, L1, fileSize, 0x01, L2};

//   strcmp()



// }


int main(int argc, char** argv){

  // parse arguments
  if ( (argc < 3) ||
       ((strcmp("/dev/ttyS0", argv[1])!=0) &&
        (strcmp("/dev/ttyS1", argv[1])!=0) )) {
    printf("Usage:\t[serial port] [file path]\n");
    exit(-1);
  }

  // setup serial port
  if (setup(argv[1]) == ERROR){
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

  // // 1 ler dados do ficheiro
  // FILE *file;
  // file = open(argv[2], "r");
  // if (file){
  //   // obter dados do ficheiro
  //   struct stat fileStat;
  //   int status;

  //   if (stat(argv[2], &fileStat) < 0 ){
  //     return -1;
  //   }

  //   int fileSize = fileStat.st_size;



  //   // 2 gerar pacote start
  //   generateStartPackages();

  //   // 3 gerar n pacotes com k dados lidos do ficheiro
  //   generateFPackages();

  //   // 4 invocar llwrite(buff) passando em buf: start ou f on end (de D1 a Dn)

  // } else {
  //   printf("Error: could not read file %s\n", argv[2]);
  //   return -1;
  // }

  return 0;
}
