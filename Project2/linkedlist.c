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
    node_t* new = initNode(w);
    if (root == NULL){
        root = new;
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

void print(node_t *root)
{
    if (root != NULL){
        printf("%s | %d\n", root->word, root->count);
        print(root->next);
    }
    return;
}

void destroy(node_t* root){
    if (root != NULL){
                if (root->next != NULL){
                        destroy(root->next);
                }
                free(root);
        }
}

int main()
{
    node_t *list;
    list = initNode("hello");
    list = add(list, "can");
    list = add(list, "i");
    list = add(list, "have");
    list = add(list, "a");
    list = add(list, "drink");
    list = add(list, "can");
    list =add(list, "have");
    print(list); 
}