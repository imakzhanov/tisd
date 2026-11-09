#ifndef ARR_QUEUE_H
#define ARR_QUEUE_H

#include <stddef.h>

#include "config.h"
#include "errors.h"

typedef struct
{
    double values[QUEUE_CAPACITY];
    size_t p_in;
    size_t p_out;
    size_t size;
} arr_queue_t;

int arr_push(arr_queue_t *queue, double value);
int arr_pop(arr_queue_t *queue, double *value);

#endif
