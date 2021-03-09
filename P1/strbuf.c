#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "strbuf.h"

#ifndef DEBUG
#define DEBUG 0
#endif


/*
void dump(strbuf_t *S){
        int i;
        printf("[%2lu/%2lu] ", S->used, S->length);
        for (i = 0; i < S->used; i++)
                printf("%02x ", S->data[i]);
        for (; i < S->length; i++)
                printf("_ ");
        printf("\n");
}

int main(int argc, char **argv){
        strbuf_t A;
        char c;
        sb_init(&A, 10);
        sb_append(&A, 'S');
        sb_append(&A, 'h');
        sb_append(&A, 'i');
        sb_append(&A, 'b');
        dump(&A);
        sb_remove(&A,&c);
        dump(&A);
        printf("%c\n", c);
        sb_append(&A,'h');
        sb_append(&A,'b');
        dump(&A);
        sb_insert(&A, 4, 'a');
        dump(&A);
        sb_concat(&A, "Sddque");
        dump(&A);
        sb_insert(&A,8,'i');
        sb_insert(&A,11,'i');
        dump(&A);
        sb_insert(&A,30,'y');
        dump(&A);
        sb_insert(&A,90,'z');
        dump(&A);
        sb_destroy(&A);

        return EXIT_SUCCESS;
}
*/

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
                char *p = realloc(S->data, sizeof(char) * size);
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
                        char *p = realloc(S->data, sizeof(char) * size);
                        if (!p) return 1;

                        S->data = p;
                        S->length = size;

                        if (DEBUG) printf("Increased size to %lu\n", size);
                }
                else{
                        size_t size = S->length * 2;
                        char *p = realloc(S->data, sizeof(char) * size);
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
                        char *p = realloc(S->data, sizeof(char) * size);
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
                dump(S);
        }
        return 0;
}