#ifndef ERRORS_H
#define ERRORS_H

#define OK 0
#define ERR_INPUT 1       // не число или лишние символы в строке
#define ERR_EMPTY 2       // пустой ввод или конец ввода (EOF)
#define ERR_RANGE 3       // значение вне допустимого диапазона
#define ERR_ZERO 4        // ноль там, где нужен ненулевой элемент
#define ERR_DUPLICATE 5   // элемент с такими координатами уже введён
#define ERR_FILE_OPEN 6   // файл не открылся
#define ERR_FILE_FORMAT 7 // содержимое файла не соответствует формату
#define ERR_MEMORY 8
#define ERR_SIZE 9        // размеры матриц не совпадают
#define ERR_NO_DATA 10    // матрица ещё не введена
#define ERR_NOT_IMPL 11   // функция ещё не написана
#define ERR_FILE_WRITE 12 // не удалось записать в файл
#define ERR_NO_RESULT 13  // результат сложения ещё не получен

void print_error(int rc);

#endif
