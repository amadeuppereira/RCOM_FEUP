all: sender receiver

sender:
	gcc -o sender logic.c sender.c -Wall
	
receiver:
	gcc -o receiver logic.c receiver.c -Wall

clean:
	rm sender receiver
