#define _POSIX_C_SOURCE 199309L

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include "array_stack.h"
#include "config.h"
#include "errors.h"
#include "list_stack.h"

typedef int (*run_t)(int n);

static stack_t array;
static list_stack_t list;

static double now_us(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1e6 + ts.tv_nsec / 1e3;
}

// n добавлений, затем n удалений в первый стек массива
static int run_array(int n)
{
    double value;
    int rc = OK;

    stack_init(&array);
    for (int i = 0; i < n && rc == OK; i++)
        rc = stack_push(&array, 1, i);
    for (int i = 0; i < n && rc == OK; i++)
        rc = stack_pop(&array, 1, &value);
    return rc;
}

// n добавлений, затем n удалений в стек-список
static int run_list(int n)
{
    double value;
    int rc = OK;

    list_stack_init(&list, n, UINTPTR_MAX);
    for (int i = 0; i < n && rc == OK; i++)
        rc = list_stack_push(&list, i);
    for (int i = 0; i < n && rc == OK; i++)
        rc = list_stack_pop(&list, &value);
    return rc;
}

// среднее время одного прогона. замер - пачка из BATCH_SIZE прогонов. Замеры повторяются, пока погрешность среднего не станет мала
static int measure(run_t run, int n, double *mean)
{
    int count = 0; // кол-во замеров
    double m2 = 0; // сумма квадратов отклонений замеров
    int rc = OK;
    double start, time, delta;

    *mean = 0;
    while (rc == OK && (count < MIN_SAMPLES || (count < MAX_SAMPLES && 2 * sqrt(m2 / (count - 1) / count) > MAX_ERROR * *mean)))
    {
        start = now_us();
        for (int k = 0; k < BATCH_SIZE && rc == OK; k++)
            rc = run(n);
        time = (now_us() - start) / BATCH_SIZE;

        count++;
        delta = time - *mean;
        *mean += delta / count;
        m2 += delta * (time - *mean);
    }
    return rc;
}

int main(void)
{
    int sizes[] = BENCH_SIZES;
    double array_time, list_time;
    int rc = OK;

    printf("Время: n добавлений и n удалений, мкс. Память: байт (без учета служебных данных malloc)\n");
    printf("Массив имеет фиксированный размер на %d элементов\n\n", STACK_SIZE);
    printf("%8s | %12s | %12s | %12s | %12s\n", "n", "массив, мкс", "список, мкс", "массив, байт", "список, байт");

    for (size_t i = 0; i < sizeof(sizes) / sizeof(sizes[0]) && rc == OK; i++)
    {
        int n = sizes[i];
        if (n > STACK_SIZE)
            continue;

        rc = measure(run_array, n, &array_time);
        if (rc == OK)
            rc = measure(run_list, n, &list_time);
        if (rc != OK)
            break;

        size_t array_memory = sizeof(stack_t);
        size_t list_memory = sizeof(list_stack_t) + (size_t)n * sizeof(node_t);
        printf("%8d | %12.3f | %12.3f | %12zu | %12zu\n", n, array_time, list_time, array_memory, list_memory);
    }

    if (rc != OK)
        print_error(rc);
    return rc;
}
