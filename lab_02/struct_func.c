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

void flat_print_header(void)
{
    printf("%-30s %8s %6s %10s %-10s %s\n",
           "Адрес", "Площадь", "Комн.", "Цена/м2", "Тип", "Доп. поля");
}

void flat_print(Flat *f)
{
    printf("%-30s %8.2f %6d %10.2f ", f->address, f->area, f->rooms, f->price_m2);
    if (f->type == PRIMARY)
    {
        printf("%-10s отделка=%s\n", "первичное",
               f->variable.primary.has_finishing ? "да" : "нет");
    }
    else
    {
        printf("%-10s год=%d, собств.=%d, животные=%s\n", "вторичное",
               f->variable.secondary.build_year,
               f->variable.secondary.owners,
               f->variable.secondary.had_pets ? "да" : "нет");
    }
}

void table_print(Table *table)
{
    if (table->count == 0)
    {
        printf("Таблица пуста.\n");
        return;
    }
    flat_print_header();
    for (int i = 0; i < table->count; i++)
        flat_print(&table->data[i]);
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

void key_table_print(KeyTable *key_table, Table *table)
{
    if (key_table->count == 0)
    {
        printf("Таблица ключей пуста.\n");
        return;
    }
    printf("%-6s %-8s ", "№п/п", "Индекс");
    flat_print_header();
    for (int i = 0; i < key_table->count; i++)
    {
        printf("%-6d %-8d ", i + 1, key_table->items[i].index);
        flat_print(&table->data[key_table->items[i].index]);
    }
}
