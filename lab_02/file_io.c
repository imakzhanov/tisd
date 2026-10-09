#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_io.h"
#include "errors.h"

/* Формат строки в файле (поля разделены ';')
    первичное:  address;area;rooms;price_m2;1;has_finishing
    вторичное:  address;area;rooms;price_m2;2;build_year;owners;had_pets
 */

#define LINE_BUF 256

int parse_line(char *line, Flat *f)
{
    char address[MAX_ADDRESS_LEN];
    double area, price_m2;
    int rooms, type_tag;
    int n1, consumed;

    n1 = sscanf(line, "%49[^;];%lf;%d;%lf;%d;%n",
                address, &area, &rooms, &price_m2, &type_tag, &consumed);
    if (n1 != 5)
        return ERR_PARSE;

    memset(f, 0, sizeof(Flat));
    strncpy(f->address, address, MAX_ADDRESS_LEN - 1);
    f->area = area;
    f->rooms = rooms;
    f->price_m2 = price_m2;

    char *rest = line + consumed;
    if (type_tag == 1)
    {
        f->type = PRIMARY;
        int has_finishing;
        if (sscanf(rest, "%d", &has_finishing) != 1)
            return ERR_PARSE;
        f->variable.primary.has_finishing = has_finishing;
    }
    else if (type_tag == 2)
    {
        f->type = SECONDARY;
        int build_year, owners, had_pets;
        if (sscanf(rest, "%d;%d;%d", &build_year, &owners, &had_pets) != 3)
            return ERR_PARSE;
        f->variable.secondary.build_year = build_year;
        f->variable.secondary.owners = owners;
        f->variable.secondary.had_pets = had_pets;
    }
    else
        return ERR_PARSE;
    return OK;
}

static void print_struct_into_line(Flat *f, char *buf, size_t bufsize)
{
    if (f->type == PRIMARY)
        snprintf(buf, bufsize, "%s;%.2f;%d;%.2f;1;%d\n",
                 f->address, f->area, f->rooms, f->price_m2,
                 f->variable.primary.has_finishing);
    else
        snprintf(buf, bufsize, "%s;%.2f;%d;%.2f;2;%d;%d;%d\n",
                 f->address, f->area, f->rooms, f->price_m2,
                 f->variable.secondary.build_year,
                 f->variable.secondary.owners,
                 f->variable.secondary.had_pets);
}

int count_entries(FILE *f, size_t *count)
{
    *count = 0;
    char line[LINE_BUF];

    while (fgets(line, sizeof(line), f) != NULL)
        (*count)++;

    if (*count == 0)
        return ERR_EMPTY_FILE;
    return OK;
}

int load_table(char *filename, Table *t)
{
    FILE *in_file = fopen(filename, "r");
    if (in_file == NULL)
        return ERR_FILE_OPEN;

    size_t count;
    int rc = count_entries(in_file, &count);
    if (rc != OK)
    {
        fclose(in_file);
        return rc;
    }
    rewind(in_file);

    Flat *data = malloc(sizeof(Flat) * count);
    if (data == NULL)
    {
        fclose(in_file);
        return ERR_MEMORY;
    }

    char line[LINE_BUF];
    for (size_t n = 0; n < count; n++)
    {
        if (fgets(line, sizeof(line), in_file) == NULL)
        {
            fclose(in_file);
            free(data);
            return ERR_READ;
        }
        if (parse_line(line, &data[n]) != OK)
        {
            fclose(in_file);
            free(data);
            return ERR_PARSE;
        }
    }
    fclose(in_file);

    t->data = data;
    t->count = (int)count;
    return OK;
}

int save_table(char *filename, Table *t)
{
    FILE *f = fopen(filename, "w");
    if (f == NULL)
        return ERR_FILE_OPEN;

    char buf[LINE_BUF];
    for (int i = 0; i < t->count; i++)
    {
        print_struct_into_line(&t->data[i], buf, sizeof(buf));
        fputs(buf, f);
    }
    fclose(f);
    return OK;
}
