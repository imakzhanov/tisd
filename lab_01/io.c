#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "io.h"

int my_isdigit(char c)
{
    return c >= '0' && c <= '9';
}

void skip_spaces(char buf[], int *pos)
{
    while (buf[*pos] == ' ' || buf[*pos] == '\n')
        (*pos)++;
}

int parse_real(char buf[], real_num *num) 
{
    // инициализация 
    num->mantissa_sign = 1;
    num->order = 0;
    num->mantissa_len = 0;
    for (int i = 0; i < MAX_MANTISSA_LEN; i++) {
        num->mantissa[i] = 0;
    }

    int pos = 0;

    skip_spaces(buf, &pos);

    // определение знака
    if (buf[pos] == '+')
        pos++;
    else if (buf[pos] == '-') 
    {
        num->mantissa_sign = -1;
        pos++;
    }

    int digits[MAX_BUFFER_LEN]; 
    int total_digits = 0;

    int dot_position = -1; // Позиция точки
    int has_digits = 0;
    int has_dot = 0;

    
    while (buf[pos] != '\0' && buf[pos] != '\n' && buf[pos] != 'E' && buf[pos] != 'e') 
    {
        if (buf[pos] == '.') 
        {
            if (has_dot) 
                return -1; 
            has_dot = 1;
            dot_position = total_digits;
        } 
        else if (my_isdigit(buf[pos])) 
        {
            has_digits = 1;
            if (total_digits < MAX_BUFFER_LEN) 
            {
                digits[total_digits++] = buf[pos] - '0';
            }
        } 
        else if (buf[pos] == ' ') 
        {
            pos++;
            continue;
        } 
        else 
            return INCORRECT_SYMBOL;
        pos++;
    }

    if (!has_digits) 
        return INCORRECT_INPUT;

    if (!has_dot)
        dot_position = total_digits;

    // Удаление ведущих нулей
    int first_not_zero = 0;
    while (first_not_zero < total_digits && digits[first_not_zero] == 0)
        first_not_zero++;

    // обработка числа 0
    if (first_not_zero == total_digits) 
    {
        num->mantissa_len = 1;
        num->mantissa[0] = 0;
        num->order = 0;

        return OK;
    }

    int mantissa_order = dot_position - first_not_zero;

    int mantissa_pos = 0;
    for (int i = first_not_zero; i < total_digits; i++) 
    {
        if (mantissa_pos < MAX_MANTISSA_LEN) 
            num->mantissa[mantissa_pos++] = digits[i];
        else 
            return TOO_LONG_MANTISSA;
    }
    mantissa_pos--;
    while (num->mantissa[mantissa_pos] == 0)
        mantissa_pos--;
    num->mantissa_len = mantissa_pos + 1;

    // Разбор экспоненциальной части
    int exp_val = 0;
    if (buf[pos] == 'E' || buf[pos] == 'e')
    {
        pos++;
        int exp_sign = 1;
        skip_spaces(buf, &pos);

        // Разбор знака
        if (buf[pos] == '+')
            pos++;
        else if (buf[pos] == '-') 
        {
            exp_sign = -1;
            pos++;
        }

        int exp_digits = 0;
        int has_exp_digits = 0;

        // Чтение цифр порядка
        while (my_isdigit(buf[pos])) 
        {
            has_exp_digits = 1;
            exp_val = exp_val * 10 + (buf[pos] - '0');
            exp_digits++;
            pos++;
        }

        if (!has_exp_digits) 
            return INCORRECT_INPUT;

        if (exp_digits > 5)
        {
            if (exp_sign == 1)
                return MACHINE_INF;
            else
                return MACHINE_ZERO;
        }
            
        
        exp_val *= exp_sign;
    }

    skip_spaces(buf, &pos);

    if (buf[pos] != '\0' && buf[pos] != '\n')
        return INCORRECT_INPUT;

    int final_order = mantissa_order + exp_val;

    num->order = final_order;

    return OK;
}

int parse_integer(char buf[], integer_num *num)
{
    // инициализация 
    num->sign = 1;
    num->mantissa_len = 0;
    for (int i = 0; i < MAX_MANTISSA_LEN; i++) {
        num->mantissa[i] = 0;
    }

    int pos = 0;

    skip_spaces(buf, &pos);

    // определение знака
    if (buf[pos] == '+')
        pos++;
    else if (buf[pos] == '-') 
    {
        num->sign = -1;
        pos++;
    }

    int digits[MAX_BUFFER_LEN]; 
    int total_digits = 0;
    int has_digits = 0;

    
    while (buf[pos] != '\0' && buf[pos] != '\n' && buf[pos] != ' ') 
    {
        if (my_isdigit(buf[pos])) 
        {
            has_digits = 1;
            if (total_digits < MAX_BUFFER_LEN) 
            {
                digits[total_digits++] = buf[pos] - '0';
            }
        }
        else 
            return INCORRECT_SYMBOL;
        pos++;
    }

    if (!has_digits) 
        return INCORRECT_INPUT;

    // Удаление ведущих нулей
    int first_not_zero = 0;
    while (first_not_zero < total_digits && digits[first_not_zero] == 0)
        first_not_zero++;

    // обработка числа 0
    if (first_not_zero == total_digits) 
    {
        num->mantissa_len = 1;
        num->mantissa[0] = 0;
        
        return OK;
    }

    int mantissa_pos = 0;
    for (int i = first_not_zero; i < total_digits; i++) 
    {
        if (mantissa_pos < MAX_MANTISSA_LEN) 
            num->mantissa[mantissa_pos++] = digits[i];
        else 
            return TOO_LONG_MANTISSA;
    }
    num->mantissa_len = mantissa_pos;

    skip_spaces(buf, &pos);

    if (buf[pos] != '\0' && buf[pos] != '\n')
        return INCORRECT_INPUT;

    return OK;
}

void print_integer(integer_num number)
{   
    if (number.sign == -1)
        printf("-");
    else
        printf("+");
    for (int i = 0; i < number.mantissa_len; i++)
    {
        printf("%d", number.mantissa[i]);
    }
    printf("\n");
}

void print_real(real_num number)
{
    if (number.mantissa_sign == -1)
        printf("-");
    else
        printf("+");
    printf("0.");

    for (int i = 0; i < number.mantissa_len; i++)
    {
        printf("%d", number.mantissa[i]);
    }    
    printf(" E %d\n", number.order);
}

void print_error(int rc)
{
    printf("---------\n");
    switch (rc)
    {
    case INCORRECT_SYMBOL:
        printf("Введен неверный символ\n");
        break;
    case INCORRECT_INPUT:
        printf("Неверный ввод\n");
        break;
    case TOO_LONG_MANTISSA:
        printf("Слишком длинная мантисса\n");
        break;
    case MACHINE_INF:
        printf("Достигнута машинная бесконечность (переполнение порядка)\n");
        break;
    case MACHINE_ZERO:
        printf("Достигнут машинный ноль (переполнение порядка)\n");
        break;
    case DIVISION_BY_ZERO:
        printf("Деление на ноль\n");
        break;
    default:
        printf("Неизвестная ошибка\n");
        break;
    }
    printf("---------\n");
}
