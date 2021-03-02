#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "strbuf.h"
#include <fcntl.h>
#include <ctype.h>

int main(int argc, char **argv)
{
    strbuf_t buf;
    sb_init(&buf, 100);
    int bytes, i;
    int width = 20;
    int BUFSIZE = 256;
    //char buf[BUFSIZE];
    if(argc > 2 || argc == 0)               // Make sure enough arguements are passed
    {
        return EXIT_FAILURE;
    }

    int fd = open(argv[1], O_RDONLY);        // Open file in read only
    if (fd == -1)                           // returns -1 if couldnt open file
    {
        return EXIT_FAILURE;
    }

    while(( bytes = read(fd, &buf, BUFSIZE)) > 0 )       // while we are reading 
    {
        write(0, &buf, (sizeof(char) * width));
	    putchar('\n');
	
    }

    if (bytes < 0)                             // if bytes read are negative??
    {
        perror("Read error");
    }

    close(fd);                               // closes file
}