#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>


#ifndef DEBUG
#define DEBUG 0
#endif

#ifndef BUFSIZE
#define BUFSIZE 128
#endif

int overflow = 0;
int overcount = 0;
int overflow_buf[64];

int wrap(int width, char *buf, int output_fd, int width_left)
{
        char character, adjacent_character;
        int found_end = 0;  //A boolean to check if we found the end of a word
        char extra_buf[64]; //An extra buffer to copy to global overflow buffer
        int word_len;
        int word_start = 0; //The start position of a word
        int word_end = 0;   //The end position of a word

        for (int i = 0; i < BUFSIZE; i++)
        {
                character = buf[i];
                adjacent_character = buf[i + 1];

                if (character == 0 && adjacent_character == 0)
                {
                        return width_left;
                }
                // (isspace(char) != 0 means whitespace)
                else if (isspace(character) != 0 && isspace(adjacent_character) == 0)
                {
                        word_start = i + 1; //Starting index of word is set to the first LETTER
                }
                else if (isspace(character) == 0 && isspace(adjacent_character) != 0)
                {
                        word_end = i;  //Ending index of word is set to the last LETTER
                        found_end = 1; //Sets boolean to true for finding the end of a word
                }
                else
                {
                        if (i == BUFSIZE - 1)
                        { //If we reach the end of the buffer in the middle of a word
                                if (DEBUG)
                                        printf("Accessing Overflow Buffer \n");
                                overcount = 0; //Sets overflow buffer size count to zero
                                for (int k = word_start; k < BUFSIZE; k++)
                                {
                                        extra_buf[overcount] = buf[k]; //Copies from buffer to extra buffer from the start of the word
                                        overcount++;
                                }

                                memcpy(overflow_buf, extra_buf, overcount); //Sets global overflow buffer to equal extra buffer
                                overflow = 1;                               //Sets global overflow boolean to true

                                return width_left;
                        }
                }

                if (found_end == 1)
                {
                        //if (DEBUG) printf("Finished Word \n");
                        if (overflow == 1)
                        { //If global overflow boolean is set to true
                                if (DEBUG)
                                        printf("Writing Overflow\n");
                                word_len = overcount + (word_end - word_start + 1); //Word length is the sum of of the size of the overflow buffer plus size of current buffers remaining part of word
                                if (word_len > width_left)
                                {                                  //Checks if word PLUS the space after it will fit in the desired width
                                        write(output_fd, "\n", 1); //If there's not enough space starts a new line
                                        width_left = width;
                                }
                                write(output_fd, &overflow_buf[0], sizeof(char) * overcount); //Writes overflow part of word
                                overflow = 0;                                                 //Resets overflow boolean
                                word_len -= overcount;                                        //Removes overflow word size from word length
                                width_left -= overcount;
                        }
                        else
                        {
                                word_len = word_end - word_start + 1;
                                if (word_len > width_left)
                                {
                                        write(output_fd, "\n", 1);
                                        width_left = width;
                                }
                        }
                        //if (DEBUG) printf("Writing Word Length %d \n", word_len);
                        write(output_fd, &buf[word_start], sizeof(char) * (word_len)); //Writes full word OR remainder after overflow
                        write(output_fd, " ", 1);                                      //Writes space after word
                        width_left -= word_len + 1;                                    //Removes word + space from width
                        found_end = 0;                                                 //Resets found end of word boolean
                }
        }
        return width_left;
        // We come out of this at the end of the buffer (i = 19)
        /* Next steps, if we end in the middle of a word put the beginning of that word in another buffer
        -> call read again to overwrite buffer
        -> If new buffer starts with a space then we have the completed word in other buffer
        -> else keep going untill we find the space (part of word is still stashed)
        -> words can be too big for the buffer.*/
}

int main(int argc, char **argv)
{
        int input_fd, output_fd, bytes_read;
        int width;

        if (argc < 1 || argc > 3) //Checks number of arguments
                return EXIT_FAILURE;
        else                   //If no output file is given (argc = 2)
                output_fd = 0; //Uses standard output

        if (argc == 3)         // check second arg if past
        {
                int status = stat(*argv[2], statbuf);
        }

        width = atoi(argv[1]); //Gets the width from input
        int width_left = width;
        if (width <= 0)
                return EXIT_FAILURE;

        char buf[BUFSIZE]; //Creates buffer with BUFSIZE

        input_fd = open(argv[2], O_RDONLY); //Opens input file in read only
        if (input_fd == -1)
        {
                return EXIT_FAILURE;
        }

        for (int i = 0; i < width; i++)
        {
                write(0, " ", 1);
        }

        write(0, "|\n", 1);
        write(0, "\n", 1);

        while (bytes_read = read(input_fd, buf, BUFSIZE) > 0) //Continuously refreshes the buffer
        {
                width_left = wrap(width, buf, output_fd, width_left); //Calls word wrapping method
                if (DEBUG)
                        printf("Refreshing Buffer\n");
                memset(buf, 0, sizeof(char) * BUFSIZE);
        }

        putchar('\n'); //Adds line at the end of program for AESTHETIC purposes only

        if (bytes_read < 0)
        {
                perror("Read error");
        }

        close(input_fd); //Closes input file
        return EXIT_SUCCESS;
}