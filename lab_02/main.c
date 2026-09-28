#include <stdio.h>
#include "struct_func.h"
#include "file_io.h"
#include "menu.h"
#include "errors.h"

int main(void)
{
    Table t;
    table_init(&t);

    if (load_table("data.txt", &t) == OK)
        printf("Загружено записей из data.txt: %d\n", t.count);
    else
        printf("Файл data.txt не найден или пуст. Начинаем с пустой таблицы.\n");

    run_menu_loop(&t);

    table_free(&t);
    return 0;
}
