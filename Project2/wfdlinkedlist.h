#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"

// this list contains the file name and respective linked list of wfds
// 
typedef struct wfd_t {
    char *path;
    int word_count;
    struct node_t *list;
    struct wfd_t *next;
}wfd_t;

wfd_t* initNodeWFD(char *path, node_t *node)
{
    wfd_t *wfd_node = (wfd_t*)malloc(sizeof(wfd_t));
    wfd_node->path = path;
    wfd_node->list = node;
    wfd_node->word_count = 0;
    wfd_node->next = NULL;

    return wfd_t;
}

wfd_t* addNodeWFD(wfd_t *root, char *path, node_t *node)
{
    wfd_t *new_node = initNodeWFD(path, node);
    new_node->next = root;
    return new_node;
    
}

wfd_t* destroyListWFD(wfd_t *root)
{
    
        if (root->next != NULL){
            destroyList(root->next);
        }
        free(root);
}

wfd_t* printListWFD(wfd_t *root)
{
    do
    {
        printf("file name: %s\nword count: %d\n", root->path, root->word_count);
        printList(root->list);
    } while (root->next != NULL);
    
}







