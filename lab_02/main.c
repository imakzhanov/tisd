#include <stdio.h>
#include "struct_func.h"
#include "file_io.h"
#include "menu.h"
#include "errors.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Запуск: ./app.exe <входной файл> <выходной файл>\n");
        return INCORRECT_ARGS;
    }

    char *in_filename = argv[1];
    char *out_filename = argv[2];

    Table table;
    table_init(&table);

    if (load_table(in_filename, &table) == OK)
        printf("Загружено записей из %s: %d\n", in_filename, table.count);
    else
        printf("Файл %s не найден или пуст. Сейчас таблица пуста.\n", in_filename);

    KeyTable key_table;
    int is_key_actual = 0;
    if (build_key_table(&table, &key_table) != OK)
    {
        printf("Не удалось выделить память\n");
        table_free(&table);
        return ERR_MEMORY;
    }
    is_key_actual = 1;

    run_menu(&table, &key_table, &is_key_actual, out_filename);

    key_table_free(&key_table);
    table_free(&table);
    return 0;
}
