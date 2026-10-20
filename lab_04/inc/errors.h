#ifndef ERRORS_H
#define ERRORS_H

#define OK 0
#define ERR_OVERFLOW 1
#define ERR_EMPTY 2
#define ERR_STACK_NUM 3
#define ERR_MEMORY 4
#define ERR_INPUT 5
#define ERR_RANGE 6
#define ERR_EOF 7

void print_error(int rc);

#endif
