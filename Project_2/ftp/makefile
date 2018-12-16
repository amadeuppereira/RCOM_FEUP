all: clientFTP clientTCP download
	gcc clientTCP.o clientFTP.o download.o -o download

download: 
	gcc download.c -c

clientFTP: 
	gcc clientFTP.c -c 

clientTCP:
	gcc clientTCP.c -c 

clean:
	rm download *.o
