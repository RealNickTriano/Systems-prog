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
    else
    {
        fprintf(stderr, "Invalid Optional Argument: %s\n", argv);
        exit(EXIT_FAILURE);
    }
    return 0;
}

int CheckArgs(char **argv, int argc, int opt_arg_count)
{
     // number of optional arguments inputed

    for (int i = 0; i < argc; i++)
    {
        if (strncmp(argv[i], "-", sizeof(char)) == 0) // found an optional arg
        {
            SetOptions(argv[i]); // set thread/file suffix value
            opt_arg_count++;
        }
    }
    return opt_arg_count;
    
}
int main(int argc, char **argv)
{
    int opt_arg_count = 0;
    
    if (argc < 3) //not enough arguments
    {
        return EXIT_FAILURE;
    }

    opt_arg_count = CheckArgs(argv, argc, opt_arg_count); // Check for optional arguments, we will limit these to come before and typed once

	if(DEBUG)
	{
		printf("dir threads: %d\nfile threads: %d\nanalysis threads: %d\noptional arguments: %d\n",directory_threads, file_threads, analysis_threads, opt_arg_count);
	}
    return EXIT_SUCCESS;
}