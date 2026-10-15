#ifndef IO_H
#define IO_H

#include <stddef.h>
#include <stdio.h>

#include "struct_func.h"

int read_line(char *buf, size_t size);
int read_int(const char *prompt, int min, int max, int *value);

// fixed_rows/fixed_cols: если > 0, размер матрицы задан заранее и не запрашивается
int input_manual(standard_t *m, int fixed_rows, int fixed_cols);
int input_file(const char *path, standard_t *m);
int input_random(standard_t *m, int fixed_rows, int fixed_cols);

void print_csc(FILE *out, const char *name, const csc_t *m);
// создаёт папку (если она уже есть - не ошибка)
int create_dir(const char *path);
// запись в файл в формате для ввода: rows, cols, nnz, затем a, ia, ja (индексы с 0, как хранятся)
int save_csc(const char *path, const csc_t *m);
void print_standard(const char *name, const standard_t *m);

#endif
