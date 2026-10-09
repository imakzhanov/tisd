#include "sort.h"

static void swap_flat(Flat *a, Flat *b)
{
    Flat tmp = *a;
    *a = *b;
    *b = tmp;
}

static void swap_key(KeyEntry *a, KeyEntry *b)
{
    KeyEntry tmp = *a;
    *a = *b;
    *b = tmp;
}

void sort_bubble_table(Table *table)
{
    for (int i = 0; i < table->count - 1; i++)
    {
        for (int j = 0; j < table->count - 1 - i; j++)
        {
            if (table->data[j].price_m2 > table->data[j + 1].price_m2)
                swap_flat(&table->data[j], &table->data[j + 1]);
        }
    }
}

void sort_bubble_keys(KeyTable *key_table)
{
    for (int i = 0; i < key_table->count - 1; i++)
    {
        for (int j = 0; j < key_table->count - 1 - i; j++)
        {
            if (key_table->items[j].key > key_table->items[j + 1].key)
                swap_key(&key_table->items[j], &key_table->items[j + 1]);
        }
    }
}

static int partition_table(Flat *data, int low, int high)
{
    double pivot = data[high].price_m2;
    int i = low - 1;
    for (int j = low; j < high; j++)
    {
        if (data[j].price_m2 <= pivot)
        {
            i++;
            swap_flat(&data[i], &data[j]);
        }
    }
    swap_flat(&data[i + 1], &data[high]);
    return i + 1;
}

static void quick_table(Flat *data, int low, int high)
{
    if (low < high)
    {
        int p = partition_table(data, low, high);
        quick_table(data, low, p - 1);
        quick_table(data, p + 1, high);
    }
}

void sort_quick_table(Table *t)
{
    if (t->count > 1)
        quick_table(t->data, 0, t->count - 1);
}

static int partition_keys(KeyEntry *items, int low, int high)
{
    double pivot = items[high].key;
    int i = low - 1;
    for (int j = low; j < high; j++)
    {
        if (items[j].key <= pivot)
        {
            i++;
            swap_key(&items[i], &items[j]);
        }
    }
    swap_key(&items[i + 1], &items[high]);
    return i + 1;
}

static void quick_keys(KeyEntry *items, int low, int high)
{
    if (low < high)
    {
        int p = partition_keys(items, low, high);
        quick_keys(items, low, p - 1);
        quick_keys(items, p + 1, high);
    }
}

void sort_quick_keys(KeyTable *kt)
{
    if (kt->count > 1)
        quick_keys(kt->items, 0, kt->count - 1);
}
