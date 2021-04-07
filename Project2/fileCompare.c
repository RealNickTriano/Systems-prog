#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <signal.h>
#include <pthread.h>

#ifndef DEBUG
#define DEBUG 0
#endif

int directory_threads = 1, file_threads = 1, analysis_threads = 1;
char file_name_suffix[] = ".txt";

int SetOptions(char *argv)
{
    if(strncmp(argv, "-d", sizeof(char) * 2) == 0)
    {   
        directory_threads = atoi(&argv[2]);
    }
    else if(strncmp(argv, "-f", sizeof(char) * 2) == 0)
    {
        file_threads = atoi(&argv[2]);
    }
    else if(strncmp(argv, "-a", sizeof(char) * 2) == 0)
    {
        analysis_threads = atoi(&argv[2]);
    }
    else if(strncmp(argv, "-s", sizeof(char) * 2) == 0)
    {
        // set new file name suffix
    }
    return 0;
}

int CheckArgs(char **argv)
{

    for (int i = 0; i < 4; i++)
    {
        if (strncmp(argv[i], "-", sizeof(char)) == 0) // found an optional arg
        {
            SetOptions(argv[i]); // set thread/file suffix value
        }
    }
    return 0;
    
}
int main(int argc, char **argv)
{
    if (argc < 3) //not enough arguments
    {
        return EXIT_FAILURE;
    }

    CheckArgs(argv); // Check for optional arguments, we will limit these to come before and typed once

	if(DEBUG)
	{
		printf("dir threads: %d\n
                file threads: %d\n
                analysis threads: %d\n", 
                directory_threads, file_threads, analysis_threads);
	}
    return EXIT_SUCCESS;
}