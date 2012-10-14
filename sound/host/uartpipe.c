#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <termios.h>

#define CHUNKSIZE 192

int main(int argc, char** argv) {
	FILE* tty;
	int ttyId;
	int c;
	char val;
	char buf[CHUNKSIZE];
	
	struct termios termOptions;


	if(argc < 2) {
		fprintf(stderr, "you need to give the tty device as first argument.\n");
		exit(1);
	}

	ttyId = open (argv[1], O_RDWR | O_NOCTTY );
	if (ttyId < 0) {
	        perror ("opening TTY");
		exit (2);
	}
	tcgetattr( ttyId, &termOptions );
	cfsetispeed( &termOptions, B921600 );
	cfsetospeed( &termOptions, B921600 );
	tcsetattr( ttyId, TCSANOW, &termOptions );
	close(ttyId);                               
	                               
	
	tty = fopen(argv[1], "r+");
	if(tty == NULL) {
		perror("opening TTY");
		exit(2);
	}

	while(1) {
		c = fread(&val, 1, 1, tty);
		if(c != 1) {
			fprintf(stderr, "error reading from TTY\n");
			exit(3);
		}
		c = fread(buf, CHUNKSIZE, 1, stdin);
		if(c != 1) {
			fprintf(stderr, "error reading from stdin\n");
			exit(4);
		}
		fwrite(buf, CHUNKSIZE, 1, tty);
	}
}
