#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "strbuf.h"
#include <fcntl.h>
#include <ctype.h>  

int CheckWord(int width, char* buf)
{
	char character, adjacent_character;
	int end_of_word = 0;
	char overflow_buf[64];
	int width_left, word_len;
    int word_start = 0;
	int word_end = 0;
    width_left = width;

 for(int i = 0; i < 19; i++)
    {
		character = buf[i];
		adjacent_character = buf[i + 1];

		if (isspace(character) != 0 && isspace(adjacent_character) == 0)         //Check if the current character is whitespace (!= 0 means whitespace)             
		{
				
		    word_start = i;                         // This is where the word starts
		}
		else if (isspace(character) == 0 && isspace(adjacent_character) != 0)
		{
				
		    word_end = i + 1;                           //This is where the word ends
			end_of_word = 1;                            // Set this to true so we know when we found the end of a word
		}
		else
		{
		    //We are in the middle of a word
		}
		if(end_of_word == 1)										// Print a word everytime we reach the end of one
		{															
			word_len = (word_end - word_start);                             // Calculate the length of that word
			write(0, &buf[word_start], sizeof(char) * (word_len));          // Write to stdout the whole word
			end_of_word = 0;                                                // set this too false 
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
    int width = 20;
    char buf[width];
    
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
    
    input_fd = open(argv[1], O_RDONLY);         // Open input file in read only
    if (input_fd == -1)                         // returns -1 if couldnt open file
    {
        return EXIT_FAILURE;
    }

    bytes_read = read(input_fd, buf, 20); 
    CheckWord(width,buf);									// Call CheckWord to find the words in the buffer (This is also writing currently)        
         
               
		putchar('\n');

    if (bytes_read < 0) 
	{
		perror("Read error");
    }

    close(input_fd);						// close input file

    return EXIT_SUCCESS;
}

