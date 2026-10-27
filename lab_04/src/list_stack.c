#include <stdio.h>
#include <stdlib.h>

#include "errors.h"
#include "list_stack.h"


void list_stack_init(list_stack_t *s, int max_count, uintptr_t max_addr)
{
    s->top = NULL;
    s->count = 0;
    s->max_count = max_count;
    s->max_addr = max_addr;
}

void list_stack_set_max_addr(list_stack_t *s, uintptr_t max_addr)
{
    s->max_addr = max_addr;
}

int list_stack_is_empty(list_stack_t *s)
{
    return s->top == NULL;
}

int list_stack_is_full(list_stack_t *s)
{
    return s->count >= s->max_count;
}

int list_stack_push(list_stack_t *s, double value)
{
    if (list_stack_is_full(s))
        return ERR_OVERFLOW;

    node_t *node = malloc(sizeof(node_t));
    if (node == NULL)
        return ERR_MEMORY;

    if ((uintptr_t)node > s->max_addr)
    {
        free(node);
        return ERR_OVERFLOW;
    }

    node->value = value;
    node->prev = s->top;
    s->top = node;
    s->count++;
    return OK;
}

int list_stack_pop(list_stack_t *s, double *value)
{
    if (list_stack_is_empty(s))
        return ERR_EMPTY;

    node_t *old = s->top;
    *value = old->value;
    s->top = old->prev;
    s->count--;

    free(old);
    return OK;
}

// вывод от вершины к основанию, без извлечения
void list_stack_print(list_stack_t *s)
{
    printf("Стек списком (сверху вниз), элементов: %d, макс. адрес: %p\n", s->count, (void *)s->max_addr);
    if (list_stack_is_empty(s))
        printf("  пуст\n");
    for (node_t *cur = s->top; cur != NULL; cur = cur->prev)
        printf("  %g, адрес %p\n", cur->value, (void *)cur);
}

void list_stack_clear(list_stack_t *s)
{
    double value;

    int rc = list_stack_pop(s, &value);
    while (rc == OK)
        rc = list_stack_pop(s, &value);
}

void freed_add(freed_t *f, void *addr)
{
    if (f->count == MAX_FREED)
    {
        for (int i = 1; i < MAX_FREED; i++)
            f->addr[i - 1] = f->addr[i];
        f->count--;
    }
    f->addr[f->count++] = addr;
}

void freed_print(freed_t *f)
{
    printf("Освобожденные адреса (от старых к новым): \n");
    if (f->count == 0)
        printf("пусто\n");
    for (int i = 0; i < f->count; i++)
        printf("%d)     %p\n", i + 1, f->addr[i]);
}
