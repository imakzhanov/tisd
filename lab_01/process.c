#include <stdio.h>

#include "process.h"

/**
 * @brief 
 * 
 * @param a 
 * @param a_len 
 * @param b 
 * @param b_len 
 * @return int
 * 1 if a > b
 * -1 if a < b
 * 0 if a == b 
 */
int comparator(int a[], int a_len, int b[], int b_len)
{
    if (a_len > b_len)
        return 1;
    if (a_len < b_len)
        return -1;

    for (int i = 0; i < a_len; i++)
    {
        if (a[i] > b[i])
            return 1;
        if  (a[i] < b[i])
            return -1;
    }
    return 0;
}

void subtract(int a[], int *a_len, int b[], int b_len)
{
    int i = *a_len - 1;
    int j = b_len - 1;

    int carry = 0;

    while (i >= 0)
    {
        int b_current;
        if (j >= 0)
            b_current = b[j];
        else
            b_current = 0;

        int diff = a[i] - b_current - carry;
        if (diff < 0)
        {
            diff += 10;
            carry = 1;
        }
        else
            carry = 0;

        a[i] = diff;
        i--;
        j--;

    }
    int first_not_zero = 0;
    while (first_not_zero < *a_len && a[first_not_zero] == 0)
        first_not_zero++;
    if (first_not_zero == *a_len)
    {
        a[0] = 0;
        *a_len = 1;
    }
    else 
    {
        for (int i = first_not_zero; i < *a_len; i++)
            a[i - first_not_zero] = a[i];
        *a_len -= first_not_zero;
    }
}


int division(real_num a, integer_num b, real_num *result)
{
    if (b.mantissa[0] == 0 && b.mantissa_len == 1)
        return DIVISION_BY_ZERO;

    if (a.mantissa_len == 1 && a.mantissa[0] == 0)
    {
        result->mantissa_sign = 1;
        result->mantissa_len = 1;
        result->mantissa[0] = 0;
        result->order = 0;
        return OK;
    }

    // вычисление знака
    result->mantissa_sign = a.mantissa_sign * b.sign;

    int result_order = a.order - b.mantissa_len + 1;

    int current_dividend[MAX_BUFFER_LEN];
    int current_dividend_len = 0;
    int a_pos = 0;

    for (int i = 0; i < b.mantissa_len; i++) 
    {
        if (a_pos < a.mantissa_len)
            current_dividend[current_dividend_len++] = a.mantissa[a_pos++];
        else
            current_dividend[current_dividend_len++] = 0;
    }

    if (comparator(current_dividend, current_dividend_len, b.mantissa, b.mantissa_len) < 0)
    {
        if (a_pos < a.mantissa_len)
            current_dividend[current_dividend_len++] = a.mantissa[a_pos++];
        else
            current_dividend[current_dividend_len++] = 0;
            
        result_order--;
    }

    int result_buffer[MAX_MANTISSA_LEN + 1];
    int result_len = 0;

    while (result_len < MAX_MANTISSA_LEN + 1)
    {
        int q = 0;

        while (comparator(current_dividend, current_dividend_len, b.mantissa, b.mantissa_len) >= 0)
        {
            subtract(current_dividend, &current_dividend_len, b.mantissa, b.mantissa_len);
            q++;
        }

        result_buffer[result_len++] = q;

        // проверка конца деления
        if (current_dividend_len == 1 && current_dividend[0] == 0 && a_pos >= a.mantissa_len)
            break;

        int new_digit;
        if (a_pos >= a.mantissa_len)
            new_digit = 0;
        else 
            new_digit = a.mantissa[a_pos++];

        if (current_dividend_len == 1 && current_dividend[0] == 0)
            current_dividend[0] = new_digit;
        else
            current_dividend[current_dividend_len++] = new_digit;
    }

    // округление
    if (result_len == MAX_MANTISSA_LEN + 1)
    {
        if (result_buffer[MAX_MANTISSA_LEN] >= 5)
        {
            int carry = 1;
            for (int i = MAX_MANTISSA_LEN - 1; i >= 0; i--)
            {
                if (result_buffer[i] == 9)
                    result_buffer[i] = 0;
                else
                {
                    result_buffer[i]++;
                    carry = 0;
                    break;
                }
            }
            if (carry > 0)
            {
                result_buffer[0] = carry;
                result_order++;
            }
        }
        result_len = MAX_MANTISSA_LEN;
    }

    if (result_order > MAX_ORDER)
        return MACHINE_INF;
    else if (result_order < -MAX_ORDER)
        return MACHINE_ZERO;

    result->mantissa_len = result_len;
    result->order = result_order;
    for (int i = 0; i < result_len; i++)
        result->mantissa[i] = result_buffer[i];

    return OK;
}

