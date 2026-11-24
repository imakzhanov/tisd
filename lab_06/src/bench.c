#include "io.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct
{
    char text[WORD_CAPACITY];
} word_t;

typedef struct
{
    const word_t *input;
    const word_t *sorted;
    size_t size;
    size_t tree_bytes;
    word_t *work;
} dataset_t;

// Сравнивает слова для сортировки массива функцией qsort.
static int compare_words(const void *left, const void *right)
{
    const word_t *a = left;
    const word_t *b = right;
    return strcmp(a->text, b->text);
}

// Заполняет массив латинскими словами для замеров.
static void make_words(word_t *words, size_t size)
{
    const char alphabet[] = "abcdefghijklmnopqrstuvwxyz";
    for (size_t i = 0; i < size; ++i)
    {
        words[i].text[0] = alphabet[i % 26];
        words[i].text[1] = alphabet[i / 26 % 26];
        words[i].text[2] = alphabet[i / (26 * 26) % 26];
        words[i].text[3] = 'a';
        words[i].text[4] = '\0';
    }
}

// Перемешивает слова в воспроизводимом порядке.
static void shuffle_words(word_t *words, size_t size)
{
    uint32_t state = 42;
    for (size_t i = size - 1; i > 0; --i)
    {
        state = state * UINT32_C(1664525) + UINT32_C(1013904223);
        size_t j = state % (i + 1);
        word_t tmp = words[i];
        words[i] = words[j];
        words[j] = tmp;
    }
}

// Строит дерево, вставляя слова в порядке массива.
static int build_tree(node_t **root, const word_t *words, size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        int rc = tree_insert(root, words[i].text);
        if (rc != OK)
            return rc;
    }
    return OK;
}

// Строит сбалансированное дерево, сначала вставляя середину каждого диапазона.
static int build_balanced(node_t **root, const word_t *words, size_t size)
{
    if (!size)
        return OK;

    size_t middle = size / 2;
    int rc = tree_insert(root, words[middle].text);
    if (rc == OK)
        rc = build_balanced(root, words, middle);
    if (rc == OK)
        rc = build_balanced(root, words + middle + 1, size - middle - 1);
    return rc;
}

// Вычисляет время от начала замера в микросекундах.
static int elapsed_us(clock_t start, double *time)
{
    clock_t end = clock();
    if (start == (clock_t)-1 || end == (clock_t)-1 || end < start)
        return TIME_ERROR;
    *time = (double)(end - start) * 1e6 / CLOCKS_PER_SEC;
    return OK;
}

// Замеряет среднее время точного поиска и среднее число сравнений.
static int measure_search(const node_t *root, const dataset_t *data,
                          double *time, double *mean_comparisons)
{
    size_t comparisons = 0;
    size_t hits = 0;
    char missing[5];
    clock_t start = clock();
    for (size_t i = 0; i < SEARCH_RUNS; ++i)
    {
        const char *key = data->input[(i / 2) % data->size].text;
        if (i % 2)
        {
            memcpy(missing, key, sizeof(missing));
            missing[3] = 'b';
            key = missing;
        }
        hits += tree_find(root, key, &comparisons) != NULL;
    }

    int rc = elapsed_us(start, time);
    if (rc != OK)
        return rc;
    if (hits != (SEARCH_RUNS + 1) / 2)
        return BENCH_ERROR;
    *time /= SEARCH_RUNS;
    *mean_comparisons = (double)comparisons / SEARCH_RUNS;
    return OK;
}

// Печатает время в процентах от времени базового варианта.
static void print_relative_time(double time, double baseline)
{
    if (time > 0 && baseline > 0)
        printf(" %12.2f\n", 100.0 * (time / baseline));
    else
        printf(" %12s\n", "n/a");
}

// Сравнивает точный поиск в деревьях разной формы.
static int bench_search(const dataset_t *data)
{
    const char *names[] = {"balanced", "shuffled", "chain"};
    double baseline = 0;

    for (size_t shape = 0; shape < sizeof(names) / sizeof(names[0]); ++shape)
    {
        node_t *root = NULL;
        int rc;
        if (shape == 0)
            rc = build_balanced(&root, data->sorted, data->size);
        else
            rc = build_tree(&root, shape == 1 ? data->input : data->sorted, data->size);

        double time, comparisons;
        if (rc == OK)
            rc = measure_search(root, data, &time, &comparisons);
        if (rc == OK)
        {
            tree_stats_t stats = tree_stats(root);
            if (shape == 0)
                baseline = time;
            printf("%6zu %-10s %10zu %10zu %12.3f %12.2f %12zu",
                   stats.nodes, names[shape], stats.levels - 1, stats.forks, time,
                   comparisons, data->tree_bytes);
            print_relative_time(time, baseline);
        }
        tree_clear(&root);
        if (rc != OK)
            return rc;
    }
    return OK;
}

// Записывает слова в файл по одному на строку.
static int write_words(FILE *file, const dataset_t *data)
{
    for (size_t i = 0; i < data->size; ++i)
        if (fprintf(file, "%s\n", data->input[i].text) < 0)
            return FILE_ERROR;

    if (fflush(file) == EOF)
        return FILE_ERROR;
    return OK;
}

// Замеряет поиск по букве в файле и проверяет число совпадений.
static int measure_file_prefix(FILE *file, const char *letter, size_t expected,
                               double *time, size_t *comparisons)
{
    clock_t start = clock();
    for (int run = 0; run < BENCH_RUNS; ++run)
    {
        if (fseek(file, 0, SEEK_SET) != 0)
            return FILE_ERROR;

        size_t count;
        int rc = file_count_prefix(file, letter, &count, comparisons);
        if (rc != OK)
            return rc;
        if (count != expected)
            return BENCH_ERROR;
    }

    int rc = elapsed_us(start, time);
    if (rc == OK)
        *time /= BENCH_RUNS;
    return rc;
}

// Сравнивает поиск слов на заданную букву в дереве и файле.
static int bench_prefix(const dataset_t *data)
{
    FILE *file = tmpfile();
    if (!file)
        return FILE_ERROR;

    node_t *root = NULL;
    int rc = build_tree(&root, data->input, data->size);
    if (rc != OK)
    {
        tree_clear(&root);
        fclose(file);
        return rc;
    }

    const char *letter = "a";
    size_t tree_comparisons = 0;
    size_t count = 0;
    clock_t start = clock();
    for (int run = 0; run < SEARCH_RUNS; ++run)
        count = tree_count_prefix(root, letter, &tree_comparisons);

    double tree_time, file_time = 0;
    size_t file_comparisons = 0;
    rc = elapsed_us(start, &tree_time);
    if (rc == OK)
        rc = write_words(file, data);
    long file_bytes = 0;
    if (rc == OK)
    {
        file_bytes = ftell(file);
        if (file_bytes < 0)
            rc = FILE_ERROR;
    }
    if (rc == OK)
        rc = measure_file_prefix(file, letter, count, &file_time, &file_comparisons);
    if (fclose(file) == EOF && rc == OK)
        rc = FILE_ERROR;
    if (rc == OK)
    {
        tree_time /= SEARCH_RUNS;
        printf("%6zu %-10s %10zu %12.3f %12.2f %12zu", data->size, "tree", count, tree_time,
               (double)tree_comparisons / SEARCH_RUNS, data->tree_bytes);
        print_relative_time(tree_time, file_time);
        printf("%6zu %-10s %10zu %12.3f %12.2f %12ld", data->size, "file", count, file_time,
               (double)file_comparisons / BENCH_RUNS, file_bytes);
        print_relative_time(file_time, file_time);
    }

    tree_clear(&root);
    return rc;
}

// Копирует очередное слово обхода в выходной массив.
static void save_sorted_word(const char *word, void *context)
{
    word_t **next = context;
    strcpy((*next)->text, word);
    ++*next;
}

// Сравнивает сортировку через дерево с сортировкой массива функцией qsort.
static int bench_sort(const dataset_t *data)
{
    word_t *work = data->work;
    double tree_total = 0;
    double array_total = 0;
    for (int run = 0; run < BENCH_RUNS; ++run)
    {
        node_t *root = NULL;
        clock_t start = clock();
        int rc = build_tree(&root, data->input, data->size);
        if (rc == OK)
        {
            word_t *next = work;
            tree_traverse(root, INORDER, save_sorted_word, &next);
            if (next != work + data->size)
                rc = BENCH_ERROR;
        }

        double time;
        if (rc == OK)
            rc = elapsed_us(start, &time);
        tree_clear(&root);
        if (rc != OK)
            return rc;
        tree_total += time;

        for (size_t i = 0; i < data->size; ++i)
            if (strcmp(work[i].text, data->sorted[i].text) != 0)
                return BENCH_ERROR;

        memcpy(work, data->input, data->size * sizeof(*work));
        start = clock();
        qsort(work, data->size, sizeof(*work), compare_words);
        rc = elapsed_us(start, &time);
        if (rc != OK)
            return rc;
        array_total += time;
    }

    double tree_time = tree_total / BENCH_RUNS;
    double array_time = array_total / BENCH_RUNS;
    printf("%6zu %-10s %12.3f %12zu", data->size, "tree", tree_time, data->tree_bytes);
    print_relative_time(tree_time, array_time);
    printf("%6zu %-10s %12.3f %12zu", data->size, "qsort", array_time,
           sizeof(word_t *) + data->size * sizeof(word_t));
    print_relative_time(array_time, array_time);
    return OK;
}

// Готовит набор заданного размера и запускает один замер.
static int bench_dataset(size_t size, int (*measure)(const dataset_t *))
{
    word_t *words = calloc(3 * size, sizeof(*words));
    if (!words)
        return MEMORY_ERROR;

    word_t *input = words;
    word_t *sorted = words + size;
    word_t *work = words + 2 * size;
    make_words(input, size);
    memcpy(sorted, input, size * sizeof(*input));
    qsort(sorted, size, sizeof(*sorted), compare_words);
    shuffle_words(input, size);
    size_t tree_bytes = sizeof(node_t *) + size * sizeof(node_t);
    for (size_t i = 0; i < size; ++i)
        tree_bytes += strlen(input[i].text) + 1;
    const dataset_t data = {input, sorted, size, tree_bytes, work};

    int rc = measure(&data);

    free(words);
    return rc;
}

// Выводит одну таблицу, отделяя группы разных размеров.
static int bench_table(int (*measure)(const dataset_t *), int width)
{
    const size_t sizes[] = {100, 500, 1000, 2000};
    const char separator[] =
        "----------------------------------------------------------------------------------------------------";
    for (size_t i = 0; i < sizeof(sizes) / sizeof(sizes[0]); ++i)
    {
        if (i > 0)
            printf("%.*s\n", width, separator);
        int rc = bench_dataset(sizes[i], measure);
        if (rc != OK)
            return rc;
    }
    return OK;
}

// Запускает три таблицы бенчмарка.
int main(void)
{
    puts("Для файла указан объём на диске в байтах.");
    puts("forks — число узлов с двумя потомками.");

    printf("\nТочный поиск (balanced = 100%%)\n");
    int width = printf("%6s %-10s %10s %10s %12s %12s %12s %12s\n",
                       "n", "tree", "height", "forks", "time, us", "comparisons", "memory, B",
                       "time, %") - 1;
    int rc = bench_table(bench_search, width);
    if (rc == OK)
    {
        puts("\nПоиск всех слов на «a» (file = 100%)");
        width = printf("%6s %-10s %10s %12s %12s %12s %12s\n",
                       "n", "source", "matches", "time, us", "comparisons", "storage, B",
                       "time, %") - 1;
        rc = bench_table(bench_prefix, width);
    }
    if (rc == OK)
    {
        printf("\nСортировка (qsort = 100%%)\n");
        width = printf("%6s %-10s %12s %12s %12s\n",
                       "n", "sort", "time, us", "memory, B", "time, %") - 1;
        rc = bench_table(bench_sort, width);
    }
    if (rc != OK)
        print_error(rc);
    return rc;
}
