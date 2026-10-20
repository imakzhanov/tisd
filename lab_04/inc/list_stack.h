#ifndef LIST_STACK_H
#define LIST_STACK_H

#include <stdint.h>

#include "config.h"

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

void list_stack_init(list_stack_t *s, int max_count, uintptr_t max_addr);
int list_stack_is_empty(const list_stack_t *s);
int list_stack_is_full(const list_stack_t *s);
int list_stack_push(list_stack_t *s, double value);
int list_stack_pop(list_stack_t *s, double *value);
// от вершины к основанию, без извлечения
void list_stack_print(const list_stack_t *s);
void list_stack_print_freed(const list_stack_t *s);
void list_stack_clear(list_stack_t *s);

#endif
