#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct node_t{
    char *word;
    float count;
    double frequency;
	double mean_frequency;
    struct node_t* next;
}node_t;

node_t* initNode(char* w)
{
    node_t *new_node = (node_t*)malloc(sizeof(node_t));
    new_node->word = w;
    new_node->count = 1;
    new_node->frequency = 0.0;
    new_node->mean_frequency = 0.0;
    new_node->next = NULL;
    return new_node;
}

node_t* insert(node_t *root, char* w)
{
    node_t* pre = initNode(w);
    if(root == NULL){
        root = pre;
        return root;
    }
    pre->next = root;
    root = pre;
    return root;
}

node_t* add(node_t *root, char* w)
{   
    node_t* new_node = initNode(w);
    if (root == NULL){
        root = new_node;
        return root;
    }

    if(strcmp(w, root->word) == 0){
        root->count++;
        return root;
    }

    if(strcmp(w, root->word) > 0){
            root->next = add(root->next, w);
    }
    if(strcmp(w, root->word) < 0){
            return insert(root, w);
    }
    return root;
}

node_t* find(node_t *root, char* w){
    if (root == NULL){
        return NULL;
    }

    if(strcmp(w, root->word) == 0)
        return root;
    else{
        if (root->next == NULL)
            return NULL;
        find(root->next, w);
    }
}

void printList(node_t *root)
{
    if (root != NULL){
        printf("%s | %.1f | %.2f | %f\n", root->word, root->count, root->frequency, root->mean_frequency);
        printList(root->next);
    }
    return;
}

void destroyList(node_t* root){
    if (root != NULL){
                if (root->next != NULL){
                        destroyList(root->next);
                }
                free(root);
        }
}

typedef struct wfd_t {
    char *path;
    int word_count;
    double kld;
    struct node_t *list;
    struct wfd_t *next;
}wfd_t;

wfd_t* initNodeWFD(char *path, node_t *node, int words_found)
{
    wfd_t *wfd_node = (wfd_t*)malloc(sizeof(wfd_t));
    wfd_node->path = path;
    wfd_node->list = node;
    wfd_node->kld = 0;
    wfd_node->word_count = words_found;
    wfd_node->next = NULL;

    return wfd_node;
}

wfd_t* addNodeWFD(wfd_t *root, char *path, node_t *node, int words_found)
{
    wfd_t *new_node = initNodeWFD(path, node, words_found);
    new_node->next = root;
    return new_node;
    
}

void destroyListWFD(wfd_t *root)
{
    
        if (root->next != NULL){
            destroyListWFD(root->next);
        }
        free(root);
}

void printListWFD(wfd_t *root)
{
    if(root!= NULL)
    {
        printf("file name: %s\nword count: %d\nKLD: %f\n\n", root->path, root->word_count, root->kld);
        printList(root->list);
	printListWFD(root->next);
    }
	return;
    
}

