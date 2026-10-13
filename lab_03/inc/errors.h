#ifndef ERRORS_H
#define ERRORS_H

#define OK 0
#define ERR_INPUT 1
#define ERR_EMPTY 2
#define ERR_RANGE 3
#define ERR_ZERO 4
#define ERR_DUPLICATE 5
#define ERR_FILE_OPEN 6
#define ERR_FILE_FORMAT 7
#define ERR_MEMORY 8
#define ERR_SIZE 9
#define ERR_NO_DATA 10
#define ERR_FILE_WRITE 12

void print_error(int rc);

#endif
