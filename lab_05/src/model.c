#include "model.h"

#include <math.h>
#include <stdlib.h>

#include "arr_queue.h"
#include "list_queue.h"

const params_t default_params = {
    .arrival = {{1, 5}, {0, 3}},
    .service = {{0, 4}, {0, 1}}
};

static double random_time(range_t range)
{
    double fraction = (rand() + 0.5) / (RAND_MAX + 1.0);
    return range.min + (range.max - range.min) * fraction;
}

static int valid_range(range_t range)
{
    return isfinite(range.min) && isfinite(range.max)
        && range.min >= 0 && range.min <= range.max;
}

static int run(const params_t *params, arr_queue_t *array, list_queue_t *list,
               progress_fn progress, result_t *result)
{
    double arrival[2] = {
        random_time(params->arrival[0]), random_time(params->arrival[1])
    };
    double finish = INFINITY;
    int active = -1;
    int rc = OK;
    result->queue_memory = array ? 2 * sizeof(*array) : 2 * sizeof(*list);

    while (result->queue[0].departed < TARGET)
    {
        double now = fmin(fmin(arrival[0], arrival[1]), finish);
        double dt = now - result->time;
        for (int i = 0; i < 2; i++)
            result->queue[i].area += result->queue[i].size * dt;
        if (active < 0)
            result->idle += dt;
        result->time = now;

        for (int i = 0; i < 2; i++)
        {
            if (arrival[i] != now)
                continue;
            rc = array ? arr_push(&array[i], now) : list_push(&list[i], now);
            if (rc != OK)
                return rc;
            result->queue[i].arrived++;
            result->queue[i].size++;
            arrival[i] = now + random_time(params->arrival[i]);
            if (!isfinite(arrival[i]) || arrival[i] <= now)
                return TIME_ERROR;
        }

        if (list)
        {
            size_t memory = 2 * sizeof(*list) + (list[0].size + list[1].size) * sizeof(node_t);
            if (memory > result->queue_memory)
                result->queue_memory = memory;
        }

        int report = 0;
        if (finish == now)
        {
            result->queue[active].departed++;
            report = active == 0 && result->queue[0].departed % REPORT_STEP == 0;
            active = -1;
            finish = INFINITY;
        }

        if (active < 0 && result->queue[0].departed < TARGET)
        {
            // Приоритет первого типа учитывается только при выборе новой заявки.
            for (int i = 0; i < 2; i++)
            {
                queue_stats_t *stats = &result->queue[i];
                if (stats->size == 0)
                    continue;

                double entered;
                rc = array ? arr_pop(&array[i], &entered) : list_pop(&list[i], &entered);
                if (rc != OK)
                    return rc;
                stats->size--;
                stats->started++;
                stats->wait += now - entered;
                active = i;
                finish = now + random_time(params->service[i]);
                if (!isfinite(finish))
                    return TIME_ERROR;
                break;
            }
        }
        if (report && progress)
            progress(result);
    }
    return OK;
}

static int run_array(const params_t *params, progress_fn progress, result_t *result)
{
    arr_queue_t queues[2] = {0};
    return run(params, queues, NULL, progress, result);
}

int simulate(const params_t *params, queue_kind_t kind, unsigned seed,
             int show_addresses, progress_fn progress, result_t *result)
{
    for (int i = 0; i < 2; i++)
        if (!valid_range(params->arrival[i]) || params->arrival[i].max == 0
            || !valid_range(params->service[i]))
            return INPUT_ERROR;

    *result = (result_t){0};
    srand(seed);
    if (kind == QUEUE_ARRAY)
        return run_array(params, progress, result);

    list_queue_t queues[2] = {0};
    for (int i = 0; i < 2; i++)
        queues[i].show_addresses = show_addresses;

    int rc = run(params, NULL, queues, progress, result);
    for (int i = 0; i < 2; i++)
    {
        double unused;
        queues[i].show_addresses = 0;
        while (queues[i].size)
            list_pop(&queues[i], &unused);
    }
    return rc;
}
