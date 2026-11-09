#include "arr_queue.h"

int arr_push(arr_queue_t *queue, double value)
{
    if (queue->size == QUEUE_CAPACITY)
        return OVERFLOW_QUEUE;

    queue->values[queue->p_in] = value;
    queue->p_in = (queue->p_in + 1) % QUEUE_CAPACITY;
    queue->size++;
    return OK;
}

int arr_pop(arr_queue_t *queue, double *value)
{
    if (queue->size == 0)
        return EMPTY_QUEUE;

    *value = queue->values[queue->p_out];
    queue->p_out = (queue->p_out + 1) % QUEUE_CAPACITY;
    queue->size--;
    return OK;
}
