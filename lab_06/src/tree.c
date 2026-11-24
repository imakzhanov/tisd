#include "tree.h"

#include <stdlib.h>
#include <string.h>

// Вставляет слово, направляя одинаковые ключи вправо.
int tree_insert(node_t **root, const char *word)
{
    size_t length = strlen(word);
    if (length == 0)
        return INPUT_ERROR;
    if (length >= WORD_CAPACITY)
        return WORD_TOO_LONG;

    node_t **link = root;
    while (*link)
    {
        if (strcmp(word, (*link)->word) < 0)
            link = &(*link)->left;
        else
            link = &(*link)->right;
    }

    node_t *node = malloc(sizeof(*node));
    if (!node)
        return MEMORY_ERROR;
    node->word = malloc(length + 1);
    if (!node->word)
    {
        free(node);
        return MEMORY_ERROR;
    }
    memcpy(node->word, word, length + 1);
    node->left = NULL;
    node->right = NULL;
    *link = node;
    return OK;
}

// Ищет точное слово и при необходимости считает сравнения.
const node_t *tree_find(const node_t *root, const char *word, size_t *comparisons)
{
    while (root)
    {
        int cmp = strcmp(word, root->word);
        if (comparisons)
            ++*comparisons;
        if (cmp == 0)
            return root;
        root = cmp < 0 ? root->left : root->right;
    }
    return NULL;
}

// Удаляет одно вхождение слова и восстанавливает связи дерева.
int tree_remove(node_t **root, const char *word)
{
    if (!*root)
        return EMPTY_TREE;

    node_t **link = root;
    while (*link)
    {
        int cmp = strcmp(word, (*link)->word);
        if (cmp == 0)
            break;
        link = cmp < 0 ? &(*link)->left : &(*link)->right;
    }
    if (!*link)
        return NOT_FOUND;

    node_t *removed = *link;
    if (!removed->left)
        *link = removed->right;
    else if (!removed->right)
        *link = removed->left;
    else
    {
        node_t **min_link = &removed->right;
        while ((*min_link)->left)
            min_link = &(*min_link)->left;

        node_t *successor = *min_link;
        *min_link = successor->right;
        successor->left = removed->left;
        successor->right = removed->right;
        *link = successor;
    }
    free(removed->word);
    free(removed);
    return OK;
}

// Обходит дерево в заданном порядке и вызывает обработчик для каждого слова.
void tree_traverse(const node_t *root, order_t order,
                   void (*visit)(const char *, void *), void *context)
{
    if (!root)
        return;
    if (order == PREORDER)
        visit(root->word, context);
    tree_traverse(root->left, order, visit, context);
    if (order == INORDER)
        visit(root->word, context);
    tree_traverse(root->right, order, visit, context);
    if (order == POSTORDER)
        visit(root->word, context);
}

// Считает совпадения в поддереве и при необходимости учитывает сравнения.
static size_t count_prefix(const node_t *root, const char *letter, size_t length,
                           size_t *comparisons)
{
    size_t count = 0;
    while (root)
    {
        int cmp = strncmp(root->word, letter, length);
        if (comparisons)
            ++*comparisons;
        if (cmp == 0)
        {
            ++count;
            count += count_prefix(root->left, letter, length, comparisons);
        }
        root = cmp > 0 ? root->left : root->right;
    }
    return count;
}

// Считает узлы со словами на заданную букву, пропуская неподходящие ветви.
size_t tree_count_prefix(const node_t *root, const char *letter, size_t *comparisons)
{
    return count_prefix(root, letter, strlen(letter), comparisons);
}

// Вычисляет число узлов, уровней и узлов с двумя потомками.
tree_stats_t tree_stats(const node_t *root)
{
    if (!root)
        return (tree_stats_t){0, 0, 0};
    tree_stats_t left = tree_stats(root->left);
    tree_stats_t right = tree_stats(root->right);
    return (tree_stats_t){
        left.nodes + right.nodes + 1,
        (left.levels > right.levels ? left.levels : right.levels) + 1,
        left.forks + right.forks + (root->left && root->right)
    };
}

// Освобождает все узлы и обнуляет корень.
void tree_clear(node_t **root)
{
    node_t *node = *root;
    while (node)
    {
        if (node->left)
        {
            // Правый поворот поднимает левого потомка, сохраняя оба поддерева.
            node_t *left = node->left;
            node->left = left->right;
            left->right = node;
            node = left;
        }
        else
        {
            node_t *next = node->right;
            free(node->word);
            free(node);
            node = next;
        }
    }
    *root = NULL;
}
