all: clientFTP

clientFTP: clientTCP.o
	gcc clientFTP.c -o clientFTP

clientTCP.o:
	gcc -c clientTCP.c

clean:
	rm getip clientFTP
