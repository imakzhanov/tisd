#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bench.h"
#include "sort.h"

#define REPEAT_COUNT 50

int time_sort_table(Table *table, void (*sort)(Table *), double *time)
{
    double total = 0;
    for (int r = 0; r < REPEAT_COUNT; r++)
    {
        Table copy;
        copy.count = table->count;
        copy.data = malloc(sizeof(Flat) * table->count);
        if (copy.data == NULL)
            return ERR_MEMORY;
        memcpy(copy.data, table->data, sizeof(Flat) * table->count);

        clock_t start = clock();
        sort(&copy);
        clock_t end = clock();

        total += (double)(end - start) / CLOCKS_PER_SEC * 1000000;
        table_free(&copy);
    }
    *time = total / REPEAT_COUNT;
    return OK;
}

int time_sort_keys(Table *table, void (*sort)(KeyTable *), double *time)
{
    double total = 0;
    for (int r = 0; r < REPEAT_COUNT; r++)
    {
        KeyTable key_table;
        if (build_key_table(table, &key_table) != OK)
            return ERR_MEMORY;

        clock_t start = clock();
        sort(&key_table);
        clock_t end = clock();

        total += (double)(end - start) / CLOCKS_PER_SEC * 1000000;
        key_table_free(&key_table);
    }
    *time = total / REPEAT_COUNT;
    return OK;
}

double percent(double time, double base_time)
{
    if (base_time == 0)
        return 0;
    return time / base_time * 100;
}

int make_report(char *filename, Table *table)
{
    if (table->count == 0)
        return ERR_EMPTY_TABLE;

    double bubble_table, quick_table, bubble_keys, quick_keys;
    if (time_sort_table(table, sort_bubble_table, &bubble_table) != OK ||
        time_sort_table(table, sort_quick_table, &quick_table) != OK ||
        time_sort_keys(table, sort_bubble_keys, &bubble_keys) != OK ||
        time_sort_keys(table, sort_quick_keys, &quick_keys) != OK)
        return ERR_MEMORY;

    long table_memory = sizeof(Flat) * table->count;
    long keys_memory = sizeof(KeyEntry) * table->count;

    FILE *f = fopen(filename, "w");
    if (f == NULL)
        return ERR_FILE_OPEN;

    fprintf(f, "Records: %d\n", table->count);
    fprintf(f, "%-15s %12s %12s %10s\n", "Method", "Average Time, microsec", "Memory, B", "Time, %");
    fprintf(f, "%-20s %12.0f %12ld %10.1f\n", "Bubble (table)", bubble_table, table_memory, 100.0);
    fprintf(f, "%-20s %12.0f %12ld %10.1f\n", "Bubble (keys)", bubble_keys, keys_memory,
            percent(bubble_keys, bubble_table));
    fprintf(f, "%-20s %12.0f %12ld %10.1f\n", "Quick (table)", quick_table, table_memory,
            percent(quick_table, bubble_table));
    fprintf(f, "%-20s %12.0f %12ld %10.1f\n", "Quick (keys)", quick_keys, keys_memory,
            percent(quick_keys, bubble_table));

    fclose(f);
    return OK;
}
