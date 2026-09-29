#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "struct_func.h"

#define EPS 1e-6

void table_init(Table *t)
{
    t->data = NULL;
    t->count = 0;
}

int table_add(Table *t, Flat f)
{
    Flat *new_data = realloc(t->data, sizeof(Flat) * (t->count + 1));
    if (new_data == NULL)
        return ERR_MEMORY;
    t->data = new_data;
    t->data[t->count] = f;
    t->count++;
    return OK;
}

int flat_matches_field(Flat *f, FieldId field, void *value)
{
    switch (field)
    {
        case ADDRESS_FIELD:
            return strcmp(f->address, (char *)value) == 0;
        case AREA_FIELD:
            return fabs(f->area - *(double *)value) < EPS;
        case ROOMS_FIELD:
            return f->rooms == *(int *)value;
        case PRICE_M2_FIELD:
            return fabs(f->price_m2 - *(double *)value) < EPS;
    }
    return 0;
}

int table_remove_by_field(Table *t, FieldId field, void *value, int *out_removed)
{
    int removed = 0;
    int write = 0;
    for (int read = 0; read < t->count; read++)
    {
        if (flat_matches_field(&t->data[read], field, value))
        {
            removed++;
            continue;
        }
        if (write != read)
            t->data[write] = t->data[read];
        write++;
    }
    t->count = write;
    *out_removed = removed;
    if (removed == 0)
        return ERR_NOT_FOUND;
    return OK;
}

void table_free(Table *t)
{
    free(t->data);
    t->data = NULL;
    t->count = 0;
}
void print_flat_header(void)
{
    printf("%-20s %8s %6s %10s %10s %10s %11s %7s %9s\n",
           "Address", "Area", "Rooms", "Price/m2", "Type",
           "Finishing", "Build year", "Owners", "Had pets");
}

void print_flat(Flat *f)
{
    char finishing[8] = "-";
    char build_year[16] = "-";
    char owners[16] = "-";
    char had_pets[8] = "-";
    const char *type_str;

    if (f->type == PRIMARY)
    {
        type_str = "primary";
        snprintf(finishing, sizeof(finishing), "%s",
                 f->variable.primary.has_finishing ? "yes" : "no");
    }
    else
    {
        type_str = "secondary";
        snprintf(build_year, sizeof(build_year), "%d",
                  f->variable.secondary.build_year);
        snprintf(owners, sizeof(owners), "%d",
                  f->variable.secondary.owners);
        snprintf(had_pets, sizeof(had_pets), "%s",
                  f->variable.secondary.had_pets ? "yes" : "no");
    }

    printf("%-20s %8.2f %6d %10.2f %10s %10s %11s %7s %9s\n",
           f->address, f->area, f->rooms, f->price_m2, type_str,
           finishing, build_year, owners, had_pets);
}

void print_table(Table *table)
{
    if (table->count == 0)
    {
        printf("Таблица пуста.\n");
        return;
    }
    print_flat_header();
    for (int i = 0; i < table->count; i++)
        print_flat(&table->data[i]);
}

int build_key_table(Table *table, KeyTable *key_table)
{
    key_table->items = malloc(sizeof(KeyEntry) * table->count);
    if (key_table->items == NULL)
        return ERR_MEMORY;

    key_table->count = table->count;
    for (int i = 0; i < table->count; i++)
    {
        key_table->items[i].index = i;
        key_table->items[i].key = table->data[i].price_m2;
    }
    return OK;
}

void key_table_free(KeyTable *key_table)
{
    free(key_table->items);
    key_table->items = NULL;
    key_table->count = 0;
}

void print_table_by_key(KeyTable *key_table, Table *table)
{
    if (key_table->count == 0)
    {
        printf("Таблица ключей пуста.\n");
        return;
    }
    printf("%-6s %-8s ", "Number", "Table Index");
    print_flat_header();
    for (int i = 0; i < key_table->count; i++)
    {
        printf("%-6d %-8d ", i + 1, key_table->items[i].index);
        print_flat(&table->data[key_table->items[i].index]);
    }
}

void print_key_table(KeyTable *key_table)
{
    if (key_table->count == 0)
    {
        printf("Таблица ключей пуста.\n");
        return;
    }
    printf("%-10s %-10s %s\n", "Number", "Index", "Key (price per m2)");
    for (int i = 0; i < key_table->count; i++)
        printf("%-10d %-10d %.2f\n", i + 1, key_table->items[i].index, key_table->items[i].key);
}
