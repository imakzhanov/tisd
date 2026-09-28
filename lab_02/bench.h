#ifndef BENCH_H
#define BENCH_H

#include "struct_func.h"
#include "errors.h"

typedef struct {
    double time_ms;
    long memory_bytes;
} BenchResult;

/* Все bench_* возвращают OK и заполняют *out, либо ERR_EMPTY_TABLE
   (замер на пустой таблице не имеет смысла), либо ERR_MEMORY. */
int bench_sort_table_bubble(Table *t, BenchResult *out);
int bench_sort_table_quick(Table *t, BenchResult *out);
int bench_sort_keys_bubble(Table *t, BenchResult *out);
int bench_sort_keys_quick(Table *t, BenchResult *out);
int bench_lookup_via_keys(Table *t, BenchResult *out);

/* (value - base) / base * 100, в процентах относительно base — чистая
   математика, не имеет сценария отказа, поэтому остаётся double */
double relative_percent(double base, double value);

int print_bench_report(Table *t);
int save_bench_report(char *filename, Table *t);

#endif
