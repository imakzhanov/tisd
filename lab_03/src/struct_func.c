#include <stdlib.h>

#include "struct_func.h"
#include "errors.h"

// модуль случайных значений при случайном заполнении
#define RANDOM_VALUE_MAX 100

void standard_fill_zero(standard_t *m, int rows, int cols)
{
    m->rows = rows;
    m->cols = cols;
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            m->matrix[i][j] = 0;
}

// память, которую занимает матрица при стандартном хранении
size_t standard_memory(const standard_t *m)
{
    return m->rows * m->cols * sizeof(int);
}

int standard_count_nonzero(const standard_t *m)
{
    int count = 0;
    for (int i = 0; i < m->rows; i++)
        for (int j = 0; j < m->cols; j++)
            if (m->matrix[i][j] != 0)
                count++;
    return count;
}

void csc_init(csc_t *m)
{
    m->rows = 0;
    m->cols = 0;
    m->nonzero_count = 0;
    m->a = NULL;
    m->ia = NULL;
    m->ja = NULL;
}

// выделяет память под матрицу с nonzero_count ненулевыми
int csc_alloc(csc_t *m, int rows, int cols, int nonzero_count)
{
    csc_init(m);
    m->ja = calloc(cols + 1, sizeof(int));
    if (m->ja == NULL)
        return ERR_MEMORY;

    if (nonzero_count > 0)
    {
        m->a = malloc(nonzero_count * sizeof(int));
        m->ia = malloc(nonzero_count * sizeof(int));
        if (m->a == NULL || m->ia == NULL)
        {
            csc_free(m);
            return ERR_MEMORY;
        }
    }

    m->rows = rows;
    m->cols = cols;
    m->nonzero_count = nonzero_count;
    return OK;
}

void csc_free(csc_t *m)
{
    free(m->a);
    free(m->ia);
    free(m->ja);
    csc_init(m);
}

// память под массив CSC
size_t csc_memory(const csc_t *m)
{
    return (2 * m->nonzero_count + (m->cols + 1)) * sizeof(int);
}

/**
 * @brief Сложение матриц стандартного вида
 * 
 * @param a первое слагаемое
 * @param b второе слагаемое
 * @param res результат a + b
 * @return int OK или ERR_SIZE, если размеры не совпадают
 */
int standard_add(const standard_t *a, const standard_t *b, standard_t *res)
{
    if (a->rows != b->rows || a->cols != b->cols)
        return ERR_SIZE;

    res->rows = a->rows;
    res->cols = a->cols;
    for (int i = 0; i < a->rows; i++)
        for (int j = 0; j < a->cols; j++)
            res->matrix[i][j] = a->matrix[i][j] + b->matrix[i][j];
    return OK;
}

/**
 * @brief сложение матриц вида csc
 * 
 * @param a первое слагаемое
 * @param b второе слагаемое
 * @param res результат a + b, память выделяется внутри, освобождает вызывающий
 * @return int OK, ERR_SIZE или ERR_MEMORY
 */
int csc_add(const csc_t *a, const csc_t *b, csc_t *res)
{
    if (a->rows != b->rows || a->cols != b->cols)
        return ERR_SIZE;

    int actual_nonzero = 0;
    int rc = csc_alloc(res, a->rows, a->cols, a->nonzero_count + b->nonzero_count);
    if (rc != OK)
        return rc; 

    int row_a, row_b;
    for (int j = 0; j < a->cols; j++)
    {
        // индекс для списка ia/a
        row_a = a->ja[j];
        row_b = b->ja[j];

        // пока не пройдем все числа в столбце, перенос в матрицу результата
        while (row_a < a->ja[j + 1] && row_b < b->ja[j + 1])
        {
            //если число встречается на строке выше чем в 2 матрице (просто переносим его без сложения)
            if (a->ia[row_a] < b->ia[row_b])
            {
                res->a[actual_nonzero] = a->a[row_a];
                res->ia[actual_nonzero] = a->ia[row_a];
                row_a++;
                actual_nonzero++;
            }
            //  переносим наоборот из 2 матрицы
            else if (a->ia[row_a] > b->ia[row_b])
            {
                res->a[actual_nonzero] = b->a[row_b];
                res->ia[actual_nonzero] = b->ia[row_b];
                actual_nonzero++;
                row_b++;
            }
            // сложение элементов
            else
            {
                if (a->a[row_a] + b->a[row_b] != 0)
                {
                    res->a[actual_nonzero] = a->a[row_a] + b->a[row_b];
                    res->ia[actual_nonzero] = a->ia[row_a];
                    actual_nonzero++;
                }
                row_a++, row_b++;
            }
        }
        while (row_b < b->ja[j + 1])
        {
            res->a[actual_nonzero] = b->a[row_b];
            res->ia[actual_nonzero] = b->ia[row_b];
            actual_nonzero++;
            row_b++;
        }
        while (row_a < a->ja[j + 1])
        {
            res->a[actual_nonzero] = a->a[row_a];
            res->ia[actual_nonzero] = a->ia[row_a];
            row_a++;
            actual_nonzero++;
        }
        res->ja[j + 1] = actual_nonzero;
    }
    res->nonzero_count = actual_nonzero;

    return OK;
}

/**
 * @brief Перевод матрицы стандартного вида в формат CSC
 * 
 * @param src 
 * @param res 
 * @return int
 */
int standard_to_csc(const standard_t *src, csc_t *res)
{
    // первый проход: считаем ненулевые, чтобы выделить память ровно под них
    int rc = csc_alloc(res, src->rows, src->cols, standard_count_nonzero(src));
    if (rc != OK)
        return rc;

    int current_nonzero = 0;
    for (int j = 0; j < src->cols; j++)
    {
        for (int i = 0; i < src->rows; i++)
        {
            if (src->matrix[i][j] != 0)
            {
                res->a[current_nonzero] = src->matrix[i][j];
                res->ia[current_nonzero] = i;
                current_nonzero++;
            }
        }
        res->ja[j + 1] = current_nonzero;
    }
    return OK;
}

/**
 * @brief Перевод матрицы из формата CSC в стандартный вид с проверкой корректности CSC
 * 
 * @param src матрица в формате CSC
 * @param res результат
 * @return int OK, ERR_FILE_FORMAT (неверный ja), ERR_RANGE (номер строки вне матрицы),
 *             ERR_ZERO (нулевое значение) или ERR_DUPLICATE (два элемента в одной клетке)
 */
int csc_to_standard(const csc_t *src, standard_t *res)
{
    if (src->ja[0] != 0 || src->ja[src->cols] != src->nonzero_count)
        return ERR_FILE_FORMAT;

    standard_fill_zero(res, src->rows, src->cols);
    for (int j = 0; j < src->cols; j++)
    {
        if (src->ja[j] > src->ja[j + 1])
            return ERR_FILE_FORMAT;
        for (int k = src->ja[j]; k < src->ja[j + 1]; k++)
        {
            if (src->ia[k] < 0 || src->ia[k] >= src->rows)
                return ERR_RANGE;
            if (src->a[k] == 0)
                return ERR_ZERO;
            if (res->matrix[src->ia[k]][j] != 0)
                return ERR_DUPLICATE;
            res->matrix[src->ia[k]][j] = src->a[k];
        }
    }
    return OK;
}


// случайное число от 0 до n - 1
static long random_below(long n)
{
    long random = (long)rand() * ((long)RAND_MAX + 1) + rand();
    return random % n;
}

// случайное ненулевое значение от -RANDOM_VALUE_MAX до RANDOM_VALUE_MAX
static int random_nonzero()
{
    int value = rand() % RANDOM_VALUE_MAX + 1;
    if (rand() % 2 == 0)
        return value;
    return -value;
}

/**
 * @brief Случайное заполнение матрицы с заданным процентом ненулевых элементов
 * 
 * @param m 
 * @param rows 
 * @param cols 
 * @param percent процент ненулевых элементов (от 0 до 100)
 * @return int
 */
int standard_fill_random(standard_t *m, int rows, int cols, int percent)
{
    if (rows < 1 || rows > MAX_SIZE || cols < 1 || cols > MAX_SIZE || percent < 0 || percent > 100)
        return ERR_RANGE;

    standard_fill_zero(m, rows, cols);

    long total = (long)rows * cols;
    long count = total * percent / 100; // округление вниз

    int pos = 0;
    // каждая клетка берется с вероятностью (сколько осталось взять) / (сколько осталось клеток)
    for (int i = 0; i < rows; i++)
    {
        for(int j = 0; j < cols; j++)
        {   
            pos = i * cols + j;
            if (random_below(total - pos) < count)
            {
                m->matrix[i][j] = random_nonzero();
                count--;
            }
        }
    }

    return OK;
}
