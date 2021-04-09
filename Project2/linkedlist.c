#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct node_t{
    char *word;
    int count;
    struct node_t* next;
}node_t;

node_t* initNode(char* w)
{
    node_t *new = (node_t*)malloc(sizeof(node_t));
    new->word = w;
    new->count = 0;
    new->next = NULL;
    return new;
}

void insert(node_t *root, char* w)
{
    node_t* pre = initNode(w);
    if(root == NULL){
        root = new_node;
        return;
    }
    pre->next = root;
    root = pre;
}

void add(node_t *root, char* w)
{   
    node_t* new = initNode(w);
    if (root == NULL){
        root = new_node;
        return;
    }

    if(strcmp(w, root->word) == 0){
        root->count++;
        return;
    }

    if(strcmp(w, root->word) > 0){
                root->next = add(root->next, w);
    }
    if(strcmp(w, root->word) < 0){
            insert(root, w);
    }
    return;
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

void print(node_t *root)
{
    if (root != NULL){
        printf("%s | %d", root->word, root->count)
        print(root->next);
    }
    return;
}

void destroy(node_t* root){
    if (root != NULL){
                if (root->next != NULL){
                        freeAll(root->next);
                }
                free(root);
        }
}

int main()
{
    for (int i = 0; i < 25; i++)
    {
        InsertAtTail(i);
    }
    PrintList(); 
}