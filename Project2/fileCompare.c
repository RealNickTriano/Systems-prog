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
#include "Queue.h"

#ifndef DEBUG
#define DEBUG 0
#endif

int directory_threads = 1, file_threads = 1, analysis_threads = 1;
char file_name_suffix[] = ".txt";


int is_directory(const char *path)// !=0 if file is directory
{
    struct stat statbuf;
    stat(path, &statbuf);
    return S_ISDIR(statbuf.st_mode);
}

int is_file(const char *path) //!=0 if file is regular file
{
        struct stat statbuf;
        stat(path, &statbuf);
        return S_ISREG(statbuf.st_mode);
}

/*int FindWFD(const char *path)
{
    int bytes_read, input_fd;
    char buf[100];

    input_fd = open(path, O_RDONLY);
    while ((bytes_read = read(input_fd, buf, 100)) > 0) //reading file
    {
        // convert letters to all lowercase
        for (i = 0; i < bytes_read; i++) 
        {
	        buf[i] = tolower(buf[i]);
	    }
        // Find words in read
        // Once you find a word check if its already in the list
        // if not Add word to linked list
        // if already in list increment its respective int +1
        // in the other list of ints

        
    }
}*/
int SetOptions(char *argv)  // sets number of threads/ file name suffix
{
    if(strncmp(argv, "-d", sizeof(char) * 2) == 0)
    {   
        if(atoi(&argv[2]) <=0 )
        {
            fprintf(stderr, "Invalid number of threads: %s\n", argv);
            exit(EXIT_FAILURE);
        }
        else
            directory_threads = atoi(&argv[2]);
    }
    else if(strncmp(argv, "-f", sizeof(char) * 2) == 0)
    {
        if(atoi(&argv[2]) <=0 ){
            fprintf(stderr, "Invalid number of threads: %s\n", argv);
            exit(EXIT_FAILURE);
        }
        else
        file_threads = atoi(&argv[2]);
    }
    else if(strncmp(argv, "-a", sizeof(char) * 2) == 0)
    {
        if(atoi(&argv[2]) <=0 ){
            fprintf(stderr, "Invalid number of threads: %s\n", argv);
            exit(EXIT_FAILURE);
        }
        else
        analysis_threads = atoi(&argv[2]);
    }
    else if(strncmp(argv, "-s", sizeof(char) * 2) == 0)
    {
        // set new file name suffix
    }
    else
    {
        fprintf(stderr, "Invalid optional argument: %s\n", argv);
        exit(EXIT_FAILURE);
    }
    return 0;
}

int CheckArgs(char **argv, int argc, int opt_arg_count) // Checks arguments seperates optional/regular
{
     // number of optional arguments inputed

    for (int i = 1; i < argc; i++)
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
    struct targs *args;
	pthread_t *tids;
    
    if (argc < 3) //not enough arguments
    {
        return EXIT_FAILURE;
    }

    queue_t file_q;     // Create and initialize file/directory queues
    init(&file_q);
    queue_t directory_q;
    init(&directory_q);

    opt_arg_count = CheckArgs(argv, argc, opt_arg_count); // Check for optional arguments

    for (int i = 1; i < argc; i++)
    {
        if (is_directory(argv[i]) != 0) // found a directory
        {
            // add to directory queue
            enqueue(&directory_q, argv[i]);
            //start directory threads
        }
        else if (is_file(argv[i]) != 0) // found a file
        {
            // add to file queue
            enqueue(&file_q, argv[i]);
            //start file threads
        }
        else{}
    }
    if(DEBUG)
    {
	printf("File Queue...\n");
        printQueue(&file_q);
	printf("Directory Queue...\n");
	printQueue(&directory_q);
    }
    
	if(DEBUG)
	{
		printf("dir threads: %d\nfile threads: %d\nanalysis threads: %d\noptional arguments: %d\n",directory_threads, file_threads, analysis_threads, opt_arg_count);
	}
    return EXIT_SUCCESS;
}