#include "io.h"
#include <time.h>
#include <stdio.h>

static void print_menu(int show_addresses)
{
    printf("\n");
    printf("1. Массив\n");
    printf("2. Список\n");
    printf("3. Изменить параметры\n");
    printf("4. Адреса узлов списка (%s)\n", show_addresses ? "вкл" : "выкл");
    printf("0. Выход\n");
    printf("> ");
}

int main(void)
{
    // unsigned seed = (unsigned)time(NULL);

    params_t params = default_params;
    int show_addresses = 0;

    for (;;)
    {
        print_menu(show_addresses);
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
