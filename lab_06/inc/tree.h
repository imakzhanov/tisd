#ifndef TREE_H
#define TREE_H

#include <stddef.h>

#include "config.h"
#include "errors.h"

typedef struct node
{
    char *word;
    struct node *left;
    struct node *right;
} node_t;

typedef enum
{
    PREORDER,
    INORDER,
    POSTORDER
} order_t;

typedef struct
{
    size_t nodes;
    size_t levels;
    size_t forks;
} tree_stats_t;

int tree_insert(node_t **root, const char *word);
int tree_remove(node_t **root, const char *word);
const node_t *tree_find(const node_t *root, const char *word, size_t *comparisons);
void tree_traverse(const node_t *root, order_t order,
                   void (*visit)(const char *, void *), void *context);
size_t tree_count_prefix(const node_t *root, const char *letter, size_t *comparisons);
tree_stats_t tree_stats(const node_t *root);
void tree_clear(node_t **root);

#endif
