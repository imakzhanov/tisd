#define _POSIX_C_SOURCE 199309L

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include "array_stack.h"
#include "config.h"
#include "errors.h"
#include "list_stack.h"

stack_t array_stack;
list_stack_t list_stack;

double now_us(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1e6 + ts.tv_nsec / 1e3;
}

// n добавлений, затем n удалений в первый стек массива (который в начале)
int run_array(int n)
{
    double value;
    int rc = OK;

    stack_init(&array_stack);
    for (int i = 0; i < n && rc == OK; i++)
        rc = stack_push(&array_stack, 1, i);
    for (int i = 0; i < n && rc == OK; i++)
        rc = stack_pop(&array_stack, 1, &value);
    return rc;
}

// n добавлений, затем n удалений в стек список
int run_list(int n)
{
    double value;
    int rc = OK;

    list_stack_init(&list_stack, n, UINTPTR_MAX);
    for (int i = 0; i < n && rc == OK; i++)
        rc = list_stack_push(&list_stack, i);
    for (int i = 0; i < n && rc == OK; i++)
        rc = list_stack_pop(&list_stack, &value);
    return rc;
}

// Замеры повторяются пока погрешность среднего не станет мала
int measure(int method, int n, double *mean)
{
    int count = 0;
    double m2 = 0; // сумма квадратов отклонений замеров
    int rc = OK;
    double start, time, delta;

    *mean = 0;
    while (rc == OK && (count < MIN_SAMPLES || (count < MAX_SAMPLES && 2 * sqrt(m2 / (count - 1) / count) > MAX_ERROR * *mean)))
    {
        // 1 - массив, 2 - список
        if (method == 1)
        {
            start = now_us();
            for (int k = 0; k < BATCH_SIZE && rc == OK; k++)
                rc = run_array(n);
            time = (now_us() - start) / BATCH_SIZE;
        }
        else
        {
            start = now_us();
            for (int k = 0; k < BATCH_SIZE && rc == OK; k++)
                rc = run_list(n);
            time = (now_us() - start) / BATCH_SIZE;
        }

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

    printf("Время: n добавлений и n удалений, микросек. Память в байтах\n");
    printf("Массив имеет фиксированный размер на %d элементов\n\n", STACK_SIZE);
    printf("%8s | %15s %15s %7s | %12s %12s %7s |\n",
           "n", "array, microsec", "list, microsec", "t, %", "array, bytes", "list, bytes", "mem, %");

    for (size_t i = 0; i < sizeof(sizes) / sizeof(sizes[0]) && rc == OK; i++)
    {
        int n = sizes[i];
        if (n > STACK_SIZE)
            continue;

        rc = measure(1, n, &array_time);
        if (rc == OK)
            rc = measure(2, n, &list_time);
        if (rc != OK)
            break;

        size_t array_memory = sizeof(stack_t);
        size_t list_memory = sizeof(list_stack_t) + (size_t)n * sizeof(node_t);
        printf("%8d | %15.3f %15.3f %7.1f | %12zu %12zu %7.1f |\n", n, array_time, list_time, list_time / array_time * 100,
               array_memory, list_memory, (double)list_memory / array_memory * 100);
    }

    if (rc != OK)
        print_error(rc);
    return rc;
}
