#include <stdio.h>

#include "errors.h"

void print_error(int rc)
{
    switch (rc)
    {
        case OK:
            break;
        case ERR_OVERFLOW:
            printf("Ошибка: переполнение стека\n");
            break;
        case ERR_EMPTY:
            printf("Ошибка: стек пуст\n");
            break;
        case ERR_STACK_NUM:
            printf("Ошибка: неверный номер стека\n");
            break;
        case ERR_MEMORY:
            printf("Ошибка: не удалось выделить память\n");
            break;
        case ERR_INPUT:
            printf("Ошибка: неверный формат ввода\n");
            break;
        case ERR_RANGE:
            printf("Ошибка: значение вне допустимого диапазона\n");
            break;
        case ERR_EOF:
            printf("Ошибка: ввод завершен\n");
            break;
        default:
            printf("Неизвестная ошибка\n");
    }
}
