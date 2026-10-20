#include <stdio.h>

#include "errors.h"

void print_error(int rc)
{
    switch (rc)
    {
        case OK:
            break;
        case ERR_OVERFLOW:
            printf("Ошибка: Переполнение стека\n");
            break;
        case ERR_EMPTY:
            printf("Ошибка: Стек пуст\n");
            break;
        case ERR_STACK_NUM:
            printf("Ошибка: Неверный номер стека\n");
            break;
        case ERR_MEMORY:
            printf("Ошибка: Не удалось выделить память\n");
            break;
        case ERR_INPUT:
            printf("Ошибка: Неверный формат ввода\n");
            break;
        case ERR_RANGE:
            printf("Ошибка: Значение вне допустимого диапазона\n");
            break;
        case ERR_EOF:
            printf("Ошибка: Ввод завершен\n");
            break;
        default:
            printf("Неизвестная ошибка\n");
    }
}
