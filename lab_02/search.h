#ifndef SEARCH_H
#define SEARCH_H

#include "struct_func.h"
#include "errors.h"

typedef enum { SUB_HAS_FINISHING, SUB_BUILD_YEAR, SUB_OWNERS, SUB_HAD_PETS } SubFieldId;

int search_by_task(Table *table, double min_price, double max_price, Table *out);

#endif
