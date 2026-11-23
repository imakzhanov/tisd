#include "io.h"

static void print_menu(void)
{
    printf("\n");
    printf("1. Загрузить слова из файла (заменить дерево)\n");
    printf("2. Показать дерево\n");
    printf("3. Посчитать и выделить слова на заданную букву\n");
    printf("4. Добавить слово\n");
    printf("5. Удалить одно вхождение слова\n");
    printf("6. Найти точное слово\n");
    printf("7. Показать обходы дерева\n");
    printf("8. Очистить дерево\n");
    printf("0. Выход\n> ");
}

static int load_words(node_t **root)
{
    char path[PATH_CAPACITY];
    int rc = read_file_path(path);
    if (rc == OK)
        rc = load_tree(path, root);
    if (rc == OK)
    {
        tree_stats_t stats = tree_stats(*root);
        printf("Загружено узлов: %zu. Высота дерева: %zu.\n", stats.nodes, stats.levels - 1);
    }
    return rc;
}

static int find_by_letter(const node_t *root)
{
    if (!root)
        return EMPTY_TREE;

    char letter[WORD_CAPACITY];
    int rc = read_key(letter, 1);
    if (rc == OK)
    {
        size_t count = tree_count_prefix(root, letter, NULL);
        printf("Узлов со словами на «%s»: %zu.\n", letter, count);
        print_tree(root, letter);
    }
    return rc;
}

static int change_word(node_t **root, int choice)
{
    if (!*root && choice != '4')
        return EMPTY_TREE;

    char word[WORD_CAPACITY];
    int rc = read_key(word, 0);
    if (rc != OK)
        return rc;

    if (choice == '4')
    {
        rc = tree_insert(root, word);
        if (rc == OK)
            printf("Добавлено слово «%s».\n", word);
    }
    else if (choice == '5')
    {
        rc = tree_remove(root, word);
        if (rc == OK)
            printf("Удалено одно вхождение слова «%s».\n", word);
    }
    else
    {
        size_t comparisons = 0;
        const node_t *found = tree_find(*root, word, &comparisons);
        printf("Слово «%s» %s. Сравнений: %zu.\n",
               word, found ? "найдено" : "не найдено", comparisons);
    }
    return rc;
}

static void print_word(const char *word, void *context)
{
    int *first = context;
    if (!*first)
        putchar(' ');
    fputs(word, stdout);
    *first = 0;
}

static int print_traversals(const node_t *root)
{
    if (!root)
        return EMPTY_TREE;

    const traversal_t orders[] = {PREORDER, INORDER, POSTORDER};
    const char *labels[] = {"Префиксный", "Инфиксный", "Постфиксный"};
    for (size_t i = 0; i < sizeof(orders) / sizeof(orders[0]); ++i)
    {
        int first = 1;
        printf("%s обход: ", labels[i]);
        tree_traverse(root, orders[i], print_word, &first);
        putchar('\n');
    }
    return OK;
}

int main(void)
{
    node_t *root = NULL;
    for (;;)
    {
        print_menu();
        int choice = read_choice();
        if (choice == '0')
            break;

        int rc = OK;
        switch (choice)
        {
            case '1':
                rc = load_words(&root);
                break;
            case '2':
                if (root)
                    print_tree(root, NULL);
                else
                    rc = EMPTY_TREE;
                break;
            case '3':
                rc = find_by_letter(root);
                break;
            case '4':
            case '5':
            case '6':
                rc = change_word(&root, choice);
                break;
            case '7':
                rc = print_traversals(root);
                break;
            case '8':
                tree_clear(&root);
                puts("Дерево очищено.");
                break;
            default:
                rc = INPUT_ERROR;
        }
        if (rc != OK)
            print_error(rc);
    }

    tree_clear(&root);
    return OK;
}
