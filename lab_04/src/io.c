#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "errors.h"
#include "io.h"

int read_line(char *buf, size_t size)
{
    if (fgets(buf, size, stdin) == NULL)
        return ERR_EOF;

    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n')
        buf[--len] = '\0';
    else if (len == size - 1)
    {
        // строка не поместилась в буфер, остаток выбрасывается
        int c;
        while ((c = getchar()) != '\n' && c != EOF)
            ;
        return ERR_INPUT;
    }

    return OK;
}

int read_int(const char *prompt, int min, int max, int *value)
{
    char line[LINE_SIZE];
    char extra;
    int rc;

    printf("%s [от %d до %d]: ", prompt, min, max);
    rc = read_line(line, sizeof(line));
    if (rc != OK)
        return rc;
    if (sscanf(line, "%d %c", value, &extra) != 1)
        return ERR_INPUT;
    if (*value < min || *value > max)
        return ERR_RANGE;
    return OK;
}

int read_double(const char *prompt, double *value)
{
    char line[LINE_SIZE];
    char *end;
    int rc;

    printf("%s: ", prompt);
    rc = read_line(line, sizeof(line));
    if (rc != OK)
        return rc;

    *value = strtod(line, &end);
    if (end == line)
        return ERR_INPUT;
    while (isspace((unsigned char)*end))
        end++;
    if (*end != '\0' || !isfinite(*value))
        return ERR_INPUT;
    return OK;
}
