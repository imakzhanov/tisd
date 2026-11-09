#ifndef MODEL_H
#define MODEL_H

#include <stddef.h>

#include "config.h"
#include "errors.h"

typedef struct
{
    double min;
    double max;
} range_t;

typedef struct
{
    range_t arrival[2];
    range_t service[2];
} params_t;

typedef enum
{
    QUEUE_ARRAY,
    QUEUE_LIST
} queue_kind_t;

typedef struct
{
    size_t arrived;
    size_t departed;
    size_t size;
    size_t started;
    double area;
    double wait;
} queue_stats_t;

typedef struct
{
    double time;
    double idle;
    queue_stats_t queue[2];
    size_t queue_memory;
} result_t;

typedef void (*progress_fn)(const result_t *);

extern const params_t default_params;

int simulate(const params_t *params, queue_kind_t kind, unsigned seed,
             int show_addresses, progress_fn progress, result_t *result);

#endif
