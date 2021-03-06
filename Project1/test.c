#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

#ifndef BUFSIZE
#define BUFSIZE 256
#endif

#ifndef DEBUG
#define DEBUG 0
#endif

int main(int argc, char **argv)
{
    int fd, bytes, i;
    char buf[BUFSIZE];
    if (argc != 2) return EXIT_FAILURE;
    
    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
	perror(argv[1]);
	return EXIT_FAILURE;
    }

    while ((bytes = read(fd, buf, BUFSIZE)) > 0) {
	if (DEBUG) {
	    printf("%d bytes: |", bytes);
	    for (i = 0; i < bytes; ++i) {
		putchar(buf[i]);
	    }
	    putchar('|');
	    putchar('\n');
	    continue;
	}

	for (i = 0; i < bytes; ++i) {
	    buf[i] = toupper(buf[i]);
	}

	write(0, buf, bytes);
    }
    if (bytes < 0) {
	perror("Read error");
    }

    close(fd);

    return EXIT_SUCCESS;
}