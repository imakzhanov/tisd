#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "config.h"
#include "struct_func.h"
#include "errors.h"

typedef struct
{
    double standard_time;   // среднее время одного сложения, мкс
    double csc_time;
    size_t standard_memory; // память под A, B и результат, в байтах
    size_t csc_memory;
} bench_result_t;

static standard_t standard_a, standard_b;

static double now_us(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1e6 + ts.tv_nsec / 1e3;
}

static int bench_compare(const standard_t *a, const standard_t *b, const csc_t *csc_a, const csc_t *csc_b, bench_result_t *r)
{
    static standard_t res;
    csc_t cres;
    double start;
    int rc = OK;

    start = now_us();
    for (int k = 0; k < REPEATS && rc == OK; k++)
        rc = standard_add(a, b, &res);
    r->standard_time = (now_us() - start) / REPEATS;

    // время выделения памяти под результат входит в сложение CSC
    csc_init(&cres);
    start = now_us();
    for (int k = 0; k < REPEATS && rc == OK; k++)
    {
        csc_free(&cres);
        rc = csc_add(csc_a, csc_b, &cres);
    }
    r->csc_time = (now_us() - start) / REPEATS;

    r->standard_memory = standard_memory(a) + standard_memory(b) + standard_memory(&res);
    r->csc_memory = csc_memory(csc_a) + csc_memory(csc_b) + csc_memory(&cres);
    csc_free(&cres);
    return rc;
}

int main(void)
{
    const int sizes[] = BENCH_SIZES;
    const int percents[] = BENCH_PERCENTS;
    const int n_sizes = sizeof(sizes) / sizeof(sizes[0]);
    const int n_percents = sizeof(percents) / sizeof(percents[0]);
    bench_result_t r;
    int rc = OK;

    srand(1); // использую одинаковый сид чтобы замеры были на одних и тех же данных
    printf("Матрицы n x n со случайным заполнением; время - одно сложение, память - A, B и результат\n");

    printf("%6s | %4s | %14s %14s %20s | %14s %14s %20s |\n",
           "n", "%", "t standard, us", "t CSC, us", "t CSC/standard, %", "mem std, B", "mem CSC, B", "mem CSC/standard, %");

    for (int s = 0; s < n_sizes && rc == OK; s++)
    {
        for (int p = 0; p < n_percents && rc == OK; p++)
        {
            csc_t csc_a, csc_b;
            csc_init(&csc_a);
            csc_init(&csc_b);

            standard_fill_random(&standard_a, sizes[s], sizes[s], percents[p]);
            
            standard_fill_random(&standard_b, sizes[s], sizes[s], percents[p]);
            
            standard_to_csc(&standard_a, &csc_a);
            
            standard_to_csc(&standard_b, &csc_b);
            
            bench_compare(&standard_a, &standard_b, &csc_a, &csc_b, &r);
            if (rc == OK)
            {
                // время и память CSC в процентах от обычного способа
                double time_ratio = r.standard_time > 0 ? r.csc_time / r.standard_time * 100 : 0;
                double mem_ratio = r.standard_memory > 0 ? (double)r.csc_memory / r.standard_memory * 100 : 0;
                printf("%6d | %4d | %14.2f %14.2f %20.1f | %14zu %14zu %20.1f |\n",
                       sizes[s], percents[p], r.standard_time, r.csc_time, time_ratio,
                       r.standard_memory, r.csc_memory, mem_ratio);
            }

            csc_free(&csc_a);
            csc_free(&csc_b);
        }
        if (rc == OK)
            printf("-------------------------------------------------------------------------------------------------------------------------\n");
    }

    return rc;
}
