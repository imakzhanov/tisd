#include "io.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

static int measure(const params_t *params, queue_kind_t kind)
{
    result_t result;
    clock_t start = clock();
    for (int i = 0; i < BENCH_RUNS; i++)
    {
        int rc = simulate(params, kind, MODEL_SEED, 0, NULL, &result);
        if (rc != OK)
            return rc;
    }
    double us = (double)(clock() - start) * 1e6 / CLOCKS_PER_SEC / BENCH_RUNS;
    printf("%-10s %12.3f %14zu\n",
           kind == QUEUE_ARRAY ? "array" : "list", us, result.queue_memory);
    return OK;
}

int main(int argc, char **argv)
{
    params_t params = default_params;
    int rc = OK;
    if (argc == 2 && strcmp(argv[1], "-p") == 0)
        rc = read_params(&params);
    else if (argc != 1)
    {
        fprintf(stderr, "Запуск: ./bench.exe [-p]\n");
        return INPUT_ERROR;
    }

    if (rc == OK)
    {
        printf("Среднее время прогона за %d повторов и пиковая память очередей\n", BENCH_RUNS);
        printf("%-10s %12s %14s\n", "queue", "time, us", "memory, bytes");
        rc = measure(&params, QUEUE_ARRAY);
        if (rc == OK)
            rc = measure(&params, QUEUE_LIST);
    }
    if (rc != OK)
        print_error(rc);
    return rc;
}
