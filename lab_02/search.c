#include "search.h"

int search_by_variant(Table *table, FlatType type, SubFieldId field, int value, Table *out)
{
    for (int i = 0; i < table->count; i++)
    {
        Flat *f = &table->data[i];
        if (f->type != type)
            continue;

        int match = 0;
        if (type == PRIMARY && field == SUB_HAS_FINISHING)
            match = (f->variable.primary.has_finishing == value);
        else if (type == SECONDARY)
        {
            switch (field)
            {
                case SUB_BUILD_YEAR:
                    match = (f->variable.secondary.build_year == value);
                    break;
                case SUB_OWNERS:
                    match = (f->variable.secondary.owners == value);
                    break;
                case SUB_HAD_PETS:
                    match = (f->variable.secondary.had_pets == value);
                    break;
                default:
                    match = 0;
            }
        }

        if (match)
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
