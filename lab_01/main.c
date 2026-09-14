#include <stdio.h>
#include <stdint.h>

/*
Смоделировать операцию деления действительного числа в
форме ±m.n Е ±K, где суммарная длина мантиссы (m+n) – не
более 35 значащих цифр, а величина порядка K ≤ 5 цифр, на
целое число длиной ≤ 35 десятичных цифр. Результат выдать
в форме ±0.m1 Е ±K1, где m1 – не более 35 значащих цифр, а
K1 – не более 5 цифр.
*/

#define MAX_MANTISSA_LEN 35

typedef struct
{
    int mantissa_sign;
    int mantissa[MAX_MANTISSA_LEN];
    int order;
} Number;

int my_isdigit(char c)
{
    return (c >= '0' && c <= '9');
}

void parse_sign(char buff[], size_t *pos, Number *number)
{
    if (buff[*pos] == '-')
    {
        (*number).mantissa_sign = -1;
        (*pos)++;
    }
    else
    {
        (*number).mantissa_sign = 1;
        if (buff[*pos] == '+')
            (*pos)++;
    }
}

void skip_zero(char buff[], size_t *pos)
{
    while (buff[*pos] == '0')
        (*pos)++;
}

void while_digit(char buff[], size_t *pos, Number *number, size_t mantissa_len)
{
    while (my_isdigit(buff[*pos]))
    {
        number->mantissa[mantissa_len++] = buff[(*pos)++];
    }
}

void skip_spaces(char buff[], size_t *pos)
{
    while (buff[*pos] == ' ')
        (*pos)++;
}

void parse_order()

void parse_mantissa(char buff[], size_t *pos, Number *number)
{
    size_t mantissa_len = 0;
    skip_zero(buff, pos);
    while_digit(buff, pos, number, &mantissa_len);
    skip_spaces(buff, pos);
    if (buff[*pos] == 'E' || buff[*pos] == 'e')
    {
        parse_order()
    }
    else if (buff[*pos] == '\0')
        return ;
}

int main(void)
{
    Number dividend;
    Number divider;

    // считывание чисел

    char buffer[100];
    size_t pos = 0;
    fgets(buffer, sizeof(buffer), stdin);
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n')
        buffer[len - 1] = '\0';
    
    parse_sign(buffer, &pos, &dividend);

    
    
    

}

