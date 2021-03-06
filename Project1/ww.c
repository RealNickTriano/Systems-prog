#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "strbuf.h"
#include <fcntl.h>
#include <ctype.h>

int Wrap(unsigned width, int input_fd, int output_fd)
    {
        int BUFSIZE = 256;
        int bytes_read, i, width_left, word_len, word_end;
        int word_start = 0;
	    char character, adjacent_character;
        char buf[BUFSIZE];
        strbuf_t overflow_buf;
        sb_init(&overflow_buf, BUFSIZE);
        width_left = width;

        while((bytes_read = read(input_fd, buf, BUFSIZE)) > 0)              
        {    
            for(i = 0; i < BUFSIZE; i++)
            {
                character = buf[i];
                adjacent_character = buf[i + 1];

                if ((isspace(character) == 1 && isspace(adjacent_character) != 1))         //Check if the current character is whitespace (1 means whitespace)             
                {
                    word_start = i;                         // This is where the word starts
                }
                else if((isspace(character)) != 1 && (isspace(adjacent_character)) != 1)
                {
                    word_end = i;                           //This is where the word ends
                }
                else
                {
                    //We are in the middle of a word
                }
                if(word_end <= width_left)
                {
                    word_len = word_end - word_start;               // Calculate word length
                    //Print word
                    write(output_fd, buf, word_len);                // Write to output the word
                }

            }     
              
        }
    return 0;
	}

    

int main(int argc, char **argv)
{
    int input_fd, output_fd;
    int width = *argv[1];
    FILE* input_file;
    
    // Check args
    if(argc > 3 || argc == 0)                  // Make sure enough arguements are passed
    {
        return EXIT_FAILURE;
    }
    else                              		// If theres no output file given 
    {
        output_fd = 1;                         // output is standard output
    }
    /*else
    {
        output_fd = fileno(*argv[3]);           // If there is an output file given get the file descriptor ***FOR PART 2***
    }*/
    
    input_fd = open(argv[2], O_RDONLY);         // Open input file in read only
    input_file = fdopen(input_fd, O_RDONLY);    // make a FILE struct from input file discriptor
    if (input_fd == -1)                         // returns -1 if couldnt open file
    {
        return EXIT_FAILURE;
    }

    int error = Wrap(width, input_file, output_fd);           // Call wrap

    if(error > 0)
    {
        printf("Error: Exceeded page width");
    }

    // Close both files
    close(input_fd);  
    close(output_fd);                               
}

