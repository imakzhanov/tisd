#include "io.h"

#include <string.h>

// Слова состоят из латинских или русских букв в UTF-8.
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

static void lower_letter(unsigned char *text)
{
    if (text[0] >= 'A' && text[0] <= 'Z')
        text[0] += 'a' - 'A';
    else if (text[0] == 0xD0 && text[1] == 0x81)
    {
        text[0] = 0xD1;
        text[1] = 0x91;
    }
    else if (text[0] == 0xD0 && text[1] >= 0x90 && text[1] <= 0x9F)
        text[1] += 0x20;
    else if (text[0] == 0xD0 && text[1] >= 0xA0 && text[1] <= 0xAF)
    {
        text[0] = 0xD1;
        text[1] -= 0x20;
    }
}

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

int read_file_path(char path[PATH_CAPACITY])
{
    printf("Путь к текстовому файлу UTF-8: ");
    if (read_line(path, PATH_CAPACITY) != 1 || !path[0])
        return INPUT_ERROR;
    return OK;
}

int read_key(char word[WORD_CAPACITY], int single_letter)
{
    if (single_letter)
        printf("Буква (русская или латинская): ");
    else
        printf("Слово (русские и латинские буквы, до %d байт): ", WORD_CAPACITY - 1);

    if (read_line(word, WORD_CAPACITY) != 1 || !word[0])
        return INPUT_ERROR;

    unsigned char *text = (unsigned char *)word;
    while (*text)
    {
        size_t length = letter_size(text);
        if (!length || (single_letter && text[length]))
            return INPUT_ERROR;
        lower_letter(text);
        text += length;
    }
    return OK;
}

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

        lower_letter(text);
        memcpy(word + used, text, length);
        used += length;
    }

    if (ferror(file))
        return FILE_ERROR;
    word[used] = '\0';
    *has_word = used != 0;
    return OK;
}

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

static void print_branch(const node_t *root, const char *letter, size_t length,
                         size_t depth, const char *side)
{
    if (!root)
        return;

    for (size_t i = 0; i < depth; ++i)
        printf("    ");
    printf("%s", side);
    if (letter && strncmp(root->word, letter, length) == 0)
        printf("\033[1;32m%s\033[0m\n", root->word);
    else
        puts(root->word);

    print_branch(root->left, letter, length, depth + 1, "|-- Л: ");
    print_branch(root->right, letter, length, depth + 1, "|-- П: ");
}

void print_tree(const node_t *root, const char *letter)
{
    print_branch(root, letter, letter ? strlen(letter) : 0, 0, "Корень: ");
}

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
        "Результаты сравниваемых алгоритмов не совпали."
    };
    fprintf(stderr, "%s\n", messages[rc]);
}
