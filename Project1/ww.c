#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#ifndef DEBUG
#define DEBUG 0
#endif

#ifndef BUFSIZE
#define BUFSIZE 128
#endif

int overflow = 0;
int overcount = 0;
int overflow_buf[64];
int input_fd, output_fd, width;
int width_left;
int directory = 0;
int file = 0;

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
}

int is_directory(const char *path)
{
        printf("isDIR");
        struct stat statbuf;
        stat(path, &statbuf);
        return S_ISDIR(statbuf.st_mode);
}

int is_file(const char *path)
{
        struct stat statbuf;
        stat(path, &statbuf);
        return S_ISREG(statbuf.st_mode);
}

int wrap_file(int input_fd, char *buf, int output_fd, int width_left, int width)
{
        int bytes_read;
        width_left = width;

        while (bytes_read = read(input_fd, buf, BUFSIZE) > 0) //Continuously refreshes the buffer
        {
                width_left = wrap(width, buf, output_fd, width_left); //Calls word wrapping method
                if (DEBUG)
                        printf("Refreshing Buffer\n");
                memset(buf, 0, sizeof(char) * BUFSIZE);
        }

        if (bytes_read < 0)
        {
                perror("Read error");
        }
}

int manageDirectory(DIR *dir_pointer, char **argv, char *buf)
{
        char *output_name = "wrap.";
        printf("managing dir...");
        struct dirent *de; //struct to contain data about next file entry
        dir_pointer = opendir(argv[2]);
        if (dir_pointer == NULL)
        {
                perror("Problem opening directory");
        }
        int ch = chdir(argv[2]); // change working directory to dir_pointer file name
        if (ch == -1)
        {
                perror("Problem changing directory");
        }
        printf("changing dir...");
        while (de = readdir(dir_pointer)) // Loops through all files in the directory first two are "." and ".."
        {                                 // access fields using de->d_ino, de->d_type, de->d_name
                if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
                {
                        // go to the next files
                }
                else
                {
                        if (de->d_type == DT_REG)
                        {
                                input_fd = open(de->d_name, O_RDONLY); //also need directory name for full path i think

                                output_name = strcat(output_name, de->d_name); // make output file name
                                output_fd = open(output_name, O_WRONLY | O_TRUNC | O_CREAT);
                                if (output_fd == -1)
                                {
                                        return EXIT_FAILURE;
                                }
                                // need path for this too
                                // Need to set input_fd/output_fd before call
                                wrap_file(input_fd, buf, output_fd, width_left, width); // Call wrap on the file
                                printf("wrapped file");
                                close(input_fd);  //Closes input file(read)
                                close(output_fd); //Closes output file(write)
                        }
                        else if (de->d_type == DT_DIR)
                        {
                                // go to next file, we skip subdirectories
                        }
                }
        }
}

int main(int argc, char **argv)
{

        DIR *dir_pointer;

        if (argc < 1 || argc > 3) //Checks number of arguments
                return EXIT_FAILURE;
        else if (argc == 2)    //If no output file is given (argc = 2)
                output_fd = 0; //Uses standard output

        width = atoi(argv[1]); //Gets the width from input
        if (width <= 0)
                return EXIT_FAILURE;

        if (is_directory(argv[2]) != 0)
        {
                //this is a directory
                directory = 1;
                printf("%d", is_directory(argv[2]));
                char buf[BUFSIZE]; //Creates buffer with BUFSIZE
                manageDirectory(dir_pointer, argv, buf);
        }

        else if (is_file(argv[2]) != 0)
        {
                //this is a file
                file = 1;
                printf("\nFILE");
                char buf[BUFSIZE];                  //Creates buffer with BUFSIZE
                input_fd = open(argv[2], O_RDONLY); //Opens input file in read only
                if (input_fd == -1)
                {
                        return EXIT_FAILURE;
                }
                wrap_file(input_fd, buf, output_fd, width_left, width);
        }

        putchar('\n'); //Adds line at the end of program for AESTHETIC purposes only

        if (file == 1) // If we just opened/wrote to one file
        {
                printf("\nFILE");
                close(input_fd);  //Closes input file
                close(output_fd); //Closes output file
        }

        if (directory == 1)
        {
                int dir = closedir(dir_pointer); //Close directory
                if (dir = -1)
                        perror("Error Closing Directory");
        }

        return EXIT_SUCCESS;
}