#include "search.h"

int search_by_task(Table *table, double min_price, double max_price, Table *out)
{
    for (int i = 0; i < table->count; i++)
    {
        Flat *f = &table->data[i];
        if (f->type == SECONDARY &&
            f->rooms == 2 &&
            !f->variable.secondary.had_pets &&
            f->price_m2 >= min_price && f->price_m2 <= max_price)
        {
            int err = table_add(out, *f);
            if (err != OK)
                return err;
        }
    }
    if (out->count == 0)
        return ERR_NOT_FOUND;
    return OK;
}
