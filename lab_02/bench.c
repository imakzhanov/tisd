#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bench.h"
#include "sort.h"

#define REPEAT_COUNT 50

double trash;

static double now_microsec(void)
{
    return (double)clock() / CLOCKS_PER_SEC * 1000000;
}

int time_sort_table(Table *table, void (*sort)(Table *), double *time)
{
    *time = 0;
    for (int r = 0; r < REPEAT_COUNT; r++)
    {
        Table copy;
        copy.count = table->count;
        copy.data = malloc(sizeof(Flat) * table->count);
        if (copy.data == NULL)
            return ERR_MEMORY;
        memcpy(copy.data, table->data, sizeof(Flat) * table->count);

        double start = now_microsec();
        sort(&copy);
        double end = now_microsec();

        *time += end - start;
        table_free(&copy);
    }
    *time /= REPEAT_COUNT;
    return OK;
}

int time_sort_keys(Table *table, void (*sort)(KeyTable *), double *sort_time, double *total_time)
{
    *sort_time = 0, *total_time = 0;
    for (int r = 0; r < REPEAT_COUNT; r++)
    {
        KeyTable key_table;
        if (build_key_table(table, &key_table) != OK)
            return ERR_MEMORY;

        double start = now_microsec();
        sort(&key_table);
        double sorted = now_microsec();

        double sum = 0;
        for (int i = 0; i < key_table.count; i++)
            sum += table->data[key_table.items[i].index].price_m2;
        trash = sum;
        double end = now_microsec();

        *sort_time += sorted - start;
        *total_time += end - start;
        key_table_free(&key_table);
    }
    *sort_time /= REPEAT_COUNT;
    *total_time /= REPEAT_COUNT;
    return OK;
}

double percent(double value, double base)
{
    if (base == 0)
        return 0;
    return value / base * 100;
}

int make_report(char *filename, Table *table)
{
    if (table->count == 0)
        return ERR_EMPTY_TABLE;

    double bubble_table, quick_table;
    double bubble_keys, bubble_keys_total, quick_keys, quick_keys_total;
    if (time_sort_table(table, sort_bubble_table, &bubble_table) != OK ||
        time_sort_table(table, sort_quick_table, &quick_table) != OK ||
        time_sort_keys(table, sort_bubble_keys, &bubble_keys, &bubble_keys_total) != OK ||
        time_sort_keys(table, sort_quick_keys, &quick_keys, &quick_keys_total) != OK)
        return ERR_MEMORY;

    long table_memory = sizeof(Flat) * table->count;
    long keys_memory = sizeof(KeyEntry) * table->count;
    long total_memory = table_memory + keys_memory;

    FILE *f = fopen(filename, "w");
    if (f == NULL)
        return ERR_FILE_OPEN;

    fprintf(f, "Количество записей: %d\n", table->count);
    fprintf(f, "Сортировка пузырьком всей таблицы (Bubble (table)) принята за 100%%\n\n");

    fprintf(f, "%-16s %25s %25s %8s %15s %15s %8s\n",
            "Method", "Average Sort, microsec", "Sort+selection, microsec", "Time, %", "Sorted mem, B", "Total mem, B", "Mem, %");
    fprintf(f, "%-16s %25.0f %25.0f %8.1f %15ld %15ld %8.1f\n", "Bubble (table)",
            bubble_table, bubble_table, 100.0, table_memory, table_memory, 100.0);
    fprintf(f, "%-16s %25.0f %25.0f %8.1f %15ld %15ld %8.1f\n", "Bubble (keys)",
            bubble_keys, bubble_keys_total, percent(bubble_keys_total, bubble_table),
            keys_memory, total_memory, percent(total_memory, table_memory));
    fprintf(f, "%-16s %25.0f %25.0f %8.1f %15ld %15ld %8.1f\n", "Quick (table)",
            quick_table, quick_table, percent(quick_table, bubble_table),
            table_memory, table_memory, 100.0);
    fprintf(f, "%-16s %25.0f %25.0f %8.1f %15ld %15ld %8.1f\n", "Quick (keys)",
            quick_keys, quick_keys_total, percent(quick_keys_total, bubble_table),
            keys_memory, total_memory, percent(total_memory, table_memory));

    fclose(f);
    return OK;
}
