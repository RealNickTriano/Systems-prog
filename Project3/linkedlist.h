#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#ifndef DEBUG
#define DEBUG 0
#endif

typedef struct node_t{
    char *key;
    char *value;
    struct node_t* next;
}node_t;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

node_t* initNode(char *key, char *value)
{
    node_t *new_node = (node_t*)malloc(sizeof(node_t));
    new_node->key = key;
    new_node->value = value;
    new_node->next = NULL;
	//if(DEBUG)
	//printf("Initialized %s\n", w);
    pthread_mutex_unlock(&lock);
    return new_node;
}

node_t* insert(node_t *root, char *key, char *value)
{
    
    node_t* pre = initNode(key, value);
    if(root == NULL){
        root = pre;
	//if(DEBUG)
	//printf("Added NULL %s\n", w);
    pthread_mutex_unlock(&lock);
        return root;
    }
    pre->next = root;
    root = pre;
	//if(DEBUG)
	//printf("Added %s\n", w);
    pthread_mutex_unlock(&lock);
    return root;
}

node_t* add(node_t *root, char *key, char *value)
{   
    pthread_mutex_lock(&lock);
    if (root == NULL){
	node_t* new_node = initNode(key, value);
        root = new_node;
	//if(DEBUG)
	//printf("Added %s\n", w);
    pthread_mutex_unlock(&lock);
        return root;
    }

    else if(strcmp(key, root->key) == 0){
        strcpy(root->value, value);
        pthread_mutex_unlock(&lock);
        return root;
    }

    else if(strcmp(key, root->key) > 0){
            pthread_mutex_unlock(&lock);
            root->next = add(root->next, key, value);
    }
    else if(strcmp(key, root->key) < 0){
            return insert(root, key, value);
    }
    else{
	    if(DEBUG)
	    printf("DANGER\n");
        pthread_mutex_unlock(&lock);
        return root;
    }
}

char* find(node_t *root, char *key){
    pthread_mutex_lock(&lock);
    if (root == NULL){
        pthread_mutex_unlock(&lock);
        return NULL;
    }

    else if(strcmp(key, root->key) == 0){
        pthread_mutex_unlock(&lock);
        return root->value;
}
    else{
        if (root->next == NULL){
            pthread_mutex_unlock(&lock);
            return NULL;
        }
        find(root->next, key);
    }

}

node_t* del(node_t *root, char *key){
    pthread_mutex_lock(&lock);
    node_t *temp = root;
    node_t *pre = NULL;

    if (temp != NULL && strcmp(temp->key,key) == 0)
    {
        root = temp->next;
        printf("Removed key:value -> %s : %s\n", temp->key, temp->value);
        free(temp);
        pthread_mutex_unlock(&lock);
        return root;
    }
    else
    {
        while(temp != NULL && strcmp(temp->key,key) != 0)
        {
            pre = temp;
            temp = temp->next;
        }
   

        if (temp == NULL){
            pthread_mutex_unlock(&lock);
            return NULL;
        }
        printf("Removed key:value -> %s : %s\n", temp->key, temp->value);
        pre->next = temp->next;
        free(temp);
        pthread_mutex_unlock(&lock);
        return pre;

    }
    

}

void printList(node_t *root)
{
    if (root != NULL){
        printf("%s\t | %s\n", root->key, root->value);
        printList(root->next);
    }
    return;
}

void destroyList(node_t* root){
    pthread_mutex_lock(&lock);
    if (root != NULL){
                if (root->next != NULL){
                        destroyList(root->next);
                }
                free(root->key);
                free(root->value);
                free(root);
        }
    pthread_mutex_unlock(&lock);
}
