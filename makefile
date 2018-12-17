all: clientFTP clientTCP download
	gcc clientTCP.o clientFTP.o download.o -o download

download: 
	gcc download.c -c -std=c11 -Wall

clientFTP: 
	gcc clientFTP.c -c -std=c11 -Wall

clientTCP:
	gcc clientTCP.c -c -std=c11 -Wall

clean:
	rm download *.o
