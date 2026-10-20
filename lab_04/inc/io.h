#ifndef IO_H
#define IO_H

#include <stddef.h>

int read_line(char *buf, size_t size);
int read_int(const char *prompt, int min, int max, int *value);
int read_double(const char *prompt, double *value);

#endif
