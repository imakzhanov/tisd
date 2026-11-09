#include "io.h"

#include <stdio.h>

int main(void)
{
    params_t params = default_params;
    int show_addresses = 0;

    for (;;)
    {
        printf("\n1. Массив\n2. Список\n3. Изменить параметры\n"
               "4. Адреса узлов списка (%s)\n0. Выход\n> ",
               show_addresses ? "вкл" : "выкл");
        int choice = read_choice();
        int rc = OK;
        switch (choice)
        {
            case '0':
                return OK;
            case '1':
            case '2':
            {
                result_t result;
                queue_kind_t kind = choice == '1' ? QUEUE_ARRAY : QUEUE_LIST;
                rc = simulate(&params, kind, MODEL_SEED, show_addresses, print_progress, &result);
                if (rc == OK)
                    print_result(&result, &params);
                break;
            }
            case '3':
                rc = read_params(&params);
                break;
            case '4':
                show_addresses = !show_addresses;
                break;
            default:
                rc = INPUT_ERROR;
        }
        if (rc != OK)
            print_error(rc);
    }
}
