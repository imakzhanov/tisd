#ifndef ARRAY_STACK_H
#define ARRAY_STACK_H

#include "config.h"

// два стека в одном массиве. Первый растет из начала вправо, второй из конца влево
typedef struct
{
    double data[STACK_SIZE];
    int top1;
    int top2;
} stack_t;

void stack_init(stack_t *s);

int stack_is_empty(stack_t *s, int num);
int stack_is_full(stack_t *s);

int stack_push(stack_t *s, int num, double value);
int stack_pop(stack_t *s, int num, double *value);

// вывод стека от вершины к основанию
void stack_print(stack_t *s, int num);

#endif
