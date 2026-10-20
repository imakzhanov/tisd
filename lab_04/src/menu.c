#include <stdint.h>
#include <stdio.h>

#include "array_stack.h"
#include "config.h"
#include "errors.h"
#include "io.h"
#include "list_stack.h"
#include "menu.h"

#define EXIT_ITEM 0
#define MAX_ITEM 9

static void print_menu(void)
{
    printf("\n----- Работа со стеком -----\n");
    printf("Стеки в массиве (два стека, размер массива %d):\n", STACK_SIZE);
    printf("1 - Добавить элемент в стек 1\n");
    printf("2 - Добавить элемент в стек 2\n");
    printf("3 - Удалить элемент из стека 1\n");
    printf("4 - Удалить элемент из стека 2\n");
    printf("5 - Показать состояние стеков\n");
    printf("Стек списком (максимум элементов %d):\n", LIST_MAX_COUNT);
    printf("6 - Добавить элемент\n");
    printf("7 - Удалить элемент\n");
    printf("8 - Показать состояние (с адресами элементов)\n");
    printf("9 - Показать адреса освобожденных элементов\n");
    printf("0 - Выход\n");
    printf("----------------------------\n");
}

static int array_push(stack_t *s, int num)
{
    double value;
    int rc = read_double("Введите число", &value);

    if (rc == OK)
        rc = stack_push(s, num, value);
    if (rc == OK)
        printf("Элемент %g добавлен в стек %d\n", value, num);
    return rc;
}

static int array_pop(stack_t *s, int num)
{
    double value;
    int rc = stack_pop(s, num, &value);

    if (rc == OK)
        printf("Из стека %d удален элемент %g\n", num, value);
    return rc;
}

static int list_push(list_stack_t *s)
{
    double value;
    int rc = read_double("Введите число", &value);

    if (rc == OK)
        rc = list_stack_push(s, value);
    if (rc == OK)
        printf("Элемент %g добавлен\n", value);
    return rc;
}

static int list_pop(list_stack_t *s)
{
    double value;
    int rc = list_stack_pop(s, &value);

    if (rc == OK)
        printf("Удален элемент %g\n", value);
    return rc;
}

int run_menu(void)
{
    stack_t array;
    list_stack_t list;
    int choice, rc;

    stack_init(&array);
    list_stack_init(&list, LIST_MAX_COUNT, UINTPTR_MAX);

    do
    {
        print_menu();
        rc = read_int("Выберите пункт", EXIT_ITEM, MAX_ITEM, &choice);
        if (rc == ERR_EOF)
            break;
        if (rc != OK)
        {
            print_error(rc);
            choice = -1;
            continue;
        }

        switch (choice)
        {
            case 1:
                rc = array_push(&array, 1);
                break;
            case 2:
                rc = array_push(&array, 2);
                break;
            case 3:
                rc = array_pop(&array, 1);
                break;
            case 4:
                rc = array_pop(&array, 2);
                break;
            case 5:
                stack_print(&array, 1);
                stack_print(&array, 2);
                break;
            case 6:
                rc = list_push(&list);
                break;
            case 7:
                rc = list_pop(&list);
                break;
            case 8:
                list_stack_print(&list);
                break;
            case 9:
                list_stack_print_freed(&list);
                break;
            default:
                break;
        }
        if (rc == ERR_EOF)
            break;
        print_error(rc);
    } while (choice != EXIT_ITEM);

    list_stack_clear(&list);
    return OK;
}
