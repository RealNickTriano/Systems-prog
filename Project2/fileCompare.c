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
#include <math.h>
#include "strbuf.h"
#include "fileQueue.h"
#include "linkedlist.h"


#ifndef DEBUG
#define DEBUG 0
#endif

#ifndef BUFSIZE
#define BUFSIZE 128
#endif

int directory_threads = 1, file_threads = 1, analysis_threads = 1;
char file_name_suffix[] = ".txt";
int files = 0;
wfd_t *wfd_repo;

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

int FindWFD(char *path)
{
    int bytes_read, input_fd;
    float words_found = 0;
    float wfd = 0.0;
    char buf[BUFSIZE];
    char character; 
    //char adjacent_character;
    int found_end = 0; //A boolean to check if we found the end of a word
    int word_len = 0;
    int word_start = 0; //The start position of a word
    //int word_end = 0;   //The end position of a word
    char *word;
    node_t *list;
    strbuf_t str;
    sb_init(&str, BUFSIZE);

    files++;
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

            if (isalpha(character) != 0 || isdigit(character) != 0 || character == '-')
            {
                word_start = 1;
                sb_append(&str, character);
                word_len++;
            }
            else if (isspace(character) != 0)
            {
                if (word_start == 1)
                {
                    found_end = 1;
                    word_start = 0;
                }
                words_found++;
            }

            if (found_end == 1)
            {
                found_end = 0; // reset found_end
                word = malloc(sizeof(char) * word_len);
		if(word == NULL)
		{
			perror("malloc: ");
		}
                //strncpy(word, &sb_word(str, word_len), word_len);
		sb_word(&str, word_len, word);
                sb_destroy(&str);
                sb_init(&str, BUFSIZE);

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
        }sb_destroy(&str);
    }
    // done reading file
    if (DEBUG)
    {
        printList(list);
    }
	
    // now calculate WFD of file
    node_t *temp = list;
    while (temp != NULL)
    {
        list = list->next;
    }
    free(temp);
    //destroyList(list);
    // add to WFD repo
    if(files == 1) // if its the first file
    {
        wfd_repo = initNodeWFD(path, list, words_found);
    }
    else 
    {
        wfd_repo = addNodeWFD(wfd_repo, path, list, words_found);
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
        int len = strlen(argv) - 2;
        strncpy(file_name_suffix, argv + 2, len);
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

/*int inList(node_t *list1, node_t *list2) // check if word in list 1 is in list 2 RETURNS 1 if item is in list 2 and 0 if not
{
    node_t *temp;
    temp = list2;

    while(list1 != NULL)
    {
        while(temp != NULL) //look through file 2 word list to see if word from file 1 is in file 2
        {
            if(strcmp(list1->word, temp->word) == 0) // word is in list 2
            {
                return 1;
            }
            else
            {
                temp = temp->next;
            }
        }
    }
	free (temp);
    return 0;
}
node_t* computeKLD(node_t *file1, node_t *file2, node_t *mean_file)
{
    int one_freq = 0;
    double mean_frequency, freq1, freq2;
    node_t *temp;
    temp = file2;

    while(file1 != NULL)
    {
        if(mean_file == NULL || inList(file1, mean_file) == 1) // if we already checked the word go next
        {
            file1 = file1->next;
        }
        else
        {
            freq1 = file1->frequency;

            while(temp != NULL) //look through file 2 word list to see if word from file 1 is in file 2
            {
                if(strcmp(file1->word, temp->word) == 0) // word is in file 2 as well
                {
                    freq2 = temp->frequency;
                    break;
                }
                else
                {
                    temp = temp->next;
                    freq2 = 0;  // freq2 will be 0 if word is never found in file 2
                }
            }

            one_freq = 1;

            //calculate mean frequency
            // add to linked list
            if(one_freq == 1)
            {
                mean_file  = initNode(file1->word);
            }
            else
            {
                mean_file = add(mean_file, file1->word);
            }

            mean_file->mean_frequency = (1/2) * (freq1 + freq2); // set mean freq of word
            file1 = file1->next; // go to next word and repeat proccess
        }
    }
free(temp);
    return mean_file;
}
void computeJSD()
{
    
    char *file_pair_name;
    node_t *file1, *file2;
    node_t *mean_file = NULL;

    
    // compute KLD for each file
    file1 = wfd_repo->list; 
    file2 = (wfd_repo->next)->list;

    mean_file = computeKLD(file1, file2, mean_file);
    mean_file =computeKLD(file2, file1, mean_file);

	printf("MEAN FILE...\n");
    printList(mean_file);

//free(file1);
//free(file2);
free(mean_file);
    

    
    // compute JSD for each file pair
	return;
}*/
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

    // start threads here:
    for (int i = 1; i < argc; i++)
    {
        if (is_directory(argv[i]) != 0) // found a directory
        {
            // add to directory queue
            enqueue(&directory_q, argv[i]);
        }
        else if (is_file(argv[i]) != 0) // found a file
        {
            // add to file queue
            enqueue(&file_q, argv[i]);
        }
        else
        {
        }
    }

    while(file_q.count != 0)
    {
        char *path;
        path = dequeue(&file_q, path);
        if (path == NULL)
        {
            return 0;
        }
        FindWFD(path);
    }
	//computeJSD();
	

	if(DEBUG)
	{
		printf("printing repo...\n\n");
		printListWFD(wfd_repo);
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