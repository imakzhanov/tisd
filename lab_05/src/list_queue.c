#include "list_queue.h"

#include <stdlib.h>
#include <stdio.h>

int list_push(list_queue_t *queue, double value)
{
    node_t *new = malloc(sizeof(node_t));
    if (!new)
        return MEMORY_ERROR;

    new->value = value;
    new->next = NULL;

    if (queue->size == 0)
        queue->p_out = new;
    else
        queue->p_in->next = new;

    queue->p_in = new;
    queue->size++;

    if (queue->show_addresses)
        printf("Добавление: %p\n", (void *)new);

    return OK;
}

int list_pop(list_queue_t *queue, double *value)
{
    if (queue->size == 0)
        return EMPTY_QUEUE;

    *value = queue->p_out->value;
    node_t *next = queue->p_out->next;

    if (queue->show_addresses)
        printf("Удаление:   %p\n", (void *)queue->p_out);
    free(queue->p_out);

    queue->p_out = next;
    if (!queue->p_out)
        queue->p_in = NULL;
    queue->size--;

    return OK;
}
