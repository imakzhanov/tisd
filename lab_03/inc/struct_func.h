#ifndef STRUCT_FUNC_H
#define STRUCT_FUNC_H

#include <stddef.h>

#include "config.h"

// обычная матрица
typedef struct
{
    int rows; //кол-во строк
    int cols; //кол-во столбцов
    int matrix[MAX_SIZE][MAX_SIZE];
} standard_t;

// разреженная матрица в формате CSC
typedef struct
{
    int rows; //кол-во строк
    int cols; //кол-во столбцов
    int nonzero_count; //кол-во ненулевых элементов

    int *a;
    int *ia;
    int *ja;
} csc_t;

void standard_fill_zero(standard_t *m, int rows, int cols);
size_t standard_memory(const standard_t *m);
int standard_count_nonzero(const standard_t *m);

void csc_init(csc_t *m);
int csc_alloc(csc_t *m, int rows, int cols, int nonzero_count);
void csc_free(csc_t *m);
size_t csc_memory(const csc_t *m);

int standard_add(const standard_t *a, const standard_t *b, standard_t *res);

void csc_add_into(const csc_t *a, const csc_t *b, csc_t *res);

int csc_add(const csc_t *a, const csc_t *b, csc_t *res);

int standard_to_csc(const standard_t *src, csc_t *res);

int csc_to_standard(const csc_t *src, standard_t *res);

int standard_fill_random(standard_t *m, int rows, int cols, int percent);

#endif
