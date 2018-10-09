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
#define F 0x7e
#define UA_A 0x03
#define UA_C 0x07
#define XOR(a, b) a^b
#define UA_BCC1 XOR(UA_A, UA_C)

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
    int state = 0;
    int isValidSet;

    char ua[5] = {F , UA_A , UA_C , UA_BCC1 , F};

    while(STOP==FALSE) {
      res = read(fd, buf+i, 1);
      switch(state) {
	      case 0:  
	         if(res > 0 && *(buf+i) == F) {
              i++;
              state++;
          }
          break;
        case 1:
           if(res > 0 && *(buf+i) != F) {
	          i++;
            state++;
	        } 
	        break;
        case 2:
          if(res > 0 && *(buf+i) != F) {
            i++;
	        }
          else if (res > 0 && *(buf+i) == F) {
	          state++;
	        }                                          
	        break;
      	default:
             isValidSet = (buf[3] == (XOR(buf[1], buf[2])));
             printf("Valid SET ? %s\n", isValidSet ? "true" : "false");
             if(isValidSet){
	              STOP = TRUE;
                res = write(fd,ua,sizeof(char)*5);
                printf("%d bytes written\n", res);
             }
             else{
               i = 0;
               state = 0;
             }
	      break;
      }
    }

 



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
