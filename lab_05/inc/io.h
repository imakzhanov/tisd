#ifndef IO_H
#define IO_H

#include "model.h"

int read_choice(void);
int read_value(double *value);
int read_params(params_t *params);
void print_progress(const result_t *result);
void print_result(const result_t *result, const params_t *params);
void print_error(int rc);

#endif
