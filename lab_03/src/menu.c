#include <stdio.h>

#include "menu.h"
#include "struct_func.h"
#include "io.h"
#include "errors.h"

standard_t a, b, res;
csc_t csc_a, csc_b, csc_res;
static int has_res_csc, has_res_std;

#define RESULT_DIR "result"


void print_menu(int has_a, int has_b, int has_res_csc, int has_res_std)
{   
    

    printf("\n----- Сложение разреженных матриц -----\n");
    printf("Матрица A: %s | Матрица B: %s\n", has_a ? "введена" : "не введена", has_b ? "введена" : "не введена");
    printf("Результат: CSC - %s | обычный - %s\n", has_res_csc ? "получен" : "не получен",
           has_res_std ? "получен" : "не получен");
    printf("-----------------------------------------\n");
    printf("1 - Ввести матрицу A\n");
    printf("2 - Ввести матрицу B\n");
    printf("3 - Показать матрицы A и B в формате CSC\n");
    printf("4 - Показать матрицы A и B в обычном виде\n");
    printf("5 - Сложить A + B в формате CSC\n");
    printf("6 - Сложить A + B обычным способом\n");
    printf("7 - Сохранить A, B и результат в папку %s\n", RESULT_DIR);
    printf("0 - Выход\n");
    printf("-----------------------------------------\n");
}

// вводит матрицу из файла path (если path != NULL) или способом, выбранным в меню;
// при ошибке ввода матрица считается не введённой
int input_matrix(const char *name, const char *path, standard_t *dst, csc_t *cdst, int *has)
{
    int choice = 0;
    int rc;

    if (path == NULL)
    {
        printf("Способ ввода матрицы %s:\n", name);
        printf("1 - вручную (координатами)\n2 - случайно (с вводом процента заполнения)\n0 - отмена\n");
        rc = read_int("Выберите способ", 0, 2, &choice);
        if (rc != OK || choice == 0)
            return rc;
    }

    *has = 0;
    has_res_csc = has_res_std = 0; // старый результат относится к прежним матрицам
    csc_free(cdst);

    if (path != NULL)
        rc = input_file(path, dst);
    else if (choice == 1)
        rc = input_manual(dst);
    else
        rc = input_random(dst);
    if (rc == OK)
        rc = standard_to_csc(dst, cdst);
    if (rc != OK)
        return rc;

    *has = 1;
    printf("Матрица %s введена: %d x %d, ненулевых элементов: %d\n", name, cdst->rows, cdst->cols, cdst->nonzero_count);
    return OK;
}

static int check_operands(int has_a, int has_b)
{
    if (!has_a || !has_b)
        return ERR_NO_DATA;
    if (a.rows != b.rows || a.cols != b.cols)
        return ERR_SIZE;
    return OK;
}

// as_csc: 1 - показать в формате CSC, 0 - в обычном виде
static int show_matrices(int has_a, int has_b, int as_csc)
{
    if (!has_a && !has_b)
        return ERR_NO_DATA;
    if (has_a)
    {
        if (as_csc)
            print_csc(stdout, "A", &csc_a, PRINT_LIMIT);
        else
            print_standard("A", &a);
    }
    else
        printf("Матрица A не введена\n");
    printf("\n");
    if (has_b)
    {
        if (as_csc)
            print_csc(stdout, "B", &csc_b, PRINT_LIMIT);
        else
            print_standard("B", &b);
    }
    else
        printf("Матрица B не введена\n");
    return OK;
}

static int menu_csc_add(int has_a, int has_b)
{
    int rc = check_operands(has_a, has_b);
    if (rc != OK)
        return rc;

    has_res_csc = 0;
    csc_free(&csc_res);
    rc = csc_add(&csc_a, &csc_b, &csc_res);
    if (rc != OK)
        return rc;
    has_res_csc = 1;

    printf("Результат сложения в формате CSC:\n");
    print_csc(stdout, "A + B", &csc_res, PRINT_LIMIT);
    if (csc_res.rows <= PRINT_STANDARD_MAX && csc_res.cols <= PRINT_STANDARD_MAX)
    {
        // res занят результатом обычного сложения, если он есть; иначе используем его для таблицы
        rc = has_res_std ? OK : csc_to_standard(&csc_res, &res);
        if (rc == OK)
            print_standard("A + B", &res);
    }
    return rc;
}

static int menu_standard_add(int has_a, int has_b)
{
    int rc = check_operands(has_a, has_b);
    if (rc != OK)
        return rc;

    has_res_std = 0;
    rc = standard_add(&a, &b, &res);
    if (rc != OK)
        return rc;
    has_res_std = 1;

    int nonzero_count = standard_count_nonzero(&res);
    printf("Результат обычного сложения: %d x %d, ненулевых элементов: %d\n", res.rows, res.cols, nonzero_count);
    if (nonzero_count == 0)
        printf("Матрица нулевая\n");
    else
        print_standard("A + B", &res);
    return OK;
}

// сохраняет в папку RESULT_DIR файлы в формате CSC: a.txt, b.txt и полученные результаты
// (res_csc.txt - сложение в CSC, res_standard.txt - обычное сложение, переведённое в CSC)
static int menu_save(int has_a, int has_b)
{
    csc_t res_csc;
    int rc;

    if (!has_a || !has_b)
        return ERR_NO_DATA;
    if (!has_res_csc && !has_res_std)
        return ERR_NO_RESULT;

    rc = create_dir(RESULT_DIR);
    if (rc == OK)
        rc = save_csc(RESULT_DIR "/a.txt", &csc_a);
    if (rc == OK)
        rc = save_csc(RESULT_DIR "/b.txt", &csc_b);
    if (rc == OK && has_res_csc)
        rc = save_csc(RESULT_DIR "/res_csc.txt", &csc_res);
    if (rc == OK && has_res_std)
    {
        rc = standard_to_csc(&res, &res_csc);
        if (rc == OK)
        {
            rc = save_csc(RESULT_DIR "/res_standard.txt", &res_csc);
            csc_free(&res_csc);
        }
    }

    if (rc == OK)
    {
        printf("Сохранено в папку %s: a.txt, b.txt", RESULT_DIR);
        if (has_res_csc)
            printf(", res_csc.txt");
        if (has_res_std)
            printf(", res_standard.txt");
        printf("\n");
    }
    return rc;
}

int run_menu(const char *path_a, const char *path_b)
{
    int run = 1;
    int choice;
    int rc;

    csc_init(&csc_a);
    csc_init(&csc_b);
    csc_init(&csc_res);

    int has_a = 0, has_b = 0;

    if (path_a != NULL)
    {
        rc = input_matrix("A", path_a, &a, &csc_a, &has_a);
        if (rc == OK)
            rc = input_matrix("B", path_b, &b, &csc_b, &has_b);
        if (rc != OK)
        {
            print_error(rc);
            csc_free(&csc_a);
            csc_free(&csc_b);
            return rc;
        }
    }

    while (run)
    {
        print_menu(has_a, has_b, has_res_csc, has_res_std);
        rc = read_int("Выберите пункт", 0, 7, &choice);
        if (rc != OK)
        {
            print_error(rc);
            continue;
        }

        switch (choice)
        {
            case 1:
                rc = input_matrix("A", NULL, &a, &csc_a, &has_a);
                break;
            case 2:
                rc = input_matrix("B", NULL, &b, &csc_b, &has_b);
                break;
            case 3:
                rc = show_matrices(has_a, has_b, 1);
                break;
            case 4:
                rc = show_matrices(has_a, has_b, 0);
                break;
            case 5:
                rc = menu_csc_add(has_a, has_b);
                break;
            case 6:
                rc = menu_standard_add(has_a, has_b);
                break;
            case 7:
                rc = menu_save(has_a, has_b);
                break;
            case 0:
                run = 0;
                break;
        }
        print_error(rc);
    }

    csc_free(&csc_a);
    csc_free(&csc_b);
    csc_free(&csc_res);
    return OK;
}
