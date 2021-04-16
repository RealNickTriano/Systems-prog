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
#include <pthread.h> // -pthread when compiling
#include <math.h>   // -lm when compiling
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
pthread_mutex_t lock;
wfd_t *wfd_repo;
int number_of_comparisons = 0;

/*typedef struct jsds
{
    char *pair_path; // names of pair of files
    float JSD;   // JSD calculated for the pair
    int combined_word_count; // combined TOTAL word count
};*/

struct jsds *JSD_struct;

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




struct targs
{
    queue_t *Q;
};

void* FindWFD(void *A)
{
    int bytes_read, input_fd;
    
    float wfd = 0.0;
    char buf[BUFSIZE];
    char character; 
    //char adjacent_character;
    int found_end = 0; //A boolean to check if we found the end of a word
    int word_len = 0;
    int word_start = 0; //The start position of a word
    //int word_end = 0;   //The end position of a word
    char *word;
	struct targs *args = A;
    node_t *list;
    strbuf_t str;
    
	sleep(1);
while((args->Q)->count != 0)
{
char *path;
	path = dequeue(args->Q, path);
        //sb_init(&str, BUFSIZE);
    float words_found = 0;
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
		if(word_start == 0)
		{
			sb_init(&str, BUFSIZE);
			word_start = 1;

		}
                
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
                //sb_init(&str, BUFSIZE);

                /* if(DEBUG)
                {
                    printf("%s\n", word);
                }*/
                // now we can add the word to linked list
                if (words_found == 1)
                {
                    list = initNode(word, 0.0);
                }
                else
                {
                    list = add(list, word, 0.0);
                }
		
            }
        }//sb_destroy(&str);
    }

    // done reading file
    if (DEBUG)
    {	pthread_mutex_lock(&lock);
	printf("FILE: %s\n", path);
        printList(list);
	pthread_mutex_unlock(&lock);
    }
	
    // now calculate WFD of file
    node_t *temp = list;
    while (temp != NULL)
    {
        wfd = (temp->count / words_found);
        temp->frequency = wfd;
        temp = temp->next;
    }
    free(temp);
    //destroyList(list);
    // add to WFD repo
	files++;
    if(files == 1) // if its the first file
    {
        wfd_repo = initNodeWFD(path, list, words_found);
    }
    else 
    {
        wfd_repo = addNodeWFD(wfd_repo, path, list, words_found);
    }
	}
    
}

node_t* computeKLD(node_t *file1, node_t *file2)
{
    int one_freq = 0;
    double mean, freq1, freq2;
    node_t *mean_list = NULL;
    node_t *tempfile2 = file2;
    node_t *tempfile1 = file1;

    while (tempfile1 != NULL && tempfile2 != NULL)
    {
        if(strcmp(tempfile1->word, tempfile2->word) == 0)
        {
            //two words match move both pointers
            //set frequencys accordingly
            freq1 = tempfile1->frequency;
            freq2 = tempfile2->frequency;
            mean = (0.5)*(freq1 + freq2);
            mean_list = add(mean_list, tempfile1->word, mean);
            if(DEBUG)
                printf("mean frequency for %s is %f\n", tempfile1->word, mean);
            tempfile1 = tempfile1->next;
            tempfile2 = tempfile2->next;
            
        }

        else if(strcmp(tempfile1->word, tempfile2->word) > 0)
        {
            // word in file 2 comes before file 1 move file 2 pointer
            //set frequencys accordingly
            freq1 = 0;
            freq2 = tempfile2->frequency;
            mean = (0.5)*(freq1 + freq2);
            mean_list = add(mean_list, tempfile2->word, mean);
            if(DEBUG)
                printf("mean frequency for %s is %f\n", tempfile1->word, mean);
            tempfile2 = tempfile2->next;
        }

        else if(strcmp(tempfile1->word, tempfile2->word) < 0)
        {
            // word in file 1 comes before file 2 move file 1 pointer
            //set frequencys accordingly
            freq1 = tempfile1->frequency;
            freq2 = 0;
            mean = (0.5) * (freq1 + freq2);
            mean_list = add(mean_list, tempfile1->word, mean);
            if(DEBUG)
                printf("mean frequency for %s is %f\n", tempfile1->word, mean);
            tempfile1 = tempfile1->next;
        }
        else
        {
            if(DEBUG)
                printf("Error in computeKLD\n");
        }
    }
    if(tempfile1 == NULL)
    {
        while(tempfile2 != NULL)
        {
            // rest of words in tempfile2 have no match 
            freq1 = 0;
            freq2 = tempfile2->frequency;
            mean = (0.5)*(freq1 + freq2);
            mean_list = add(mean_list, tempfile2->word, mean);
            if(DEBUG)
                printf("mean frequency for %s is %f\n", tempfile2->word, mean);
            tempfile2 = tempfile2->next;
        }
    }
    else if(tempfile2 == NULL)
    {
        while(tempfile1 != NULL)
        {
            // rest of words in file1 have no match 
            freq1 = tempfile1->frequency;
            freq2 = 0;
            mean = (1/2)*(freq1 + freq2);
            mean_list = add(mean_list, tempfile1->word, mean);
            if(DEBUG)
                printf("mean frequency for %s is %f\n", tempfile1->word, mean);
            tempfile1 = tempfile1->next;
        }
    }
    return mean_list;
}
void computeJSD()
{
    
    char *file_pair_name;
    node_t *file1, *file2;
    node_t *mean_files = NULL;
    wfd_t *temp_repo = wfd_repo;
    
    number_of_comparisons = (0.5) * files * (files-1);
    //JSD_struct = (jsds*)malloc(sizeof(jsds) * number_of_comparisons);
    // compute KLD for each file
    //while (wfd_repo != NULL)
    //{
    
        file1 = wfd_repo->list; 
        file2 = (wfd_repo->next)->list;

        mean_files = computeKLD(file1, file2);
        node_t *temp_mean_files = mean_files;
        float kld;
        while(file1 != NULL || temp_mean_files != NULL)
        {
            if(strcmp(file1->word, temp_mean_files->word) == 0)
            {
                kld += (file1->frequency * log2((file1->frequency / temp_mean_files->frequency)));
                file1 = file1->next;
                temp_mean_files = temp_mean_files->next;
            }
            else if(strcmp(file1->word, temp_mean_files->word) > 0)
            {
                temp_mean_files = temp_mean_files->next;
            }
            else if(strcmp(file1->word, temp_mean_files->word) < 0)
            {
                file1 = file1->next;
            }
        }
        wfd_repo->kld = kld;
        //wfd_repo = wfd_repo->next;
    //}
    /*while(wfd_repo != NULL)
    {
        jsds[i]->JSD
    }*/

	printf("MEAN FILE...\n");
    printList(mean_files);
    printListWFD(wfd_repo);

//free(file1);
//free(file2);
//free(mean_file);

    // compute JSD for each file pair
	return;
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
int main(int argc, char **argv)
{
    int opt_arg_count = 0;
    struct targs *args;
	int err;
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

    args = malloc(file_threads * sizeof(struct targs));
    tids = malloc(file_threads * sizeof(pthread_t));
    pthread_mutex_init(&lock, NULL);

    // start threads here:

    for (int i = 0; i < file_threads; i++)
    {
        args[i].Q = &file_q;
        err = pthread_create(&tids[i], NULL, FindWFD, &args[i]);
        if (err != 0)
        {
            perror("creation failed");
            return EXIT_FAILURE;
        }
    }

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

    /*while(file_q.count != 0)
    {
        char *path;
        path = dequeue(&file_q, path);
        if (path == NULL)
        {
            return 0;
        }
        FindWFD(path);
    }*/

for (int i = 0; i < file_threads; i++)
    {
        pthread_join(tids[i], NULL);
    }

    

	if(DEBUG)
	{
		printf("printing repo...\n\n");
		printListWFD(wfd_repo);
	}
    
		computeJSD();
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

    pthread_mutex_destroy(&lock);
    destroy(&file_q);
    free(tids);
    free(args);
    return EXIT_SUCCESS;
}
