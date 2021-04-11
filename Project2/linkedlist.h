#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct node_t
{
    char *word;
    float count;
    double frequency;
    struct node_t *next;
} node_t;

node_t *initNode(char *w)
{
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    new_node->word = w;
    new_node->count = 1;
    new_node->frequency = 0.0;
    new_node->next = NULL;
    return new_node;
}

node_t *insert(node_t *root, char *w)
{
    node_t *pre = initNode(w);
    if (root == NULL)
    {
        root = pre;
        return root;
    }
    pre->next = root;
    root = pre;
    return root;
}

node_t *add(node_t *root, char *w)
{
    node_t *new_node = initNode(w);
    if (root == NULL)
    {
        root = new_node;
        return root;
    }

    if (strcmp(w, root->word) == 0)
    {
        root->count++;
        return root;
    }

    if (strcmp(w, root->word) > 0)
    {
        root->next = add(root->next, w);
    }
    if (strcmp(w, root->word) < 0)
    {
        return insert(root, w);
    }
    return root;
}

node_t *find(node_t *root, char *w)
{
    if (root == NULL)
    {
        return NULL;
    }

    if (strcmp(w, root->word) == 0)
        return root;
    else
    {
        if (root->next == NULL)
            return NULL;
        find(root->next, w);
    }
}

void print(node_t *root)
{
    if (root != NULL)
    {
        printf("%s | %.1f\n", root->word, root->count);
        print(root->next);
    }
    return;
}

void destroyList(node_t *root)
{
    if (root != NULL)
    {
        if (root->next != NULL)
        {
            destroyList(root->next);
        }
        free(root);
    }
}
