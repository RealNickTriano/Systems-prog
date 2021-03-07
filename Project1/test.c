#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>



#ifndef DEBUG
#define DEBUG 0
#endif
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

		        if (isspace(character) != 0 && isspace(adjacent_character) == 0)         //Check if the current character is whitespace (0 means whitespace)             
		        {
				
		            word_start = i;                         // This is where the word starts
		        }
		        else if (isspace(character) == 0 && isspace(adjacent_character) != 0)
		        {
				
		            word_end = i + 1;                           //This is where the word ends
					end_of_word = 1;
		        }
		        else
		        {
		            //We are in the middle of a word
		        }
				if(end_of_word == 1)										// Print a word everytime we reach the end
				{															// need to figure out how to move the chars in the buffer over to overwrite the word we just wrote
					word_len = (word_end - word_start);
					write(0, &buf[word_start], sizeof(char) * (word_len));
					end_of_word = 0;
				}
				
		}
}

int main(int argc, char **argv)
{
    int input_fd, bytes;
	int end_of_word = 0;
	int width = 20;
    char buf[width];
	char overflow_buf[64];
	int bytes_read, i;

    if (argc != 2) return EXIT_FAILURE;
    
    input_fd = open(argv[1], O_RDONLY);								// open input file in read only
    if (input_fd == -1) {
	perror(argv[2]);
	return EXIT_FAILURE;
    }



    	bytes_read = read(input_fd, buf, 20); 
		CheckWord(width,buf);									// Call CheckWord to find the words in the buffer          
         
               
		putchar('\n');

    if (bytes < 0) 
	{
		perror("Read error");
    }

    close(input_fd);						// close input file

    return EXIT_SUCCESS;
}