#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "menu.h"
#include "file_io.h"
#include "sort.h"
#include "search.h"
#include "bench.h"
#include "errors.h"

void read_line(char *buf, size_t bufsize)
{
    if (fgets(buf, (int)bufsize, stdin) == NULL)
        buf[0] = '\0';

    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n')
        buf[len - 1] = '\0';
}

int input_int_with_range(char *text, int min, int max)
{
    char line[128];
    while (1)
    {
        printf("%s [от %d до %d]: ", text, min, max);
        read_line(line, sizeof(line));
        if (line[0] == '\0')
        {
            printf("Пустой ввод, повторите\n");
            continue;
        }
        int value;
        char extra;
        if (sscanf(line, "%d %c", &value, &extra) != 1)
        {
            printf("Неверный формат числа, повторите\n");
            continue;
        }
        if (value < min || value > max)
        {
            printf("Значение вне диапазона [от %d до %d], повторите\n", min, max);
            continue;
        }
        return value;
    }
}

double input_double_with_range(char *text, double min, double max)
{
    char line[128];
    while (1)
    {
        printf("%s [от %.2f до %.2f]: ", text, min, max);
        read_line(line, sizeof(line));
        if (line[0] == '\0')
        {
            printf("Пустой ввод, повторите\n");
            continue;
        }
        double value;
        char extra;
        if (sscanf(line, "%lf %c", &value, &extra) != 1)
        {
            printf("Неверный формат числа, повторите\n");
            continue;
        }
        if (value < min || value > max)
        {
            printf("Значение вне диапазона [от %.2f до %.2f], повторите\n", min, max);
            continue;
        }
        return value;
    }
}

void input_string(char *text, char *buf, int maxlen)
{
    while (1)
    {
        printf("%s (до %d символов, без ';'): ", text, maxlen - 1);
        read_line(buf, maxlen);
        if (buf[0] == '\0')
        {
            printf("Пустой ввод, повторите\n");
            continue;
        }
        if (strchr(buf, ';') != NULL)
        {
            printf("Символ ';' недопустим, повторите\n");
            continue;
        }
        return;
    }
}

void input_flat(Flat *f)
{
    memset(f, 0, sizeof(Flat));
    input_string("Адрес", f->address, MAX_ADDRESS_LEN);
    f->area = input_double_with_range("Площадь, м2", 1.0, 10000.0);
    f->rooms = input_int_with_range("Количество комнат", 1, 10);
    f->price_m2 = input_double_with_range("Стоимость за 1 м2", 1.0, 10000000.0);

    int type_choice = input_int_with_range("Тип жилья (0 - первичное, 1 - вторичное)", 0, 1);
    if (type_choice == 0)
    {
        f->type = PRIMARY;
        f->variable.primary.has_finishing =
            input_int_with_range("С отделкой? (1 - да, 0 - нет)", 0, 1);
    }
    else
    {
        f->type = SECONDARY;
        f->variable.secondary.build_year =
            input_int_with_range("Год постройки", 1900, 2026);
        f->variable.secondary.owners =
            input_int_with_range("Количество предыдущих собственников", 0, 50);
        f->variable.secondary.had_pets =
            input_int_with_range("Были ли животные? (1 - да, 0 - нет)", 0, 1);
    }
}

void menu_table_add(Table *table, int *is_key_actual)
{
    Flat f;
    input_flat(&f);
    if (table_add(table, f) == OK)
    {
        printf("Запись добавлена в конец таблицы.\n");
        *is_key_actual = 0;
    }
    else
        printf("Ошибка выделения памяти\n");
}

void menu_table_remove(Table *table, int *is_key_actual)
{
    printf("Удаление записи по значению поля.\n");
    printf("1 - Адрес\n2 - Площадь\n3 - Количество комнат\n4 - Цена за м2\n");
    int choice = input_int_with_range("Выберите поле", 1, 4);

    int removed = 0;
    int rc = OK;
    switch (choice)
    {
        case 1:
        {
            char address[MAX_ADDRESS_LEN];
            input_string("Адрес для удаления", address, MAX_ADDRESS_LEN);
            rc = table_remove_by_field(table, ADDRESS_FIELD, address, &removed);
            break;
        }
        case 2:
        {
            double area = input_double_with_range("Площадь для удаления", 0.0, 100000.0);
            rc = table_remove_by_field(table, AREA_FIELD, &area, &removed);
            break;
        }
        case 3:
        {
            int rooms = input_int_with_range("Количество комнат для удаления", 0, 100);
            rc = table_remove_by_field(table, ROOMS_FIELD, &rooms, &removed);
            break;
        }
        case 4:
        {
            double price = input_double_with_range("Цена за м2 для удаления", 0.0, 100000000.0);
            rc = table_remove_by_field(table, PRICE_M2_FIELD, &price, &removed);
            break;
        }
    }

    if (rc == OK)
    {
        printf("Удалено записей: %d.\n", removed);
        *is_key_actual = 0;
    }
    else
        printf("Записи с таким значением поля не найдены.\n");
}


void menu_print_table_by_key(Table *table, KeyTable *key_table, int *is_key_actual)
{
    if (!*is_key_actual)
    {
        printf("Таблица ключей неактуальна, пересоздайте её.\n");
        return;
    }
    printf("Таблица по порядку ключей:\n");
    print_table_by_key(key_table, table);
}

void menu_sort_table(Table *table, int *is_key_actual)
{
    int algorithm = input_int_with_range("Алгоритм (1 - сортировка пузырьком, 2 - быстрая сортировка)", 1, 2);
    if (algorithm == 1)
        sort_bubble_table(table);
    else
        sort_quick_table(table);
    printf("Таблица отсортирована по цене за м2.\n");
    *is_key_actual = 0;
}

void menu_sort_keys(KeyTable *key_table, int *is_key_actual)
{
    if (!*is_key_actual)
    {
        printf("Таблица ключей неактуальна, пересоздайте её.\n");
        return;
    }
    int algorithm = input_int_with_range("Алгоритм (1 - сортировка пузырьком, 2 - быстрая сортировка)", 1, 2);
    if (algorithm == 1)
        sort_bubble_keys(key_table);
    else
        sort_quick_keys(key_table);
    printf("Таблица ключей отсортирована:\n");
}


void menu_search_by_task(Table *table)
{
    printf("Поиск: вторичная, 2-комнатная, без животных, в диапазоне цен за м2.\n");
    double min_price = input_double_with_range("Минимальная цена за м2", 0.0, 100000000.0);
    double max_price = input_double_with_range("Максимальная цена за м2", min_price, 100000000.0);

    Table out;
    table_init(&out);
    int rc = search_by_task(table, min_price, max_price, &out);

    if (rc == OK)
    {
        printf("Найдено записей: %d\n", out.count);
        print_table(&out);
    }
    else
        printf("Записи не найдены.\n");
    table_free(&out);
}

void menu_benchmark(Table *table)
{
    int rc = make_report("report.txt", table);
    if (rc == OK)
        printf("Отчёт сохранён в report.txt\n");
    else if (rc == ERR_EMPTY_TABLE)
        printf("Таблица пуста, замер невозможен.\n");
    else
        printf("Не удалось построить отчёт\n");
}

void print_main_menu(void)
{
    printf("\n======== МЕНЮ ========\n");
    printf("1 - Просмотреть основную таблицу\n");
    printf("2 - Просмотреть таблицу ключей\n");
    printf("3 - Просмотреть таблицу по порядку ключей\n");
    printf("4 - Актуализировать таблицу ключей\n");
    printf("5 - Добавить запись\n");
    printf("6 - Удалить запись по значению поля\n");
    printf("7 - Отсортировать основную таблицу\n");
    printf("8 - Отсортировать таблицу ключей\n");
    printf("9 - Поиск: вторичное, 2-комнатная, без животных\n");
    printf("10 - Оценка эффективности алгоритмов\n");
    printf("0 - Сохранить и выйти\n");
}

void run_menu(Table *table, KeyTable *key_table, int *is_key_actual, char *output_filename)
{   
    int run = 1;
    while (run)
    {
        print_main_menu();
        int choice = input_int_with_range("Выберите пункт меню", 0, 10);
        switch (choice)
        {
            case 1: 
                print_table(table);
                break;
            case 2: 
                print_key_table(key_table);
                break;
            case 3:
                menu_print_table_by_key(table, key_table, is_key_actual); 
                break;
            case 4: 
                key_table_free(key_table);
                build_key_table(table, key_table);
                *is_key_actual = 1;
                break;
            case 5: 
                menu_table_add(table, is_key_actual); 
                break;
            case 6: 
                menu_table_remove(table, is_key_actual); 
                break;
            case 7: 
                menu_sort_table(table, is_key_actual); 
                break;
            case 8: 
                menu_sort_keys(key_table, is_key_actual); 
                break;
                break;
            case 9: 
                menu_search_by_task(table); 
                break;
            case 10: 
                menu_benchmark(table); 
                break;
            case 0:
                if (save_table(output_filename, table) == OK)
                    printf("Таблица сохранена в %s\n", output_filename);
                else
                    printf("Не удалось сохранить %s\n", output_filename);
                run = 0;
                break;
        }
    }
}
