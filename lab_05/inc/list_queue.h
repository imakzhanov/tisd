#ifndef LIST_QUEUE_H
#define LIST_QUEUE_H

#include <stddef.h>

#include "errors.h"

typedef struct node
{
    double value;
    struct node *next;
} node_t;

typedef struct
{
    node_t *p_in;
    node_t *p_out;
    size_t size;
    int show_addresses;
} list_queue_t;

int list_push(list_queue_t *queue, double value);
int list_pop(list_queue_t *queue, double *value);
void list_clear(list_queue_t *queue);

#endif
