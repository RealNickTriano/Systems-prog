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
#include "linkedlist.h"

#ifndef DEBUG
#define DEBUG 0
#endif

#ifndef BUFSIZE
#define BUFSIZE 128
#endif

int directory_threads = 1, file_threads = 1, analysis_threads = 1;
char file_name_suffix[] = ".txt";

typedef struct wfd_array
{
    node_t *root;
    char *path_name;

} wfd_array;

int is_directory(const char *path) // !=0 if file is directory
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

int FindWFD(const char *path)
{
    int bytes_read, input_fd;
    float words_found = 0;
    float wfd = 0.0;
    char buf[BUFSIZE];
    char character, adjacent_character;
    int found_end = 0; //A boolean to check if we found the end of a word
    int word_len;
    int word_start = 0; //The start position of a word
    int word_end = 0;   //The end position of a word
    char *word;
    node_t *list;

    input_fd = open(path, O_RDONLY);
    while ((bytes_read = read(input_fd, buf, BUFSIZE)) > 0) //reading file
    {
        // convert letters to all lowercase
        for (int i = 0; i < bytes_read; i++)
        {
            buf[i] = tolower(buf[i]);
        }

        for (int i = 0; i < bytes_read; i++)
        {
            character = buf[i];
            adjacent_character = buf[i + 1];

            if (isspace(character) != 0 && isspace(adjacent_character) == 0)
            {
                word_start = i + 1; //Starting index of word is set to the first LETTER
            }
            else if (isspace(character) == 0 && isspace(adjacent_character) != 0)
            {
                word_end = i;  //Ending index of word is set to the last LETTER
                found_end = 1; //Sets boolean to true for finding the end of a word
                words_found++;
            }

            if (found_end == 1)
            {
                found_end = 0; // reset found_end

                if (ispunct(buf[word_end]) != 0)
                {
                    // word ends with punctuation
                    // so get rid of that from the word
                    word_end--;
                }
                word_len = word_end - word_start + 1; // Calculate word length

                word = malloc(sizeof(char) * word_len);
                memcpy(word, &buf[word_start], sizeof(char) * word_len);

                /* if(DEBUG)
                {
                    printf("%s\n", word);
                }*/
                // now we can add the word to linked list
                if (words_found == 1)
                {
                    list = initNode(word);
                }
                else
                {
                    list = add(list, word);
                }
            }
        }
    }
    // done reading file
    if (DEBUG)
    {
        print(list);
    }

    // now calculate WFD of file
    while (list != NULL)
    {
        wfd = (list->count / words_found);
        if (DEBUG)
        {
            printf("%.2lf ", wfd);
        }

        list->frequency = wfd;
        list = list->next;
    }
}
int SetOptions(char *argv) // sets number of threads/ file name suffix
{
    if (strncmp(argv, "-d", sizeof(char) * 2) == 0)
    {
        if (atoi(&argv[2]) <= 0)
        {
            fprintf(stderr, "Invalid number of threads: %s\n", argv);
            exit(EXIT_FAILURE);
        }
        else
            directory_threads = atoi(&argv[2]);
    }
    else if (strncmp(argv, "-f", sizeof(char) * 2) == 0)
    {
        if (atoi(&argv[2]) <= 0)
        {
            fprintf(stderr, "Invalid number of threads: %s\n", argv);
            exit(EXIT_FAILURE);
        }
        else
            file_threads = atoi(&argv[2]);
    }
    else if (strncmp(argv, "-a", sizeof(char) * 2) == 0)
    {
        if (atoi(&argv[2]) <= 0)
        {
            fprintf(stderr, "Invalid number of threads: %s\n", argv);
            exit(EXIT_FAILURE);
        }
        else
            analysis_threads = atoi(&argv[2]);
    }
    else if (strncmp(argv, "-s", sizeof(char) * 2) == 0)
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

    queue_t file_q; // Create and initialize file/directory queues
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
            FindWFD(argv[i]);
            // add to file queue
            enqueue(&file_q, argv[i]);
            //start file threads
        }
        else
        {
        }
    }
    if (DEBUG)
    {
        printf("File Queue...\n");
        printQueue(&file_q);
        printf("Directory Queue...\n");
        printQueue(&directory_q);
    }

    if (DEBUG)
    {
        printf("dir threads: %d\nfile threads: %d\nanalysis threads: %d\noptional arguments: %d\n", directory_threads, file_threads, analysis_threads, opt_arg_count);
    }
    return EXIT_SUCCESS;
}