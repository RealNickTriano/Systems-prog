#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "strbuf.h"
#include <fcntl.h>
#include <ctype.h>

int main(int argc, char **argv)
{
    if(argc > 2 || argc == 0)               // Make sure enough arguements are passed
    {
        return EXIT_FAILURE;
    }

    int fd = open(argv[1], O_RDONLY);        // Open file in read only
    if (fd == -1)                           // returns -1 if couldnt open file
    {
        return EXIT_FAILURE;
    }


    close(fd);                               // closes file
}