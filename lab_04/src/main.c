#include <stdint.h>
#include <stdio.h>

#include "array_stack.h"
#include "config.h"
#include "errors.h"
#include "io.h"
#include "list_stack.h"

#define MIN_CHOISE 0
#define MAX_CHOISE 9

void print_menu(void)
{
    printf("\n----- Лабораторная 4 -----\n");
    printf("Стеки в массиве (два стека, один в начале, другой в конце массива, макс. элементов %d):\n", STACK_SIZE);
    printf("1 - Добавить элемент в стек в начале массива\n");
    printf("2 - Добавить элемент в стек в конце массива\n");
    printf("3 - Удалить элемент из стека в начале массива\n");
    printf("4 - Удалить элемент из стека в конце массива\n");
    printf("5 - Показать состояние стеков\n");
    printf("Стек списком (макс. элементов %d):\n", LIST_MAX_COUNT);
    printf("6 - Добавить элемент\n");
    printf("7 - Удалить элемент\n");
    printf("8 - Показать состояние (с адресами элементов)\n");
    printf("9 - Показать список освобожденных адресов\n");
    printf("0 - Выход\n");
    printf("----------------------------\n");
}

int array_push(stack_t *s, int num)
{
    double value;
    int rc = read_double("Введите вещественное число", &value);

    if (rc == OK)
        rc = stack_push(s, num, value);
    if (rc == OK)
        printf("Элемент %g ДОБАВЛЕН в стек в %s массива\n", value, num == 1 ? "НАЧАЛЕ" : "КОНЦЕ");
    return rc;
}

int array_pop(stack_t *s, int num)
{
    double value;
    int rc = stack_pop(s, num, &value);

    if (rc == OK)
        printf("Элемент %g УДАЛЕН из стека в %s массива\n", value, num == 1 ? "НАЧАЛЕ" : "КОНЦЕ");
    return rc;
}

int list_push(list_stack_t *s)
{
    double value;
    int rc = read_double("Введите вещественное число", &value);

    if (rc == OK)
        rc = list_stack_push(s, value);
    if (rc == OK)
        printf("Элемент %g ДОБАВЛЕН\n", value);
    return rc;
}

int list_pop(list_stack_t *s)
{
    double value;
    int rc = list_stack_pop(s, &value);

    if (rc == OK)
        printf("Элемент %g УДАЛЕН\n", value);
    return rc;
}

int main(void)
{
    stack_t array_stack;
    list_stack_t list_stack;
    int choice, rc;
    int run = 1;

    stack_init(&array_stack);
    list_stack_init(&list_stack, LIST_MAX_COUNT, UINTPTR_MAX);

    while (run)
    {
        print_menu();
        rc = read_int("Выберите пункт", MIN_CHOISE, MAX_CHOISE, &choice);
        if (rc == ERR_EOF)
        {
            run = 0;
            continue;
        }
        if (rc != OK)
        {
            print_error(rc);
            continue;
        }

        printf("======================================================================================================\n");
        switch (choice)
        {
            case 1:
                rc = array_push(&array_stack, 1);
                break;
            case 2:
                rc = array_push(&array_stack, 2);
                break;
            case 3:
                rc = array_pop(&array_stack, 1);
                break;
            case 4:
                rc = array_pop(&array_stack, 2);
                break;
            case 5:
                stack_print(&array_stack, 1);
                stack_print(&array_stack, 2);
                break;
            case 6:
                rc = list_push(&list_stack);
                break;
            case 7:
                rc = list_pop(&list_stack);
                break;
            case 8:
                list_stack_print(&list_stack);
                break;
            case 9:
                list_stack_print_freed(&list_stack);
                break;
            case 0:
                run = 0;
                break;
        }
        if (rc == ERR_EOF)
            run = 0;
        else
            print_error(rc);
        printf("======================================================================================================\n");
    }
    list_stack_clear(&list_stack);

    return OK;
}
