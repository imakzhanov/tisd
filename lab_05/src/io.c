#include "io.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static int read_line(char *line, int size)
{
    if (!fgets(line, size, stdin))
        return 0;
    if (strchr(line, '\n') || feof(stdin))
        return 1;

    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
        ;
    return -1;
}

int read_choice(void)
{
    char line[32], choice, extra;
    int rc = read_line(line, sizeof(line));
    if (rc == 0)
        return '0';
    if (rc < 0 || sscanf(line, " %c %c", &choice, &extra) != 1)
        return -1;
    return choice;
}

static int read_range(const char *name, int type, range_t *range, int is_arrival)
{
    char line[128], extra;
    printf("%s, тип %d [%.3g %.3g], введите min max: ",
           name, type, range->min, range->max);
    if (read_line(line, sizeof(line)) != 1
        || sscanf(line, "%lf %lf %c", &range->min, &range->max, &extra) != 2
        || !isfinite(range->min) || !isfinite(range->max)
        || range->min < 0 || range->min > range->max
        || (is_arrival && range->max == 0))
        return INPUT_ERROR;
    return OK;
}

int read_params(params_t *params)
{
    params_t updated = *params;
    for (int i = 0; i < 2; i++)
        if (read_range("Интервал прихода", i + 1, &updated.arrival[i], 1) != OK
            || read_range("Время обслуживания", i + 1, &updated.service[i], 0) != OK)
            return INPUT_ERROR;
    *params = updated;
    return OK;
}

void print_progress(const result_t *result)
{
    printf("\nВышло заявок типа 1: %zu\n", result->queue[0].departed);
    for (int i = 0; i < 2; i++)
    {
        const queue_stats_t *q = &result->queue[i];
        double length = result->time > 0 ? q->area / result->time : 0;
        double wait = q->started ? q->wait / q->started : 0;
        printf("Тип %d: длина %zu, средняя %.3f, вошло %zu, вышло %zu, ожидание %.3f е.в.\n",
               i + 1, q->size, length, q->arrived, q->departed, wait);
    }
}

static double mean(range_t range)
{
    return range.min + (range.max - range.min) / 2;
}

static void print_comparison(const char *name, double actual, double expected)
{
    printf("%s: факт %.3f, расчёт %.3f е.в.", name, actual, expected);
    if (expected > 0)
        printf(", расхождение %.2f%%", 100 * fabs(actual - expected) / expected);
    else if (actual == 0)
        printf(", расхождение 0.00%%");
    else
        printf(", расхождение в процентах не определено");
    putchar('\n');
}

void print_result(const result_t *result, const params_t *params)
{
    printf("\nИтоги: модельное время %.3f е.в., простой ОА %.3f е.в.\n",
           result->time, result->idle);
    double expected_busy = 0;
    for (int i = 0; i < 2; i++)
    {
        const queue_stats_t *q = &result->queue[i];
        printf("Тип %d: вошло %zu, вышло %zu\n", i + 1, q->arrived, q->departed);
        print_comparison(i == 0 ? "По входу типа 1" : "По входу типа 2",
                         result->time, mean(params->arrival[i]) * q->arrived);
        expected_busy += mean(params->service[i]) * q->departed;
    }
    print_comparison("Время работы ОА", result->time - result->idle, expected_busy);
}

void print_error(int rc)
{
    const char *messages[] = {
        "", "Не удалось выделить память.", "Очередь пуста.", "Переполнение очереди.",
        "Некорректный ввод.", "Модельное время вышло за допустимый диапазон."
    };
    fprintf(stderr, "%s\n", messages[rc]);
}
