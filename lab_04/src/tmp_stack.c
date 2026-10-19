#include <stdio.h>

#define STACK_SIZE 10

#define OK 0
#define ERR_OVERFLOW 1
#define ERR_EMPTY 2
#define ERR_STACK_NUM 3

// два стека в одном массиве: первый растет с начала вправо, второй с конца влево
typedef struct
{
    double data[STACK_SIZE];
    int top1; // индекс вершины первого стека, -1 если пуст
    int top2; // индекс вершины второго стека, STACK_SIZE если пуст
} stack_t;

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

int main(void)
{
    stack_t s;
    double value;

    stack_init(&s);
    for (int i = 1; i <= 6; i++)
        stack_push(&s, 1, i);
    for (int i = 1; i <= 4; i++)
        stack_push(&s, 2, i * 1.5);

    stack_print(&s, 1);
    stack_print(&s, 2);
    printf("Переполнение при push: %d\n", stack_push(&s, 1, 100) == ERR_OVERFLOW);

    stack_pop(&s, 2, &value);
    printf("pop из второго: %g\n", value);
    stack_push(&s, 1, 7);
    stack_print(&s, 1);
    stack_print(&s, 2);

    return 0;
}

