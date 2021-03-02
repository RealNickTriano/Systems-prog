#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "strbuf.h"

#ifndef DEBUG
#define DEBUG 0
#endif

int sb_init(strbuf_t *L, size_t length)
{
    L->data = malloc(sizeof(char) * (length));                              // Allocates memmory based on input length
    if (!L->data) return 1;                                                 // if we couldnt allocate theres an issue

    L->length = length;                                                     // set the length of the list equal to input length
    L->used   = 1;                                                         // used equals zero because we are just initializing
    

    return 0;
}

void sb_destroy(strbuf_t *L)
{
    free(L->data);
}

int ExtendList(strbuf_t *list, int index)
{
    size_t size;
    if(index > (2 * list->length))                                          // if index is more than double the length of array
    {
        size = index;                                  
    }
    else if (index >= list->length)                                         // else if index is more than length (but, less than length*2)
    {
        size = (2 * list->length);                                          // set the size to double length
    }

    int *p = realloc(list->data, sizeof(char) * size);                      // increase memory size to double the original using realloc()
    if (!p) return 1;                                                       // if this fails return 1 for DEBUG

    list->data = p;                                                         // get that memory into the array                
	list->length = size;                                                    // set the new length
    if (DEBUG) printf("Increased size to %lu\n", size);
    return 0;
}

int ShiftList(strbuf_t *list, int index)                                    // Shifts the elements in the list to the right of index 1 space each
{
                                 
    int i; 
    int k;                                      
    for(i = (list->used+1); i >= index; i--)                            
    {
	k = list->data[list->used];
        memset(&list->data[i], list->data[i - 1], sizeof(char));        // copys each element to the next spot over starting with the last used value 
        k--;                                                      
    }
 	
        list->used++;
        return 0;
}
    
int sb_append(strbuf_t *L, char item)
{
    if ((L->used) == L->length)                                            // checks to see if arraylist is full
    {
	    ExtendList(L, L->used);                                            // Double Length
    }

    L->data[L->used - 1] = item;                                            // if its not full just add the item
    ++L->used;  
    if(item == '\0')
    {
	    --L->used;
    }                                 

    return 0;
}

int sb_remove(strbuf_t *L, char *item)
{
    if (L->used == 0) return 1;                                             // if theres nothing in the list we cannot remove anything

    --L->used;

    if (item) *item = L->data[L->used];                                     // Store that value
	sb_append(L,'\0');                           

    return 0;
}

int sb_insert(strbuf_t *list, int index, char item)
{
	ExtendList(list, index);                                               // Increase length if needed
    ShiftList(list,index);                                                  // Shift List over by 1
       
        
        
    if(list->used < index + 1)                                              // if index is more than used
    {
        list->used = index + 1;                                             // the index is now the last item so make that equal to used    
    }
    else
    {
        //Do Nothing
    }
        list->data[index - 1] = item;                                       // put the item at the respective index
	
        
        return 0; //Sucess!
}

int sb_concat(strbuf_t *sb, char *str)
{
    ++sb->used;
    int i;
    for (i = 0; str[i] != '\0'; i++)
    {
        sb_append(sb, str[i]);
    }
}
