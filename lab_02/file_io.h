#ifndef FILE_IO_H
#define FILE_IO_H

#include "struct_func.h"
#include "errors.h"

/* OK при успешном разборе строки, ERR_PARSE если строка не соответствует формату */
int parse_line(char *line, Flat *f);

/* OK при успешном открытии файла (число загруженных записей — t->count),
   ERR_FILE_OPEN если файл отсутствует/не открылся, ERR_EMPTY_FILE если файл
   пуст (оба случая не считаются фатальными — вызывающий код должен просто
   начать с пустой таблицы), ERR_MEMORY при нехватке памяти */
int load_table(char *filename, Table *t);

/* OK при успешном сохранении, ERR_FILE_OPEN при ошибке открытия файла */
int save_table(char *filename, Table *t);

#endif
