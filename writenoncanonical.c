/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define SET_F 0x7e
int flag = 0;
int conta = 0;

volatile int STOP=FALSE;



void atende()                   // atende alarme
{
	printf("alarme # %d\n", conta);
	flag=1;
	conta++;
}

int main(int argc, char** argv)
{
    int fd, res;
    (void) signal(SIGALRM, atende);  // instala  rotina que atende interrupcao
    //int c;
    struct termios oldtio,newtio;
    char ua[5];
    //int i, sum = 0, speed = 0;

    if ( (argc < 2) ||
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) &&
  	      (strcmp("/dev/ttyS1", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }


  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */


    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd <0) {perror(argv[1]); exit(-1); }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 1;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 0;   /* blocking read until 5 chars received */



  /*
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
    leitura do(s) pr�ximo(  printf(":%s:%d\n", buf, res);s) caracter(es)
  */



    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }


  //  printf("Vou terminar.\n");

    printf("New termios structure set\n");

    printf("Message: ");

  //  char* temp = gets(ua);
  /*  if(temp == NULL) {
      perror("gets");
      exit(-1);
    }
    */

    //int size = strlen(buf) + 1;

    char set[5] = {SET_F , 0x03 , 0x03 , 0x00 , SET_F};

    int i;

    while (STOP==FALSE && conta < 3) {       /* lobreak;op for input */
      i = 0;
      res = write(fd,set,sizeof(char) * 5);

      fflush(NULL);
      printf("%d bytes written\n", res);
      alarm(3);
      flag = 0;
      while(flag == 0 && STOP==FALSE){
        res = read(fd,ua+i,1);
        if (ua[0] == SET_F){
          i++;
        }
        else if(i > 0 && ua[i]!=SET_F){
          i++;
        }
       else if(i == 4 && ua[i]==SET_F) {
          STOP = TRUE;
        }

      }



    }
    printf("Received: %s\n", ua);


  /*
    O ciclo FOR e as instru��es seguintes devem ser alterados de modo a respeitar
    o indicado no gui�o
  */

    sleep(1);
    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }




    close(fd);
    return 0;
}
