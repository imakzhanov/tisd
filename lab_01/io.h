#ifndef IO_H
#define IO_H

#include "process.h"
#include "consts.h"
#include "return_codes.h"

int parse_real(char buf[], real_num *num);
int parse_integer(char buf[], integer_num *num);


void print_integer(integer_num number);
void print_real(real_num number);

void print_error(int rc);

#endif