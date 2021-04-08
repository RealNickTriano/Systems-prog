#include <stdlib.h>
#include <stdio.h>

typedef struct {
    int value;
    char *word;
    struct node_t* next;
    struct node_t* prev;
}node_t;

node_t *head;

node_t *InitNode(int value)
{
    node_t *result = malloc(sizeof(node_t));
    result->value = value;
    result->next = NULL;
    result->prev = NULL;
    return result;
}

void InsertNodeAtHead(int value)
{
    node_t* new_node = InitNode(value);
    if(head == NULL)
    {
        head = new_node;
        return;
    }
    head->prev = new_node;
    new_node->next = head;
    head = new_node;
}

void InsertAtTail(int value)
{
    node_t* temp = head; // creat tempory node to start at head
    node_t* new_node = InitNode(value);
    if(head == NULL)
    {
        head = new_node;
        return;
    }
    while(temp->next != NULL) //traverse list to last node
    {
        temp = temp->next;  
        temp->next = new_node;
        new_node-> prev = temp;
    }
}

void PrintList()
{
    node_t* temp = head;
    while(temp->next != NULL)
    {
        printf("%d | ", temp->value);
        temp = temp->next;
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