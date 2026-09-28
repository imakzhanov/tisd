#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bench.h"
#include "sort.h"

static int copy_table(Table *src, Table *dst)
{
    table_init(dst);
    int err;
    for (int i = 0; i < src->count; i++)
    {
        err = table_add(dst, src->data[i]);
        if (err != OK)
        {
            table_free(dst);
            return err;
        }
    }
    return OK;
}

int bench_sort_table_bubble(Table *t, BenchResult *out)
{
    if (t->count == 0)
        return ERR_EMPTY_TABLE;

    Table copy;
    int err = copy_table(t, &copy);
    if (err != OK)
        return err;

    clock_t start = clock();
    sort_bubble_table(&copy);
    clock_t end = clock();

    out->time_ms = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
    out->memory_bytes = (long)sizeof(Flat) * t->count;

    table_free(&copy);
    return OK;
}

int bench_sort_table_quick(Table *t, BenchResult *out)
{
    if (t->count == 0)
        return ERR_EMPTY_TABLE;

    Table copy;
    int err = copy_table(t, &copy);
    if (err != OK)
        return err;

    clock_t start = clock();
    sort_quick_table(&copy);
    clock_t end = clock();

    out->time_ms = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
    out->memory_bytes = (long)sizeof(Flat) * t->count;

    table_free(&copy);
    return OK;
}

int bench_sort_keys_bubble(Table *t, BenchResult *out)
{
    if (t->count == 0)
        return ERR_EMPTY_TABLE;

    KeyTable kt;
    int err = build_key_table(t, &kt);
    if (err != OK)
        return err;

    clock_t start = clock();
    sort_bubble_keys(&kt);
    clock_t end = clock();

    out->time_ms = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
    out->memory_bytes = (long)sizeof(KeyEntry) * t->count;

    key_table_free(&kt);
    return OK;
}

int bench_sort_keys_quick(Table *t, BenchResult *out)
{
    if (t->count == 0)
        return ERR_EMPTY_TABLE;

    KeyTable kt;
    int err = build_key_table(t, &kt);
    if (err != OK)
        return err;

    clock_t start = clock();
    sort_quick_keys(&kt);
    clock_t end = clock();

    out->time_ms = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
    out->memory_bytes = (long)sizeof(KeyEntry) * t->count;

    key_table_free(&kt);
    return OK;
}

int bench_lookup_via_keys(Table *t, BenchResult *out)
{
    if (t->count == 0)
        return ERR_EMPTY_TABLE;

    KeyTable kt;
    int err = build_key_table(t, &kt);
    if (err != OK)
        return err;
    sort_quick_keys(&kt);

    volatile double sink = 0.0;
    clock_t start = clock();
    for (int i = 0; i < kt.count; i++)
        sink += t->data[kt.items[i].index].price_m2;
    clock_t end = clock();

    out->time_ms = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
    out->memory_bytes = (long)sizeof(KeyEntry) * t->count;

    key_table_free(&kt);
    return OK;
}

double relative_percent(double base, double value)
{
    if (base == 0.0)
        return 0.0;
    return (value - base) / base * 100.0;
}

static int write_report(FILE *out_stream, Table *t)
{
    BenchResult ins_table, ins_keys, qk_table, qk_keys, lookup;

    int err;
    if ((err = bench_sort_table_bubble(t, &ins_table)) != OK)
        return err;
    if ((err = bench_sort_keys_bubble(t, &ins_keys)) != OK)
        return err;
    if ((err = bench_sort_table_quick(t, &qk_table)) != OK)
        return err;
    if ((err = bench_sort_keys_quick(t, &qk_keys)) != OK)
        return err;
    if ((err = bench_lookup_via_keys(t, &lookup)) != OK)
        return err;

    fprintf(out_stream, "Отчёт по эффективности (размер таблицы: %d записей)\n\n", t->count);
    fprintf(out_stream, "%-35s %10s %14s %12s\n", "Метод", "Время,мс", "Память,байт", "% к таблице");
    fprintf(out_stream, "%-35s %10.4f %14ld %12s\n", "Пузырёк, сама таблица (а)",
            ins_table.time_ms, ins_table.memory_bytes, "база");
    fprintf(out_stream, "%-35s %10.4f %14ld %12.1f\n", "Пузырёк, таблица ключей (б)",
            ins_keys.time_ms, ins_keys.memory_bytes,
            relative_percent(ins_table.time_ms, ins_keys.time_ms));
    fprintf(out_stream, "%-35s %10.4f %14ld %12s\n", "Быстрая, сама таблица (а)",
            qk_table.time_ms, qk_table.memory_bytes, "база");
    fprintf(out_stream, "%-35s %10.4f %14ld %12.1f\n", "Быстрая, таблица ключей (б)",
            qk_keys.time_ms, qk_keys.memory_bytes,
            relative_percent(qk_table.time_ms, qk_keys.time_ms));
    fprintf(out_stream, "%-35s %10.4f %14ld %12s\n", "Выборка данных по табл. ключей",
            lookup.time_ms, lookup.memory_bytes, "-");

    fprintf(out_stream, "\nПояснение колонки \"%% к таблице\": насколько метод (б) быстрее/медленнее\n"
                 "соответствующего метода (а) в процентах: отрицательное значение — метод (б)\n"
                 "быстрее; положительное — медленнее.\n"
                 "Память таблицы ключей = sizeof(KeyEntry)*N — дополнительна к памяти самой\n"
                 "таблицы (sizeof(Flat)*N), т.е. вариант (б) требует БОЛЬШЕ памяти суммарно,\n"
                 "но не переставляет тяжёлые записи Flat при сортировке.\n");

    return OK;
}

int print_bench_report(Table *t)
{
    return write_report(stdout, t);
}

int save_bench_report(char *filename, Table *t)
{
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
        return ERR_FILE_OPEN;

    int err = write_report(fp, t);
    fclose(fp);
    return err;
}
