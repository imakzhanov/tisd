#include "io.h"

#include <string.h>

// Возвращает длину русской или латинской буквы в байтах UTF-8 либо ноль.
static size_t letter_size(const unsigned char *text)
{
    unsigned char first = text[0];
    if ((first >= 'A' && first <= 'Z') || (first >= 'a' && first <= 'z'))
        return 1;
    if (first == 0xD0 && (text[1] == 0x81 || (text[1] >= 0x90 && text[1] <= 0xBF)))
        return 2;
    if (first == 0xD1 && ((text[1] >= 0x80 && text[1] <= 0x8F) || text[1] == 0x91))
        return 2;
    return 0;
}

// Читает строку и дочитывает её остаток при превышении длины.
static int read_line(char *line, int capacity)
{
    if (!fgets(line, capacity, stdin))
        return ferror(stdin) ? -1 : 0;

    size_t length = strlen(line);
    if (length && line[length - 1] == '\n')
        line[--length] = '\0';
    else if (!feof(stdin))
    {
        int ch = getchar();
        if (ch != '\n' && ch != EOF)
        {
            while ((ch = getchar()) != '\n' && ch != EOF)
                ;
            return -1;
        }
        if (ferror(stdin))
            return -1;
    }

    if (length && line[length - 1] == '\r')
        line[length - 1] = '\0';
    return 1;
}

// Читает один символ команды меню и проверяет формат ввода.
int read_choice(void)
{
    char line[32], choice, extra;
    int rc = read_line(line, sizeof(line));
    if (rc == 0 || ferror(stdin))
        return '0';
    if (rc < 0 || sscanf(line, " %c %c", &choice, &extra) != 1)
        return -1;
    return choice;
}

// Запрашивает непустой путь к входному файлу.
int read_file_path(char path[PATH_CAPACITY])
{
    printf("Путь к текстовому файлу UTF-8: ");
    if (read_line(path, PATH_CAPACITY) != 1 || !path[0])
        return INPUT_ERROR;
    return OK;
}

// Читает и проверяет слово или одну букву.
int read_key(char word[WORD_CAPACITY], int single_letter)
{
    if (single_letter)
        printf("Буква: ");
    else
        printf("Слово (русские и латинские буквы, до %d байт): ", WORD_CAPACITY - 1);

    if (read_line(word, WORD_CAPACITY) != 1 || !word[0])
        return INPUT_ERROR;

    const unsigned char *text = (const unsigned char *)word;
    while (*text)
    {
        size_t length = letter_size(text);
        if (!length || (single_letter && text[length]))
            return INPUT_ERROR;
        text += length;
    }
    return OK;
}

// Читает следующее слово из текстового файла UTF-8.
int file_read_word(FILE *file, char word[WORD_CAPACITY], int *has_word)
{
    size_t used = 0;
    int ch;
    *has_word = 0;

    while ((ch = fgetc(file)) != EOF)
    {
        unsigned char text[2] = {(unsigned char)ch, 0};
        if (ch == 0xD0 || ch == 0xD1)
        {
            int next = fgetc(file);
            if (next >= 0x80 && next <= 0xBF)
                text[1] = (unsigned char)next;
            else if (next != EOF && ungetc(next, file) == EOF)
                return FILE_ERROR;
        }

        size_t length = letter_size(text);
        if (!length)
        {
            if (used)
                break;
            continue;
        }
        if (used + length >= WORD_CAPACITY)
            return WORD_TOO_LONG;

        memcpy(word + used, text, length);
        used += length;
    }

    if (ferror(file))
        return FILE_ERROR;
    word[used] = '\0';
    *has_word = used != 0;
    return OK;
}

// Строит дерево из файла и заменяет текущее только при успешной загрузке.
int load_tree(const char *path, node_t **root)
{
    FILE *file = fopen(path, "rb");
    if (!file)
        return FILE_ERROR;

    node_t *loaded = NULL;
    char word[WORD_CAPACITY];
    int has_word;
    int rc;
    while ((rc = file_read_word(file, word, &has_word)) == OK && has_word)
    {
        rc = tree_insert(&loaded, word);
        if (rc != OK)
            break;
    }

    if (fclose(file) == EOF && rc == OK)
        rc = FILE_ERROR;
    if (rc == OK && !loaded)
        rc = EMPTY_FILE;
    if (rc != OK)
    {
        tree_clear(&loaded);
        return rc;
    }

    tree_clear(root);
    *root = loaded;
    return OK;
}

// Последовательно читает файл и считает слова на заданную букву.
int file_count_prefix(FILE *file, const char *letter, size_t *count, size_t *comparisons)
{
    char word[WORD_CAPACITY];
    size_t length = strlen(letter);
    int has_word;
    int rc;
    *count = 0;
    while ((rc = file_read_word(file, word, &has_word)) == OK && has_word)
    {
        if (comparisons)
            ++*comparisons;
        if (strncmp(word, letter, length) == 0)
            ++*count;
    }
    return rc;
}

// выводит ветку рекурсивно
static void print_branch(const node_t *root, const char *letter, size_t length,
                         size_t depth, const char *side)
{
    if (!root)
        return;

    for (size_t i = 0; i < depth; ++i)
        fputs("  ", stdout);
    printf("%s: ", side);
    if (letter && strncmp(root->word, letter, length) == 0)
        printf("\033[1;32m%s\033[0m\n", root->word);
    else
        puts(root->word);

    print_branch(root->left, letter, length, depth + 1, "L");
    print_branch(root->right, letter, length, depth + 1, "R");
}

// Печатает дерево в консоли и подсвечивает слова на заданную букву. рекурсивно вызывает print_branch
void print_tree(const node_t *root, const char *letter)
{
    print_branch(root, letter, letter ? strlen(letter) : 0, 0, "root");
}

// Рекурсивно записывает узел, его потомков и подсветку в DOT.
static int write_dot_branch(FILE *file, const node_t *node, size_t id, size_t *next_id,
                            const char *letter, size_t length)
{
    const char *highlight = letter && strncmp(node->word, letter, length) == 0
                            ? ", style=filled, fillcolor=palegreen" : "";
    if (fprintf(file, "    n%zu [label=\"%s\"%s];\n", id, node->word, highlight) < 0)
        return FILE_ERROR;

    const node_t *children[] = {node->left, node->right};
    const char *labels[] = {"L", "R"};
    for (size_t i = 0; i < sizeof(children) / sizeof(children[0]); ++i)
    {
        if (!children[i])
            continue;

        size_t child_id = (*next_id)++;
        if (fprintf(file, "    n%zu -> n%zu [label=\"%s\"];\n",
                    id, child_id, labels[i]) < 0)
            return FILE_ERROR;

        int rc = write_dot_branch(file, children[i], child_id, next_id, letter, length);
        if (rc != OK)
            return rc;
    }
    return OK;
}

// Записывает узлы и связи дерева в файл формата DOT.
int export_tree_dot(const node_t *root, const char *path, const char *letter)
{
    if (!root)
        return EMPTY_TREE;

    FILE *file = fopen(path, "w");
    if (!file)
        return FILE_ERROR;

    size_t next_id = 1;
    int rc = FILE_ERROR;
    if (fputs("digraph Tree {\n    graph [charset=\"UTF-8\", ordering=out];\n    node [shape=box, fontname=\"sans-serif\"];\n", file) != EOF)
        rc = write_dot_branch(file, root, 0, &next_id, letter, letter ? strlen(letter) : 0);
    if (rc == OK && fputs("}\n", file) == EOF)
        rc = FILE_ERROR;
    if (fclose(file) == EOF)
        rc = FILE_ERROR;

    return rc;
}

// Выводит сообщение по коду ошибки в стандартный поток ошибок.
void print_error(int rc)
{
    const char *messages[] = {
        "",
        "Не удалось выделить память.",
        "Дерево пусто.",
        "Некорректный ввод.",
        "Ошибка открытия, чтения или записи файла.",
        "В файле нет слов.",
        "Слишком длинное слово.",
        "Слово не найдено.",
        "Не удалось измерить время.",
        "Результаты сравниваемых алгоритмов не совпали.",
        "Команда dot не найдена. Для создания PNG нужен Graphviz.",
        "Graphviz не смог создать изображение PNG."
    };
    fprintf(stderr, "%s\n", messages[rc]);
}
