#include "io.h"

#include <stdio.h>
#include <time.h>

#include "arr_queue.h"
#include "list_queue.h"

static int measure_queue(queue_kind_t kind, int n, double *mean)
{
    arr_queue_t array = {0};
    list_queue_t list = {0};
    int rc = OK;
    double value;
    clock_t start = clock();
    for (int k = 0; k < QUEUE_BENCH_RUNS && rc == OK; k++)
    {
        for (int i = 0; i < n && rc == OK; i++)
            rc = kind == QUEUE_ARRAY ? arr_push(&array, i) : list_push(&list, i);
        for (int i = 0; i < n && rc == OK; i++)
            rc = kind == QUEUE_ARRAY ? arr_pop(&array, &value) : list_pop(&list, &value);
    }
    *mean = (double)(clock() - start) * 1e6 / CLOCKS_PER_SEC / QUEUE_BENCH_RUNS;
    list_clear(&list);
    return rc;
}

static int bench_queues(void)
{
    int sizes[] = {100, 500, 1000, 2000, QUEUE_CAPACITY};
    printf("\nОчередь без ОА: n добавлений, затем n удалений, %d повторов\n", QUEUE_BENCH_RUNS);
    printf("Проценты списка относительно массива (массив = 100%%)\n");
    printf("%6s %12s %12s %8s %12s %12s %8s\n",
           "n", "array, us", "list, us", "time, %", "array, bytes", "list, bytes", "mem, %");
    for (size_t i = 0; i < sizeof(sizes) / sizeof(sizes[0]); i++)
    {
        int n = sizes[i];
        if (n > QUEUE_CAPACITY)
            continue;
        double array_time, list_time;
        int rc = measure_queue(QUEUE_ARRAY, n, &array_time);
        if (rc == OK)
            rc = measure_queue(QUEUE_LIST, n, &list_time);
        if (rc != OK)
            return rc;

        size_t array_memory = sizeof(arr_queue_t);
        size_t list_memory = sizeof(list_queue_t) + (size_t)n * sizeof(node_t);
        printf("%6d %12.3f %12.3f ", n, array_time, list_time);
        if (array_time > 0)
            printf("%8.1f", list_time / array_time * 100);
        else
            printf("%8s", "n/a");
        printf(" %12zu %12zu %8.1f\n", array_memory, list_memory,
               (double)list_memory / array_memory * 100);
    }
    return OK;
}

static int measure(const params_t *params, queue_kind_t kind, unsigned seed,
                   double *mean, size_t *memory)
{
    result_t result;
    clock_t start = clock();
    for (int i = 0; i < BENCH_RUNS; i++)
    {
        int rc = simulate(params, kind, seed, 0, NULL, &result);
        if (rc != OK)
            return rc;
    }
    *mean = (double)(clock() - start) * 1e6 / CLOCKS_PER_SEC / BENCH_RUNS;
    *memory = result.queue_memory;
    printf("%-10s %12.3f %14zu\n",
           kind == QUEUE_ARRAY ? "array" : "list", *mean, *memory);
    return OK;
}

int main(void)
{
    unsigned seed = (unsigned)time(NULL);
    double array_time, list_time;
    size_t array_memory, list_memory;
    printf("Среднее время прогона за %d повторов и пиковая память очередей\n", BENCH_RUNS);
    printf("%-10s %12s %14s\n", "queue", "time, us", "memory, bytes");
    int rc = measure(&default_params, QUEUE_ARRAY, seed, &array_time, &array_memory);
    if (rc == OK)
        rc = measure(&default_params, QUEUE_LIST, seed, &list_time, &list_memory);
    if (rc == OK)
    {
        printf("Список относительно массива (массив = 100%%): время ");
        if (array_time > 0)
            printf("%.1f%%", list_time / array_time * 100);
        else
            printf("не определено");
        printf(", память %.1f%%\n", (double)list_memory / array_memory * 100);
    }
    if (rc == OK)
        rc = bench_queues();
    if (rc != OK)
        print_error(rc);
    return rc;
}
