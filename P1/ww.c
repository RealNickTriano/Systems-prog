#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include "strbuf.h"

#ifndef BUFSIZE
#define BUFSIZE 128
#endif

int overflow = 0;
int j; 

int wrap(int width, char* buf)
{
	char character, adjacent_character;
	int found_end = 0;                                                        //boolean to check if we found the end of a word
	char overflow_buf[64];
	int width_left, word_len;
    int word_start = 0;                                                         // start position of a word
	int word_end = 0;                                                           // end position of a word
    int k;
    width_left = width;

 for(int i = 0; i < BUFSIZE; i++)
    {
		character = buf[i];
		adjacent_character = buf[i + 1];

        if(isspace(character) != 0 && overflow == 1)                // This will hopefully only trigger after we have something in the over flow 
                                                                    // and the first character of the new buffer is a space (the whole word is in overflow)
        {
            write(0, &overflow_buf[0], sizeof(char) * j);          // j is the amount of characters we copied to the buffer
            overflow = 0;                                          // reset overflow value
        }

        // (!= 0 means whitespace)
		else if (isspace(character) != 0 && isspace(adjacent_character) == 0)             
		{
		    word_start = i;
		}
		else if (isspace(character) == 0 && isspace(adjacent_character) != 0)
		{	
		    word_end = i + 1;
			found_end = 1;
		}
		else
		{
            if(i == BUFSIZE - 1)
            {
                j = 0;
                for (k = word_start; k < BUFSIZE; k++)
                {
                    overflow_buf[j] = buf[k];
                    j++;
                }
                overflow = 1;
            }
            return 0;
		}                                                           
		
        if(found_end == 1)
		{	
            if(overflow == 1){
                word_len = j + (word_end - word_start);
                if (word_len > width_left){
                    write(output_fd, "\n", 1);
                    width_left = width;
                }
                write(0, &overflow_buf[0], sizeof(char) * j);
                overflow = 0;	
            }
            else{											
			    word_len = (word_end - word_start);
                if (word_len > width_left){
                write(output_fd, "\n", 1);
                width_left = width;
                }
            }
			write(0, &buf[word_start], sizeof(char) * (word_len));
            width_left -= word_len;    
			found_end = 0;
		}	
	}
    // We come out of this at the end of the buffer (i = 19)
    /* Next steps, if we end in the middle of a word put the beginning of that word in another buffer
        -> call read again to overwrite buffer
        -> If new buffer starts with a space then we have the completed word in other buffer
        -> else keep going untill we find the space (part of word is still stashed)
        -> words can be too big for the buffer.*/
}

int main(int argc, char **argv)
{
    int input_fd, output_fd, bytes_read, i;
    
    if(argc <= 1 || argc > 3)
        return EXIT_FAILURE;
    else
        output_fd = 0;

    /*else
    {
        output_fd = fileno(*argv[3]);           // If there is an output file given get the file descriptor ***FOR PART 2***
    }*/

    int width = atoi(argv[1]);
    if (width <= 0)
        return EXIT_FAILURE;

    char buf[BUFSIZE];

    input_fd = open(argv[2], O_RDONLY);
    if (input_fd == -1)
    {
        return EXIT_FAILURE;
    }

    while(bytes_read = read(input_fd, buf, BUFSIZE) > 0)
    {
        wrap(width,buf);
        write(output_fd, "\n", 1);
    }
         
               
		putchar('\n'); // Can't use putchar

    if (bytes_read < 0) 
	{
		perror("Read error");
    }

    close(input_fd);
    return EXIT_SUCCESS;
}
