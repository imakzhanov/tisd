#ifndef STRUCT_FUNC_H
#define STRUCT_FUNC_H

#include <stddef.h>
#include "errors.h"

#define MAX_ADDRESS_LEN 50

typedef enum {PRIMARY, SECONDARY} FlatType;

typedef struct {
    char address[MAX_ADDRESS_LEN];
    double area;
    int rooms;
    double price_m2;

    FlatType type;
    union {
        struct {
            int has_finishing;
        } primary;

        struct {
            int build_year;
            int owners;
            int had_pets;
        } secondary;
    } variable;
} Flat;

typedef struct {
    Flat *data;
    int count;
} Table;

typedef struct {
    int index;
    double key;
} KeyEntry;

typedef struct {
    KeyEntry *items;
    int count;
} KeyTable;

typedef enum { ADDRESS_FIELD, AREA_FIELD, ROOMS_FIELD, PRICE_M2_FIELD } FieldId;





void table_init(Table *t);
int table_add(Table *t, Flat f);
int flat_matches_field(Flat *f, FieldId field, void *value);
int table_remove_by_field(Table *t, FieldId field, void *value, int *out_removed);
void table_free(Table *t);

void print_flat_header(void);
void print_flat(Flat *f);
void print_table(Table *table);

int build_key_table(Table *table, KeyTable *key_table);
void key_table_free(KeyTable *key_table);
void print_table_by_key(KeyTable *key_table, Table *table);
void print_key_table(KeyTable *key_table);

#endif
