#include <stdio.h>

#include "errors.h"

void print_error(int rc)
{
    switch (rc)
    {
        case OK:
            break;
        case ERR_INPUT:
            printf("Ошибка: неверный формат ввода\n");
            break;
        case ERR_EMPTY:
            printf("Ошибка: пустой ввод\n");
            break;
        case ERR_RANGE:
            printf("Ошибка: значение вне допустимого диапазона\n");
            break;
        case ERR_ZERO:
            printf("Ошибка: значение элемента не может быть нулём\n");
            break;
        case ERR_DUPLICATE:
            printf("Ошибка: элемент с такими координатами уже введён\n");
            break;
        case ERR_FILE_OPEN:
            printf("Ошибка: не удалось открыть файл\n");
            break;
        case ERR_FILE_FORMAT:
            printf("Ошибка: неверный формат файла\n");
            break;
        case ERR_FILE_WRITE:
            printf("Ошибка: не удалось записать в файл\n");
            break;
        case ERR_MEMORY:
            printf("Ошибка: не удалось выделить память\n");
            break;
        case ERR_SIZE:
            printf("Ошибка: размеры матриц не совпадают\n");
            break;
        case ERR_NO_DATA:
            printf("Ошибка: сначала введите матрицы A и B\n");
            break;
        default:
            printf("Неизвестная ошибка (код %d)\n", rc);
    }
}
