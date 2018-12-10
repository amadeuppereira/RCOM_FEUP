all: clientFTP clientTCP
	gcc clientTCP.o clientFTP.o -o download


clientFTP: clientTCP
	gcc clientFTP.c -c 

clientTCP:
	gcc clientTCP.c -c 

clean:
	rm download *.o
