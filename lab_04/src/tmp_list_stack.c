#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define OK 0
#define ERR_OVERFLOW 1
#define ERR_EMPTY 2
#define ERR_MEMORY 3

#define MAX_FREED 20

typedef struct node
{
    double value;
    struct node *next;
} node_t;

// дескриптор стека-списка
typedef struct
{
    node_t *top;     // вершина, NULL если стек пуст
    int count;       // текущее число элементов
    int max_count;   // максимальное число элементов
    uintptr_t max_addr; // адрес узла выше этого значения - переполнение
    void *freed[MAX_FREED]; // адреса освобожденных узлов
    int freed_count;
} list_stack_t;

void list_stack_init(list_stack_t *s, int max_count, uintptr_t max_addr)
{
    s->top = NULL;
    s->count = 0;
    s->max_count = max_count;
    s->max_addr = max_addr;
    s->freed_count = 0;
}

int list_stack_is_empty(const list_stack_t *s)
{
    return s->top == NULL;
}

int list_stack_is_full(const list_stack_t *s)
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
    node->next = s->top;
    s->top = node;
    s->count++;
    return OK;
}

// запоминает адрес, при заполнении массива самый старый адрес вытесняется
static void remember_freed(list_stack_t *s, void *addr)
{
    if (s->freed_count == MAX_FREED)
    {
        for (int i = 1; i < MAX_FREED; i++)
            s->freed[i - 1] = s->freed[i];
        s->freed_count--;
    }
    s->freed[s->freed_count++] = addr;
}

int list_stack_pop(list_stack_t *s, double *value)
{
    if (list_stack_is_empty(s))
        return ERR_EMPTY;

    node_t *old = s->top;
    *value = old->value;
    s->top = old->next;
    s->count--;

    remember_freed(s, old);
    free(old);
    return OK;
}

// от вершины к основанию, без извлечения
void list_stack_print(const list_stack_t *s)
{
    printf("Стек списком (сверху вниз), элементов: %d\n", s->count);
    if (list_stack_is_empty(s))
        printf("  пуст\n");
    for (const node_t *cur = s->top; cur != NULL; cur = cur->next)
        printf("  %g, адрес %p\n", cur->value, (void *)cur);
}

void list_stack_print_freed(const list_stack_t *s)
{
    printf("Освобожденные адреса (от старых к новым):\n");
    if (s->freed_count == 0)
        printf("  нет\n");
    for (int i = 0; i < s->freed_count; i++)
        printf("  %p\n", s->freed[i]);
}

void list_stack_clear(list_stack_t *s)
{
    double value;

    while (list_stack_pop(s, &value) == OK)
        ;
}

int main(void)
{
    list_stack_t s;
    double value;

    list_stack_init(&s, 5, UINTPTR_MAX);

    for (int i = 1; i <= 5; i++)
        list_stack_push(&s, i);
    list_stack_print(&s);
    printf("Переполнение при push: %d\n", list_stack_push(&s, 100) == ERR_OVERFLOW);

    list_stack_pop(&s, &value);
    list_stack_pop(&s, &value);
    list_stack_print_freed(&s);

    list_stack_push(&s, 10);
    list_stack_push(&s, 20);
    list_stack_print(&s);

    list_stack_clear(&s);
    printf("Пустой стек, pop: %d\n", list_stack_pop(&s, &value) == ERR_EMPTY);
    list_stack_print_freed(&s);

    return 0;
}
