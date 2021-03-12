#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "strbuf.h"

#ifndef DEBUG
#define DEBUG 0
#endif



int sb_init(strbuf_t *S, size_t length)
{
        S->data = malloc(sizeof(char) * length);
        if (!S->data) return 1;

        S->length = length;
        S->used   = 1;
        S->data[0] = '\0';

        return 0;
}

void sb_destroy(strbuf_t *S)
{
        free(S->data);
}

int sb_append(strbuf_t *S, char item)
{
        if (S->used >= S->length) {
                size_t size = S->length * 2;
                int *p = realloc(S->data, sizeof(char) * size);
                if (!p) return 1;

                S->data = p;
                S->length = size;

                if (DEBUG) printf("Increased size to %lu\n", size);
        }

        if (DEBUG) printf("Appeneding item %c \n", item);
        S->data[S->used - 1] = item;
        S->data[S->used] = '\0';
        ++S->used;

        return 0;
}


int sb_remove(strbuf_t *S, char *item)
{
        if (S->used == 0) return 1;

        --S->used;

        S->data[S->used] = S->data[S->used -1];
        S->data[S->used -1] = '\0';

        if (DEBUG) printf("Removed item %c \n", S->data[S->used]);

        if (item) *item = S->data[S->used];

        return 1;
}

int sb_insert(strbuf_t *S, int index, char item){
        if (DEBUG) printf("Inserting item %c at index %d \n", item, index);
        if (index + 1 >= S->length){
                if (index + 1 >= (S->length * 2)){
                        size_t size = index + 2;
                        int *p = realloc(S->data, sizeof(char) * size);
                        if (!p) return 1;
                        
                        S->data = p;
                        S->length = size;

                        if (DEBUG) printf("Increased size to %lu\n", size);
                }
                else{
                        size_t size = S->length * 2;
                        int *p = realloc(S->data, sizeof(char) * size);
                        if (!p) return 1;

                        S->data = p;
                        S->length = size;

                        if (DEBUG) printf("Increased size to %lu\n", size);
                }

                S->data[index] = item;
                S->data[index + 1] = '\0';
                S->used = index + 2;
                return 0;
        }
        else if (index >= S->used){
                S->data[index] = item;
                S->data[index + 1] = '\0';
                S->used = index + 2;

                return 0;
        }

        //if (index < S->used){
        else{
                if (S->used == S->length - 1){
                        size_t size = S->length * 2;
                        int *p = realloc(S->data, sizeof(char) * size);
                        if (!p) return 1;

                        S->data = p;
                        S->length = size;

                        if (DEBUG) printf("Increased size to %lu\n", size);
                }
                else{
                        for (int i = S->used; i > index; i--)
                                S->data[i] = S->data[i - 1];
                }
                S->data[index] = item;
                ++S->used;
        }

        return 0;
}

int sb_concat(strbuf_t *S, char *str){
        for (int i = 0; i < strlen(str); i++){
                sb_append(S, str[i]);
                
        }
        return 0;
}