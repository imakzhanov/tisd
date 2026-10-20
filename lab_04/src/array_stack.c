#include <stdio.h>

#include "array_stack.h"
#include "errors.h"

void stack_init(stack_t *s)
{
    s->top1 = -1;
    s->top2 = STACK_SIZE;
}

int stack_is_empty(const stack_t *s, int num)
{
    if (num == 1)
        return s->top1 == -1;
    return s->top2 == STACK_SIZE;
}

int stack_is_full(const stack_t *s)
{
    return s->top1 + 1 == s->top2;
}

int stack_push(stack_t *s, int num, double value)
{
    if (num != 1 && num != 2)
        return ERR_STACK_NUM;
    if (stack_is_full(s))
        return ERR_OVERFLOW;

    if (num == 1)
        s->data[++s->top1] = value;
    else
        s->data[--s->top2] = value;
    return OK;
}

int stack_pop(stack_t *s, int num, double *value)
{
    if (num != 1 && num != 2)
        return ERR_STACK_NUM;
    if (stack_is_empty(s, num))
        return ERR_EMPTY;

    if (num == 1)
        *value = s->data[s->top1--];
    else
        *value = s->data[s->top2++];
    return OK;
}

// вывод от вершины к основанию, без извлечения
void stack_print(const stack_t *s, int num)
{
    printf("Стек %d (сверху вниз):", num);
    if (stack_is_empty(s, num))
        printf(" пуст");
    else if (num == 1)
        for (int i = s->top1; i >= 0; i--)
            printf(" %g", s->data[i]);
    else
        for (int i = s->top2; i < STACK_SIZE; i++)
            printf(" %g", s->data[i]);
    printf("\n");
}
