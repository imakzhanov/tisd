#ifndef LIST_STACK_H
#define LIST_STACK_H

#include <stdint.h>

#include "config.h"

// узел стека
typedef struct node
{
    double value;
    struct node *prev;
} node_t;

// Структура стека
typedef struct
{
    node_t *top; // Указатель на последний элемент, если NULL, стек пустой
    int count;
    int max_count;
    uintptr_t max_addr; // адрес узла выше этого значения - переполнение
} list_stack_t;

// адреса освобожденных узлов, хранятся последние MAX_FREED адресов
typedef struct
{
    void *addr[MAX_FREED];
    int count;
} freed_t;

void list_stack_init(list_stack_t *s, int max_count, uintptr_t max_addr);
// ограничение действует только на новые push, уже лежащие элементы остаются
void list_stack_set_max_addr(list_stack_t *s, uintptr_t max_addr);

int list_stack_is_empty(list_stack_t *s);
int list_stack_is_full(list_stack_t *s);

int list_stack_push(list_stack_t *s, double value);
int list_stack_pop(list_stack_t *s, double *value);

// от вершины к основанию, без извлечения
void list_stack_print(list_stack_t *s);
void list_stack_clear(list_stack_t *s);


void freed_add(freed_t *f, void *addr);

void freed_print(freed_t *f);

#endif
