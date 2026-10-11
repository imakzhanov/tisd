#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <direct.h>
#define MAKE_DIR(path) _mkdir(path)
#else
#include <sys/stat.h>
#define MAKE_DIR(path) mkdir(path, 0777)
#endif

#include "config.h"
#include "io.h"
#include "struct_func.h"
#include "errors.h"

int read_line(char *buf, size_t size)
{
    if (fgets(buf, size, stdin) == NULL)
        return ERR_EMPTY;

    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n')
        buf[--len] = '\0';
    
    return OK;
}

int read_int(const char *text, int min, int max, int *value)
{
    char line[LINE_SIZE];
    int rc;

    printf("%s [от %d до %d]: ", text, min, max);
    rc = read_line(line, sizeof(line));
    if (rc != OK)
        return rc;
    if (sscanf(line, "%d", value) != 1)
        return ERR_INPUT;
    if (*value < min || *value > max)
        return ERR_RANGE;
    return OK;
}

static int input_size(int *rows, int *cols)
{
    int rc = read_int("Введите число строк", 1, MAX_SIZE, rows);
    if (rc == OK)
        rc = read_int("Введите число столбцов", 1, MAX_SIZE, cols);
    return rc;
}

// координатный ввод
int input_manual(standard_t *m)
{
    char line[LINE_SIZE];
    int rows, cols, k, i, j, value;
    int rc;

    rc = input_size(&rows, &cols);
    if (rc != OK)
        return rc;
    rc = read_int("Введите количество ненулевых элементов", 0, rows * cols, &k);
    if (rc != OK)
        return rc;

    standard_fill_zero(m, rows, cols);
    if (k > 0)
        printf("\nВводите элементы по одному в строке: <строка> <столбец> <значение>\n"
               "строка от 1 до %d, столбец от 1 до %d, значение - целое, не равное 0\n", rows, cols);

    for (int n = 0; n < k; n++)
    {
        printf("Элемент %d из %d: ", n + 1, k);
        rc = read_line(line, sizeof(line));
        if (rc != OK)
            return rc;
        if (sscanf(line, "%d %d %d", &i, &j, &value) != 3)
            return ERR_INPUT;
        if (i < 1 || i > rows || j < 1 || j > cols)
            return ERR_RANGE;
        if (value == 0)
            return ERR_ZERO;
        if (m->matrix[i - 1][j - 1] != 0)
            return ERR_DUPLICATE;
        m->matrix[i - 1][j - 1] = value;
    }
    return OK;
}

// читает n целых чисел
static int read_int_array(FILE *f, int *arr, int n)
{
    for (int k = 0; k < n; k++)
        if (fscanf(f, "%d", &arr[k]) != 1)
            return ERR_FILE_FORMAT;
    return OK;
}

/**
 * @brief чтение из файла, структура файла:
 * 
 * rows
 * cols
 * nonzero_count
 * a - массив длины nonzero_count
 * ia - массив длины nonzero_count
 * ja - массив длины cols + 1
 * 
 * @param f 
 * @param m 
 * @return int 
 */
static int read_matrix_file(FILE *f, standard_t *m)
{
    int rows, cols, nnz;
    csc_t csc;
    int rc;

    if (fscanf(f, "%d %d %d", &rows, &cols, &nnz) != 3)
        return ERR_FILE_FORMAT;
    if (rows < 1 || rows > MAX_SIZE || cols < 1 || cols > MAX_SIZE)
        return ERR_RANGE;
    if (nnz < 0 || nnz > rows * cols)
        return ERR_RANGE;

    rc = csc_alloc(&csc, rows, cols, nnz);
    if (rc != OK)
        return rc;

    rc = read_int_array(f, csc.a, nnz);
    if (rc == OK)
        rc = read_int_array(f, csc.ia, nnz);
    if (rc == OK)
        rc = read_int_array(f, csc.ja, cols + 1);
    if (rc == OK)
        rc = csc_to_standard(&csc, m);

    csc_free(&csc);
    return rc;
}

int input_file(const char *path, standard_t *m)
{
    FILE *f = fopen(path, "r");
    if (f == NULL)
        return ERR_FILE_OPEN;

    int rc = read_matrix_file(f, m);
    fclose(f);
    return rc;
}

int input_random(standard_t *m)
{
    int rows, cols, percent;
    int rc = input_size(&rows, &cols);
    if (rc == OK)
        rc = read_int("Введите процент заполнения ненулевыми", 0, 100, &percent);
    if (rc == OK)
        rc = standard_fill_random(m, rows, cols, percent);
    return rc;
}

// limit == 0 - выводить все элементы
static void print_array(FILE *out, const char *label, const int *arr, int n, int limit)
{
    int shown = (limit > 0 && n > limit) ? limit : n;

    fprintf(out, "%-3s:", label);
    for (int k = 0; k < shown; k++)
        fprintf(out, "%d ", arr[k]);
    if (shown < n)
        fprintf(out, " (ещё %d)", n - shown);
    fprintf(out, "\n");
}

void print_csc(FILE *out, const char *name, const csc_t *m, int limit)
{
    fprintf(out, "\n");
    fprintf(out, "Матрица %s в формате CSC: %d на %d, ненулевых элементов: %d\n", name, m->rows, m->cols, m->nonzero_count);
    if (m->nonzero_count == 0)
    {
        fprintf(out, "Матрица нулевая\n");
        return;
    }
    fprintf(out, "(A - значения, IA - номера строк значений, JA - начала столбцов в A/IA)\n");
    print_array(out, "A", m->a, m->nonzero_count, limit);
    print_array(out, "IA", m->ia, m->nonzero_count, limit);
    print_array(out, "JA", m->ja, m->cols + 1, limit);
}

// одна строка из n чисел через пробел
static int write_array(FILE *f, const int *arr, int n)
{
    for (int k = 0; k < n; k++)
        fprintf(f, k ? " %d" : "%d", arr[k]);
    return fprintf(f, "\n") < 0 ? ERR_FILE_WRITE : OK;
}

int create_dir(const char *path)
{
    if (MAKE_DIR(path) != 0 && errno != EEXIST)
        return ERR_FILE_OPEN;
    return OK;
}

int save_csc(const char *path, const csc_t *m)
{
    FILE *f = fopen(path, "w");
    if (f == NULL)
        return ERR_FILE_OPEN;

    int rc = fprintf(f, "%d\n%d\n%d\n", m->rows, m->cols, m->nonzero_count) < 0 ? ERR_FILE_WRITE : OK;
    if (rc == OK)
        rc = write_array(f, m->a, m->nonzero_count);
    if (rc == OK)
        rc = write_array(f, m->ia, m->nonzero_count);
    if (rc == OK)
        rc = write_array(f, m->ja, m->cols + 1);

    if (fclose(f) != 0 && rc == OK)
        rc = ERR_FILE_WRITE;
    return rc;
}

void print_standard(const char *name, const standard_t *m)
{
    if (m->rows > PRINT_STANDARD_MAX || m->cols > PRINT_STANDARD_MAX)
    {
        printf("Матрица %s (%d на %d) больше чем %d на %d, в обычном виде не выводится\n",
               name, m->rows, m->cols, PRINT_STANDARD_MAX, PRINT_STANDARD_MAX);
        return;
    }
    printf("Матрица %s в обычном виде (%d на %d):\n", name, m->rows, m->cols);
    for (int i = 0; i < m->rows; i++)
    {
        for (int j = 0; j < m->cols; j++)
            printf("%6d", m->matrix[i][j]);
        printf("\n");
    }
}
