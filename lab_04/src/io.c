#include <math.h>
#include <stdio.h>

#include "errors.h"
#include "io.h"

// Проверяет остаток строки после числа
int finish_input(int rc)
{
    if (rc == EOF)
        return ERR_EOF;

    int c = 0;
    if (rc == 1)
    {
        c = getchar();
        if (c == '\n')
            return OK;
    }
    while (c != '\n' && c != EOF)
        c = getchar();
    return ERR_INPUT;
}

int read_int(char *text, int min, int max, int *value)
{
    printf("%s [от %d до %d]: ", text, min, max);
    int rc = finish_input(scanf("%d", value));

    if (rc == OK && (*value < min || *value > max))
        return ERR_RANGE;
    return rc;
}

int read_double(char *text, double *value)
{
    printf("%s: ", text);
    int rc = finish_input(scanf("%lf", value));

    return rc;
}
