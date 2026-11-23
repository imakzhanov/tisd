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
} dataset_t;

static int compare_words(const void *left, const void *right)
{
    const word_t *a = left;
    const word_t *b = right;
    return strcmp(a->text, b->text);
}

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

static int elapsed_us(clock_t start, double *time)
{
    clock_t end = clock();
    if (start == (clock_t)-1 || end == (clock_t)-1 || end < start)
        return TIME_ERROR;
    *time = (double)(end - start) * 1e6 / CLOCKS_PER_SEC;
    return OK;
}

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

static int bench_search(const dataset_t *data)
{
    const char *names[] = {"balanced", "shuffled", "chain"};
    printf("\nТочный поиск (%d запросов, поровну найденных и отсутствующих)\n", SEARCH_RUNS);
    printf("%-10s %6s %10s %10s %12s %12s %12s\n",
           "tree", "n", "height", "forks", "time, us", "comparisons", "memory, B");

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
            printf("%-10s %6zu %10zu %10zu %12.3f %12.2f %12zu\n",
                   names[shape], stats.nodes, stats.levels - 1, stats.forks, time,
                   comparisons, sizeof(root) + stats.nodes * sizeof(*root));
        }
        tree_clear(&root);
        if (rc != OK)
            return rc;
    }
    return OK;
}

static int write_words(FILE *file, const dataset_t *data)
{
    for (size_t i = 0; i < data->size; ++i)
        if (fprintf(file, "%s\n", data->input[i].text) < 0)
            return FILE_ERROR;

    if (fflush(file) == EOF)
        return FILE_ERROR;
    return OK;
}

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

static int bench_prefix(const dataset_t *data, FILE *file)
{
    node_t *root = NULL;
    int rc = build_tree(&root, data->input, data->size);
    if (rc != OK)
    {
        tree_clear(&root);
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
    if (rc == OK)
    {
        printf("\nПоиск всех слов на «%s»: %zu совпадений\n", letter, count);
        printf("%-10s %12s %12s %12s\n",
               "source", "time, us", "comparisons", "storage, B");
        printf("%-10s %12.3f %12.2f %12zu\n", "tree", tree_time / SEARCH_RUNS,
               (double)tree_comparisons / SEARCH_RUNS, sizeof(root) + data->size * sizeof(*root));
        printf("%-10s %12.3f %12.2f %12ld\n", "file", file_time,
               (double)file_comparisons / BENCH_RUNS, file_bytes);
    }

    tree_clear(&root);
    return rc;
}

static void save_sorted_word(const char *word, void *context)
{
    word_t **next = context;
    strcpy((*next)->text, word);
    ++*next;
}

static int bench_sort(const dataset_t *data, word_t *work)
{
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

    printf("\nСортировка (%d повторов). Память хранения без выходного массива дерева\n", BENCH_RUNS);
    printf("%-10s %12s %12s\n", "sort", "time, us", "memory, B");
    printf("%-10s %12.3f %12zu\n", "tree", tree_total / BENCH_RUNS,
           sizeof(node_t *) + data->size * sizeof(node_t));
    printf("%-10s %12.3f %12zu\n", "qsort", array_total / BENCH_RUNS,
           sizeof(word_t *) + data->size * sizeof(word_t));
    return OK;
}

static int bench_dataset(size_t size)
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
    const dataset_t data = {input, sorted, size};

    int rc = bench_search(&data);
    if (rc == OK)
    {
        FILE *file = tmpfile();
        if (!file)
            rc = FILE_ERROR;
        else
        {
            rc = bench_prefix(&data, file);
            if (fclose(file) == EOF && rc == OK)
                rc = FILE_ERROR;
        }
    }
    if (rc == OK)
        rc = bench_sort(&data, work);

    free(words);
    return rc;
}

int main(void)
{
    const size_t sizes[] = {100, 500, 1000, 2000};
    puts("Память указана без расходов аллокатора. Для файла указан объём на диске в байтах.");
    puts("forks — число узлов с двумя потомками.");
    int rc = OK;
    for (size_t i = 0; i < sizeof(sizes) / sizeof(sizes[0]) && rc == OK; ++i)
        rc = bench_dataset(sizes[i]);

    if (rc != OK)
        print_error(rc);
    return rc;
}
