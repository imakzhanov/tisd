#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "menu.h"
#include "errors.h"

int main(int argc, char *argv[])
{
    if (argc != 1 && argc != 3)
    {
        printf("Использование: %s [<файл матрицы A> <файл матрицы B>]\n", argv[0]);
        return ERR_INPUT;   
    }

    srand((unsigned)time(NULL));
    if (argc == 3)
        return run_menu(argv[1], argv[2]);
    return run_menu(NULL, NULL);
}
