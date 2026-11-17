#include "io.h"
#include <time.h>
#include <stdio.h>

#include "arr_queue.h"
#include "list_queue.h"

// ручная работа с очередью (и массив и список)
static void manual_queue(queue_kind_t kind)
{
    arr_queue_t array = {0};
    list_queue_t list = {0};
    list.show_addresses = 1;

    printf("\nОчередь очищается при выборе назад.\n");
    for (;;)
    {
        printf("\n1. Добавить\n2. Удалить\n3. Показать\n0. Назад\n> ");
        int choice = read_choice();
        int rc = OK;
        double value;
        if (choice == '0')
            break;

        switch (choice)
        {
            case '1':
                rc = read_value(&value);
                if (rc == OK)
                    rc = kind == QUEUE_LIST ? list_push(&list, value) : arr_push(&array, value);
                break;
            case '2':
                rc = kind == QUEUE_LIST ? list_pop(&list, &value) : arr_pop(&array, &value);
                if (rc == OK)
                    printf("Извлечено: %g\n", value);
                break;
            case '3':
                if (kind == QUEUE_LIST)
                {
                    printf("Длина %zu, p_out = %p, p_in = %p\n",
                           list.size, (void *)list.p_out, (void *)list.p_in);
                    for (node_t *node = list.p_out; node; node = node->next)
                        printf("%g [%p]\n", node->value, (void *)node);
                }
                else
                {
                    printf("Длина %zu, p_out = %zu, p_in = %zu\n",
                           array.size, array.p_out, array.p_in);
                    for (size_t i = 0; i < array.size; i++)
                        printf("%g ", array.values[(array.p_out + i) % QUEUE_CAPACITY]);
                    putchar('\n');
                }
                break;
            default:
                rc = INPUT_ERROR;
        }
        if (rc != OK)
            print_error(rc);
    }

    list.show_addresses = 0;
    list_clear(&list);
}

static void manual_menu(void)
{
    for (;;)
    {
        printf("\nРучная работа\n1. Список\n2. Массив\n0. Назад\n> ");
        int choice = read_choice();
        if (choice == '0')
            return;
        if (choice == '1' || choice == '2')
            manual_queue(choice == '1' ? QUEUE_LIST : QUEUE_ARRAY);
        else
            print_error(INPUT_ERROR);
    }
}

static void print_menu(int show_addresses)
{
    printf("\n");
    printf("1. Массив\n");
    printf("2. Список\n");
    printf("3. Изменить параметры\n");
    printf("4. Адреса узлов списка (%s)\n", show_addresses ? "вкл" : "выкл");
    printf("5. Ручная работа\n");
    printf("0. Выход\n");
    printf("> ");
}

int main(void)
{
    //случайный сид чтобы генерировать разные времена для разных запусков 
    unsigned seed = (unsigned)time(NULL);

    params_t params = default_params;
    int show_addresses = 0;

    for (;;)
    {
        print_menu(show_addresses);
        int choice = read_choice();
        int rc = OK;
        switch (choice)
        {
            case '0':
                return OK;
            case '1':
            case '2':
            {
                result_t result;
                queue_kind_t kind = choice == '1' ? QUEUE_ARRAY : QUEUE_LIST;
                rc = simulate(&params, kind, seed, show_addresses, print_progress, &result);
                if (rc == OK)
                    print_result(&result, &params);
                break;
            }
            case '3':
                rc = read_params(&params);
                break;
            case '4':
                show_addresses = !show_addresses;
                break;
            case '5':
                manual_menu();
                break;
            default:
                rc = INPUT_ERROR;
        }
        if (rc != OK)
            print_error(rc);
    }
}
