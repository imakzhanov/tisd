#ifndef PROCESS_H
#define PROCESS_H


#include "consts.h"
#include "return_codes.h"

typedef struct
{
    int mantissa_sign;
    int mantissa[MAX_MANTISSA_LEN];
    int mantissa_len;
    int order;
} real_num;

typedef struct
{
    int sign;
    int mantissa[MAX_MANTISSA_LEN];
    int mantissa_len;
} integer_num;


int division(real_num a, integer_num b, real_num *result);

#endif