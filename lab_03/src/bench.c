#define _POSIX_C_SOURCE 199309L

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "config.h"
#include "struct_func.h"
#include "errors.h"

typedef struct
{
    double standard_time;   // среднее время одного сложения, в микросекундах
    double csc_time;
    size_t standard_memory; // память под A, B и результат, в байтах
    size_t csc_memory;
} bench_result_t;

static standard_t std_a, std_b, std_res;
static csc_t csc_a, csc_b, csc_res;

static double now_us(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1e6 + ts.tv_nsec / 1e3;
}

static int add_with_choose_type(int use_csc)
{
    if (!use_csc)
        return standard_add(&std_a, &std_b, &std_res);

    csc_free(&csc_res);
    return csc_add(&csc_a, &csc_b, &csc_res);
}

// среднее время одного сложения. замер - пачка из BATCH_SIZE сложений. Замеры повторяются, пока погрешность среднего не станет мала
static int measure(int use_csc, double *mean)
{
    int n = 0; // кол-во замеров
    double m2 = 0; // сумма квадратов отклонений замеров
    int rc = OK;

    *mean = 0;
    double start, time, delta;
    while (rc == OK && (n < MIN_SAMPLES || (n < MAX_SAMPLES && 2 * sqrt(m2 / (n - 1) / n) > MAX_ERROR * *mean)))
    {
        start = now_us();
        for (int k = 0; k < BATCH_SIZE && rc == OK; k++)
            rc = add_with_choose_type(use_csc);
        time = (now_us() - start) / BATCH_SIZE;

        n++;
        delta = time - *mean;
        *mean += delta / n;
        m2 += delta * (time - *mean);
    }
    return rc;
}

// две случайные матрицы n на n с заданным процентом заполнения, замер обоих способов сложения
static int bench_percent(int n, int percent, bench_result_t *r)
{
    int rc = OK;

    standard_fill_random(&std_a, n, n, percent);
    standard_fill_random(&std_b, n, n, percent);
    
    standard_to_csc(&std_a, &csc_a);
    standard_to_csc(&std_b, &csc_b);

    rc = measure(0, &r->standard_time);
    if (rc == OK)
        rc = measure(1, &r->csc_time);

    r->standard_memory = standard_memory(&std_a) + standard_memory(&std_b) + standard_memory(&std_res);
    r->csc_memory = csc_memory(&csc_a) + csc_memory(&csc_b) + csc_memory(&csc_res);
    csc_free(&csc_a);
    csc_free(&csc_b);
    csc_free(&csc_res);
    return rc;
}

// бинарный поиск граничного процента заполнения
static int find_border(int n, int low, int high, int *border)
{
    while (high > low + 1)
    {
        int mid = (low + high) / 2;
        bench_result_t r;
        int rc = bench_percent(n, mid, &r);

        if (rc != OK)
            return rc;
        if (r.csc_time > r.standard_time)
            high = mid;
        else
            low = mid;
    }
    *border = low;
    return OK;
}

int main(void)
{
    const int sizes[] = BENCH_SIZES;
    const int percents[] = BENCH_PERCENTS;
    const int n_sizes = sizeof(sizes) / sizeof(sizes[0]);
    const int n_percents = sizeof(percents) / sizeof(percents[0]);
    bench_result_t r;
    int low, high, border;
    int rc = OK;

    srand(1); // одинаковый сид, чтобы замеры были на одних и тех же данных
    csc_init(&csc_a);
    csc_init(&csc_b);
    csc_init(&csc_res);

    printf("Матрицы n на n со случайным заполнением; время - одно сложение, память - A, B и результат\n");
    printf("%6s | %4s | %14s %14s %7s | %14s %14s %7s |\n",
           "n", "%", "t standard, us", "t CSC, us", "t, %", "mem std, B", "mem CSC, B", "mem, %");

    for (int s = 0; s < n_sizes && rc == OK; s++)
    {
        low = 0;
        high = 100;
        for (int p = 0; p < n_percents && rc == OK; p++)
        {
            rc = bench_percent(sizes[s], percents[p], &r);
            if (rc == OK && r.csc_time > r.standard_time && high == 100)
                high = percents[p];
            else if (rc == OK && high == 100)
                low = percents[p];
            if (rc == OK)
            
                printf("%6d | %4d | %14.2f %14.2f %7.1f | %14zu %14zu %7.1f |\n", sizes[s], percents[p],
                       r.standard_time, r.csc_time, r.csc_time / r.standard_time * 100,
                       r.standard_memory, r.csc_memory, (double)r.csc_memory / r.standard_memory * 100);
        }
        if (rc == OK)
            rc = find_border(sizes[s], low, high, &border);
        if (rc == OK)
            rc = bench_percent(sizes[s], border, &r);
        if (rc == OK)
            printf("%6d | %4d | %14.2f %14.2f %7.1f | - процент при котором время обоих алгоритмов примерно равно\n", sizes[s], border,
                   r.standard_time, r.csc_time, r.csc_time / r.standard_time * 100);
        printf("-----------------------------------------------------------------------------------------------\n");
    }

    return rc;
}
