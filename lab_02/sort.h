#ifndef SORT_H
#define SORT_H

#include "struct_func.h"

/* Простой алгоритм — сортировка пузырьком по price_m2 */
void sort_bubble_table(Table *t);
void sort_bubble_keys(KeyTable *kt);

/* Ускоренный алгоритм — быстрая сортировка по price_m2 */
void sort_quick_table(Table *t);
void sort_quick_keys(KeyTable *kt);

#endif
