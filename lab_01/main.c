#include <stdio.h>
#include <string.h>

#include "return_codes.h"
#include "process.h"
#include "io.h"

/*
Смоделировать операцию деления действительного числа в
форме ±m.n Е ±K, где суммарная длина мантиссы (m+n) – не
более 35 значащих цифр, а величина порядка K ≤ 5 цифр, на
целое число длиной ≤ 35 десятичных цифр. Результат выдать
в форме ±0.m1 Е ±K1, где m1 – не более 35 значащих цифр, а
K1 – не более 5 цифр.
*/



int main(void)
{
    printf("======== ЗАДАНИЕ ========\n");
    printf("Деление вещественного числа вида ±m.n Е ±K, у которого длина мантиссы не более 35 значащих цифр\n");
    printf("на целое число длиной не более 35 цифр. Результат выводится в форме ±0.m1 Е ±K1, где m1 – не более 35 значащих цифр, а\n");
    printf("K1 – не более 5 цифр.\n");
    printf("=========================\n");


    real_num dividend;
    integer_num divider;

    real_num result;

    // считывание чисел
    
    int rc;
    char buf[MAX_BUFFER_LEN];

    printf("Введите делимое в формате ±m.n Е ±K, мантисса до 35 цифр, порядок до 5 цифр:\n");
    printf("<---5----10---15---20---25---30---35---40---45--->\n");
    fgets(buf, sizeof(buf), stdin);

    rc = parse_real(buf, &dividend);
    if (rc)
    {
        print_error(rc);
        return rc;
    }
       
    printf("Введите делитель, целое число, не более 35 цифр:\n");    
    printf("<---5----10---15---20---25---30---35---40---45--->\n");
    fgets(buf, sizeof(buf), stdin);

    rc = parse_integer(buf, &divider);
    if (rc)
    {
        print_error(rc);
        return rc;
    }
    
    // ДЕЛЕНИЕ
    printf("=========================\n");
    printf("Делимое:   ");
    print_real(dividend);
    printf("Делитель:  ");
    print_integer(divider);

    rc = division(dividend, divider, &result);
    if (rc)
    {
        print_error(rc);
        return rc;
    }

    printf("Частное:   ");
    print_real(result);

    return OK;  

}

