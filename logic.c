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

void printBuffer(char *buff, int finalLength){
	int i;
	for(i = 0; i < finalLength; i++){
		printf("%d: 0x%x ", i, buff[i]);
	}

	printf("\n\n");
}

int readFrame(Frame* f){

	char* buf = malloc(sizeof(char) * 10);
	int bufferSize = 10;
	int res, i = 0;
	int state = 0;

	while(flag == 0){
		if(i == bufferSize) {
			bufferSize *= 2;
			buf = realloc(buf, sizeof(char) * bufferSize);
		}

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
				i++;
			}
			break;
		default:
			f->msg = buf;
			f->length = i;
			return 0;
		}
	}

	return ERROR;
}

// int rejectMsg(char cflag){
// 	// rejeitar mensagem
// 		char c;
// 		if (cflag == 0x40){
// 			c = REJ1;
// 		}
// 		else if (cflag == 0x00){
// 			 c = REJ0;
// 		}

// 		// rej msg bcc1
// 		char bcc1 = XOR(UA_A, c);
// 		char rej[5] = {F, UA_A, c, bcc1, F};

// 		// write reject
// 		int ret = write(fd, rej, 5);
// 		printf("Send reject\n");
// 		return ret;
// }

// int acceptMsg(char cflag){
// 	// aceitar mensagem
// 		char c;
// 		if (cflag == 0x40){
// 			c = RR1;
// 		}
// 		else if (cflag == 0x00){
// 			 c = RR0;
// 		}

// 		// accept msg bcc1
// 		char bcc1 = XOR(UA_A, c);
// 		char rej[5] = {F, UA_A, c, bcc1, F};

// 		// write accept
// 		int ret = write(fd, rej, 5);
// 		printf("Send accept\n");
// 		return ret;
// }

// int llread(char *buffer){
// 	char temp[(PACKAGE_DATA_SIZE+4) *2], temp2[(PACKAGE_DATA_SIZE+4) *2], temp3[(PACKAGE_DATA_SIZE+4) *2];
// 	int length = readMsg(temp);

// 	if (temp[2] != C_FLAG) {
// 		// rejeitar mensagem
// 		rejectMsg(temp[2]);
// 		llread(buffer);
// 		return ERROR;
// 	}

// 	// remover cabeçalho e flag inicial
// 	int i, j;
// 	for (i = 4, j = 0 ; i < length-1; i++, j++){
// 		temp2[j] = temp[i];
// 	}
// 	length = j;
// 	// ver valor do bcc2 se está correcto
// 	int isValidBCC = (temp2[length-1] == (XOR(temp2[1], temp2[2])));
// 	//printf("bcc2: 0x%x\n",temp2[length-1]);
// 	//printf("bcc:%d\n", isValidBCC);

// 	// extrair pacote de comando da trama - destuffing
// 	length = j;
// 	for (i = 0, j = 0; i < length - 1 ; i++, j++) {
// 		if (temp2[i] == SETE_D && temp2[i+1] == 0x5e){
// 			temp3[j] = SETE_E;
// 			i++;
// 		}
// 		else if (temp2[i] == SETE_D && temp2[i+1] == 0x5d){
// 			temp3[j] = SETE_D;
// 			i++;
// 		}
// 		else {
// 			temp3[j] = temp2[i];
// 		}
// 	}
// 	//printBuffer(temp3, j);

// 	length = j;
// 	if (isValidBCC){
// 		for (i = 0; i < length; i++){
// 			buffer[i] = temp3[i];
// 		}

// 		// responder mensagem com sucesso
// 		getCFlag();
// 		acceptMsg(temp[2]);
// 		return i;

// 	} else {
// 		// rejeitar mensagem
// 		printf("bcc");
// 		rejectMsg(temp[2]);
// 		llread(buffer);
// 		return ERROR;
// 	}
// }

int llopen_Receiver(){
	Frame ua = {
		.msg = {F , UA_A , UA_C , UA_BCC1 , F},
		.length = 5
	};
	
	Frame f;
	int ret;

read_frame:
	ret = readFrame(&f);

	if (ret != ERROR) {
		int frame_type = checkFrame(f);
		if(frame_type == SET) {
			sendMsg(ua);
			free(f.msg);
			return 0;
		}
		else {
			goto read_frame;
		}
	}
	
	return ERROR;
}

int sendMsg(Frame f) {
	// write on serial port
	int res = write(fd, f.msg, sizeof(char) * f.length);
	printf("%d bytes written\n", res);
	fflush(NULL);

	return res;
}

int sendFrame(Frame f, Frame* response){
	int res;
	int STOP = FALSE;
	// reset global counter
	counter = 0;

	while (STOP==FALSE && counter < NUMBER_OF_TRIES) {
		
		sendMsg(f);

		alarm(3);
		flag = 0;

		if (readFrame(response) != ERROR){
			alarm(0);
			STOP = TRUE;
		}
  }

  if(STOP != TRUE)
	return ERROR;

  //sleep(1);

  return 0;
}

int checkFrame(Frame f) {
	if(f.msg[3] != XOR(f.msg[1], f.msg[2])) {
		return ERROR;
	}

	switch(f.msg[f.msg[2]]) {
		case SET_C:
			return SET;
		case DISC_C:
			return DISC;
		case UA_C:
			return UA;
		case RR0_C:
			return RR0;
		case RR1_C:
			return RR1;
		case REJ0_C:
			return REJ0;
		case REJ1_C:
			return REJ1;
		case I0_C:
			return I0;
		case I1_C:
			return I1;
		default:
			return ERROR;	
	}
}

int llopen_Sender(){
	Frame set = {
		.msg = {F , SET_A , SET_C , SET_BCC1 , F},
		.length = 5
	};

	Frame response;

	int ret = sendFrame(set, &response);

	if (ret != ERROR) {
		int frame_type = checkFrame(response);
		if(frame_type == UA) {
			return 0;
		}
	}

	return ERROR;
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

// void copyBuffer(char *dest, char *source, int length){
// 	int i = 0;
// 	for(i = 0; i < length; i++){
// 		dest[i] = source[i];
// 	}
// }

// char *generateBCC2(char *buffer, int *finalSize){
// 	*finalSize = *finalSize+1;

// 	char *buff1 = malloc(sizeof(char*) * (*finalSize));

// 	// generate bcc2
// 	char bcc2 = XOR(buffer[1], buffer[2]);

// 	// copy from buffer to buff1
// 	copyBuffer(buff1, buffer, *finalSize);

// 	// add bcc2 to buff1
// 	buff1[*finalSize-1] = bcc2;

// 	return buff1;
// }

// char *stuffing(char *buffer, int *finalSize){
// 	int i, j, size = 0;

// 	// count number of 0x7e and 0x7d
// 	for(i = 0; i < *finalSize; i++){
// 		if(buffer[i] == SETE_E || buffer[i] == SETE_D){
// 			size++;
// 		}
// 		size++;
// 	}

// 	char *buff = malloc(sizeof(char) * size);

// 		for(i = 0, j= 0; i < *finalSize; i++){
// 			if(buffer[i] == SETE_E){
// 				buff[j] = 0x7d;
// 				buff[j+1] = 0x5e;
// 				j++;
// 			}
// 			else if (buffer[i] == SETE_D){
// 				buff[j] = 0x7d;
// 				buff[j+1] = 0x5d;
// 				j++;
// 			} else {
// 				buff[j] = buffer[i];
// 			}
// 			j++;
// 		}

// 		*finalSize = size;


// 	return buff;
// }

char *createMsg(char *buffer, int *finalSize){
	char *msg = malloc(*finalSize + 5);

	msg[0] = F;
	msg[1] = UA_A;
	msg[2] = getCFlag();

	// add bcc1
	msg[3] = XOR(msg[1], msg[2]);

	// copy buffer
	copyBuffer(msg + 4, buffer, *finalSize);
	msg[*finalSize + 4] = F;

	*finalSize = *finalSize + 5;

	return msg;
}

// int llwrite(char *buffer, int length){
// 	int finalSize = length;

// 	// calculate BCC2
// 	char *buff1 = generateBCC2(buffer, &finalSize);

// 	// stuffing
// 	char *buff2 = stuffing(buff1, &finalSize);
// 	free(buff1);

// 	// junta cabecalho e flag inicial
// 	char *buff3 = createMsg(buff2,  &finalSize);
// 	free(buff2);

// 	// envia buff3 na porta serie;
// 	//printBuffer(buff3, finalSize);

// 	char response[5];
// 	int rej = 1;

// 	do {
// 		int r = sendMsg(buff3, finalSize, response);
// 		printf("Response Received: ");
// 		printBuffer(response, 5);

// 		if(r == ERROR){
// 			return ERROR;
// 		}

// 		if (r != ERROR && ((buff3[2] == 0x40 && response[2] == (char)RR0) || (buff3[2] == 0x00 && response[2] == (char)RR1))){
// 			rej = 0;
// 			return 0;
// 		}
// 		else if (r != ERROR && ((buff3[2] == 0x40 && response[2] == (char)REJ1) || (buff3[2] == 0x00 && response[2] == (char)REJ0))){
// 			rej = 1;
// 		} else {
// 			break;
// 		}
// 	} while(rej);

// 	free(buff3);
// 	return 0;
// }

int llclose(){
	sleep(1);
	if ( tcsetattr(fd,TCSANOW,&oldtio) == ERROR) {
    perror("tcsetattr");
    return ERROR;
  }
	return close(fd);
}
