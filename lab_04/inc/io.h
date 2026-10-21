#ifndef IO_H
#define IO_H

#include <stdint.h>

int read_int(char *text, int min, int max, int *value);
int read_double(char *text, double *value);

int read_addr(char *text, uintptr_t *value);

#endif
