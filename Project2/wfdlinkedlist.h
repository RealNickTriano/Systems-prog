#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"

typedef struct wfd_node_t {
    char *path;
    node_t *root;
    wfd_node_t *next;
}wfd_node_t;



