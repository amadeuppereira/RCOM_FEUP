/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define SET_F 0x7e
#define XOR(a, b) a^b

volatile int STOP=FALSE;


int main(int argc, char** argv)
{
    int fd, res;
    //int c;
    struct termios oldtio,newtio;
    char buf[255];

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
    leitura do(s) pr�ximo(s) caracter(es)
  */



    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    fflush(NULL);

    printf("New termios structure set\n");
  
    int i = 0;
 /*   while (STOP==FALSE) {       
      res = read(fd,buf+i,1);
      if(res > 0) {
        if (buf[i]=='\0') STOP=TRUE;
        i++;
      }
    }
*/
    int state = 0;

    while(STOP==FALSE) {
      res = read(fd, buf+i, 1);
      switch(state) {
	case 0:  
	  if(res > 0 && *(buf+i) == SET_F) {
            i++;
            state++;
          }
          break;
        case 1:
          if(res > 0 && *(buf+i) != SET_F) {
	    i++;
            state++;
	  } 
	  break;
        case 2:
          if(res > 0 && *(buf+i) != SET_F) {
            i++;
	  }
          else if (res > 0 && *(buf+i) == SET_F) {
	    state++;
	  }
	  break;
	default:
	  STOP = TRUE;
	  break;
      }
    }

    int isValidSet = (buf[3] == (XOR(buf[1], buf[2])));
    printf("Valid SET ? %s\n", isValidSet ? "true" : "false");

    //printf("Received: %s\n", buf);

    
    //writing back to the emissor
    //int size = strlen(buf) + 1;

    //res = write(fd,buf,size);

    fflush(NULL);
    //printf("Sending back...\n");
    //printf("%d bytes written\n", res);

    sleep(1);
    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    return 0;
}
