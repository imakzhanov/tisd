#ifndef CONFIG_H
#define CONFIG_H

// максимальный размер матрицы
#define MAX_SIZE 5000
// значения при случайном заполнении от -RANDOM_VALUE_MAX до RANDOM_VALUE_MAX, кроме нуля
#define RANDOM_VALUE_MAX 100

// максимальная длина буфера
#define LINE_SIZE 128

#define RESULT_DIR "result"

// замеры: повторяются, пока погрешность среднего не станет меньше MAX_ERROR от самого среднего
// минимальнное количество повторений при измерении
#define MIN_SAMPLES 10
// максимальное количество повторений при измерении
#define MAX_SAMPLES 100
#define MAX_ERROR 0.05
// сколько сложений выполняется подряд в одном замере
#define BATCH_SIZE 10

// размеры матриц и проценты заполнения, для которых строится таблица
#define BENCH_SIZES {50, 100, 200, 500, 1000, 5000}
#define BENCH_PERCENTS {1, 2, 3, 4, 5, 10, 15, 20, 30, 40, 50, 70, 100}

#endif
