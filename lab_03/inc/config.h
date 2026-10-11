#ifndef CONFIG_H
#define CONFIG_H

// максимальный размер матрицы
#define MAX_SIZE 5000
// значения при случайном заполнении от -RANDOM_VALUE_MAX до RANDOM_VALUE_MAX, кроме нуля
#define RANDOM_VALUE_MAX 100

// максимальная длина вводимой строки
#define LINE_SIZE 128
// сколько элементов CSC выводить на экран
#define PRINT_LIMIT 100
// обычная матрица выводится таблицей только до такого размера
#define PRINT_STANDARD_MAX 30

#define RESULT_DIR "result"
#define FILE_A "a.txt"
#define FILE_B "b.txt"
#define FILE_RES_CSC "res_csc.txt"
#define FILE_RES_STANDARD "res_standard.txt"

// замеры
#define REPEATS 10
// размеры матриц и проценты заполнения, для которых строится таблица
#define BENCH_SIZES {10, 50, 100, 200, 500, 1000, 5000}
#define BENCH_PERCENTS {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 15, 20, 30, 40, 50, 70, 100}

#endif
