#include "logic.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <signal.h>

struct termios oldtio, newtio;

int flag = 0;
int counter = 0;
int fd;
char buf[255];
char C_FLAG = 0x0;

char getCFlag(){
	if (C_FLAG == 0x0){
		C_FLAG = 0x40;
		return 0x0;
	} else {
		C_FLAG = 0x0;
		return 0x40;
	}
}

void alarm_function(){
	printf("Alarm #%d\n", counter);
	flag=1;
	counter++;
}

int setup(char *port) {
  fd = open(port, O_RDWR | O_NOCTTY );
  if (fd <0) {perror(port); return ERROR; }

  if ( tcgetattr(fd,&oldtio) == ERROR) { /* save current port settings */
    perror("tcgetattr");
    return ERROR;
  }

  bzero(&newtio, sizeof(newtio));
  newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;

  /* set input mode (non-canonical, no echo,...) */
  newtio.c_lflag = 0;

  newtio.c_cc[VTIME]    = 1;   /* inter-character timer unused */
  newtio.c_cc[VMIN]     = 0;   /* blocking read until 5 chars received */

  tcflush(fd, TCIOFLUSH);

  if ( tcsetattr(fd,TCSANOW,&newtio) == ERROR) {
    perror("tcsetattr");
    return ERROR;
  }

  printf("New termios structure set\n");
  return fd;
}

int llopen_Receiver(){
  int i = 0;
	int res;
  int state = 0;
  int isValidSet;
	int STOP =  FALSE;
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

int sendMsg(char *msg, int length){
	int i, res;
	int STOP = FALSE;
	int isValidSet;

	(void) signal(SIGALRM, alarm_function);

	while (STOP==FALSE && counter < 3) {
    i = 0;

    res = write(fd, msg, sizeof(char) * length);
    printf("%d bytes written\n", res);
		fflush(NULL);

		alarm(3);
		flag = 0;

    int state = 0;
    while(flag == 0 && STOP==FALSE){
      res = read(fd,buf+i,1);

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
        }
        else{
          i = 0;
          state = 0;
        }
        break;
      }
    }
  }

  if(STOP == TRUE){
		printf("Received: 0x%x\n", buf[2]);
	}
	else
		return ERROR;

  sleep(1);
  if ( tcsetattr(fd,TCSANOW,&oldtio) == ERROR) {
    perror("tcsetattr");
    return ERROR;
  }

  return 0;
}

int llopen_Sender(){
  char set[5] = {F , 0x03 , 0x03 , 0x00 , F};

	return sendMsg(set, 5);
}

int llopen(int type){
  if(type == TRANSMITTER)
  {
    return llopen_Sender();
  }
  else if (type == RECEIVER)
  {
    return llopen_Receiver();
  }

  return ERROR;
}

char *generateBCC2(char *buffer, int length){
	char *buff1 = malloc(sizeof(char) * (length+1));

	// generate bcc2
	char bcc2 = XOR(buffer[1], buffer[2]);

	// copy from buffer to buff1
	strncpy(buff1, buffer, length);

	// add bcc2 to buff1
	buff1[length] = bcc2;

	return buff1;
}

char *stuffing(char *buffer, int length){
	char *buff = malloc(sizeof(char) * 2 * length);
	int i, j = 0;

	for(i = 0; i < length; i++){
		if(buffer[i] == SET_E){
			buff[j] = 0x7d;
			buff[j+1] = 0x5e;
			j++;
		}
		else if (buffer[i] == SET_D){
			buff[j] = 0x7d;
			buff[j+1] = 0x5d;
			j++;
		} else {
			buff[j] = buffer[i];
		}
		j++;
	}

	return buff;
}

char *createMsg(char *buffer, int length){
	char *msg = malloc(length + 5);

	msg[0] = F;
	msg[1] = UA_A;
	msg[2] = getCFlag();

	// add bcc1
	msg[3] = XOR(msg[1], msg[2]);

	// copy buffer
	strncpy(msg, buffer, length);
	msg[length + 4] = F;

	return msg;
}

int llwrite(char *buffer, int length){

	// calculate BCC2
	char *buff1 = generateBCC2(buffer, length);

	// stuffing
	char *buff2 = stuffing(buff1, length+1);
	free(buff1);

	// junta cabecalho e flag inicial
	char *buff3 = createMsg(buff2, 2 * (length+1));
	free(buff2);

	// envia buff3 na porta serie
	while(sendMsg(buff3, 2 * (length+1) + 5) != 0){}

	return 0;
}
