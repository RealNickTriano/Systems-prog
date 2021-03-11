#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

int main(int argc, char **argv)
{
        int input_fd, output_fd, bytes_read;
        int width;
        DIR *dir_pointer;
        struct dirent *de; //struct to contain data about next file entry

        if (argc < 1 || argc > 3) //Checks number of arguments
                return EXIT_FAILURE;
        else                   //If no output file is given (argc = 2)
                output_fd = 0; //Uses standard output

        if (is_directory(argv[1]) != 0)
        {
                //this is a directory
                dir_pointer = opendir(argv[3]);
                if (dir_pointer == NULL)
                {
                        perror("Problem opening directory");
                }
        }

        while (de = readdir(dir_pointer))
        {
                puts(de->d_name);
                printf("%lu %d %s\n",
                de->d_ino,
                de->d_type,
                de->d_name);
        }

        int dir = closedir(dir_pointer); //Close directory
        if (dir = -1)
        {
                perror("Error Closing Directory");
        }
        return EXIT_SUCCESS;
}