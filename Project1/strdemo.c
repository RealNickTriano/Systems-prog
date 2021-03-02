#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "strbuf.h"

void PrintList(strbuf_t *list)
{
    int i;
    printf("[%2lu/%2lu] ", list->used, list->length);
    for (i = 0; i < list->used; i++)
    {
        printf("%02x ", list->data[i]);
    }
    for (; i < list->length; i++)
    {
        printf("_ ");
    }
    putchar('\n');  
    
}

int  main(int argc, char **argv)
{
    char c;
    char* b;
    b = "hello";
    strbuf_t A;
    
    printf("Initializing...\n");
    sb_init(&A, 1);

    PrintList(&A);

    printf("Appending...\n");	
    sb_append(&A,'a');
    sb_append(&A,'b');
    sb_append(&A,'c');
    PrintList(&A);
    

    printf("Removing...\n");
    sb_remove(&A,&c);
    
    
    PrintList(&A);

    printf("Inserting 'd' at index 2\n");
    sb_insert(&A,2,'d');
    printf("Inserting 'e' at index 4\n");
    sb_insert(&A,4,'e');
    

    PrintList(&A);

    printf("Concating 'hello'\n");
    sb_concat(&A, b);

    PrintList(&A);

    printf("DESTROYING...\n");
    sb_destroy(&A);

}