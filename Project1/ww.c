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
	int found_end = 0;                                                        //boolean to check if we found the end of a word
	char overflow_buf[64];
	int width_left, word_len;
    int word_start = 0;                                                         // start position of a word
	int word_end = 0;                                                           // end position of a word
    int overflow = 0;                                                           // boolean to check if we recently put stuff in the overflow buffer
    int k, j;
    width_left = width;

 for(int i = 0; i < width; i++)
    {
		character = buf[i];
		adjacent_character = buf[i + 1];

        if(isspace(character) != 0 && overflow == 1)                // This will hopefully only trigger after we have something in the over flow 
                                                                    // and the first character of the new buffer is a space (the whole word is in overflow)
        {
            write(0, &overflow_buf[0], sizeof(char) * j);          // j is the amount of characters we copied to the buffer
            overflow = 0;                                          // reset overflow value
        }

		else if (isspace(character) != 0 && isspace(adjacent_character) == 0)         //Check if the current character is whitespace (!= 0 means whitespace)             
		{
				
		    word_start = i;                         // This is where the word starts
		}
		else if (isspace(character) == 0 && isspace(adjacent_character) != 0)
		{
				
		    word_end = i + 1;                           //This is where the word ends
			found_end = 1;                            // Set this to true so we know when we found the end of a word
		}
        
		else
		{
		    //We are in the middle of a word
            if(i == width - 1)                                         // If its the end of the buffer and we are in the middle of a word
            {
                
                j = 0;
                for (k = word_start; k < width; k++)               // Copy the part of the word (from the first non-whitespace character) to an overflow buffer
                {
                    overflow_buf[j] = buf[k];
                    j++;
                }
                overflow = 1;                                       // set overflow to true
                
            }
		}                                                           
		
        if(overflow == 1)                                   // if we had stuff in the overflow print that
        {
            write(0, &overflow_buf[0], sizeof(char) * j);
            overflow = 0;                                               // reset overflow value	
        }
        if(found_end == 1)										// Print a word everytime we reach the end of one
		{												
			word_len = (word_end - word_start);                             // Calculate the length of that word
			write(0, &buf[word_start], sizeof(char) * (word_len));          // Write to stdout the whole word
			found_end = 0;                                                // set this too false 
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
    int width;
    
    
    // Check args
    if(argc > 3 || argc == 1)                  // Make sure enough arguements are passed
    {
        return EXIT_FAILURE;
    }
    else                            		// If theres no output file given 
    {
        output_fd = 0;                         // output is standard output
    }
    /*else
    {
        output_fd = fileno(*argv[3]);           // If there is an output file given get the file descriptor ***FOR PART 2***
    }*/

    width = atoi(argv[1]);                       // get the width from input (atoi casts to int)
    if (width <= 0)
        return EXIT_FAILURE;
    char buf[width];                            // create buffer with width as size

    input_fd = open(argv[2], O_RDONLY);         // Open input file in read only
    if (input_fd == -1)                         // returns -1 if couldnt open file
    {
        return EXIT_FAILURE;
    }

    while(bytes_read = read(input_fd, buf, width) > 0)                // continously read in sizes of the width
    {
        CheckWord(width,buf);									// Call CheckWord to find the words in the buffer (This is also writing currently)
        write(output_fd, "\n", 1);
    }
         
               
		putchar('\n');

    if (bytes_read < 0) 
	{
		perror("Read error");
    }

    close(input_fd);						// close input file

    return EXIT_SUCCESS;
}