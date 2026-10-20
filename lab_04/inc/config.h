#ifndef CONFIG_H
#define CONFIG_H

// максимальная длина буфера ввода
#define LINE_SIZE 128

// размер массива под два стека (bench собирается с большим значением через -DSTACK_SIZE)
#ifndef STACK_SIZE
#define STACK_SIZE 10
#endif


#define LIST_MAX_COUNT 10

#define MAX_FREED 20


#define MIN_SAMPLES 10
#define MAX_SAMPLES 100
#define MAX_ERROR 0.03

#define BATCH_SIZE 100

#define BENCH_SIZES {100, 1000, 5000, 10000}

#endif
