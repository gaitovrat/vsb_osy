#include "sol.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LENGTH 1000

static void set_status(int *status, const char *status_state)
{
    if (!strcmp(status_state, "OK"))
        *status = 0;
    else
        *status = 1;
}

static void print_status(int *statuses, int length)
{
    for (int i = 0; i < length; ++i)
    {
        if (statuses[i] == 0)
        {
            printf("%d %s\n", i, "OK");
        }
        else
        {
            printf("%d %s\n", i, "BAD");
        }
    }
}

static void first_solution(int m)
{
    int *statuses = (int *)malloc(sizeof(int) * m);

    for (int i = 0; i < m; ++i)
    {
        char *str_num = (char *)malloc(sizeof(char) * LENGTH);
        int sum = 0;
        int k = 0;
        char c = 0;

        while ((c = fgetc(stdin)) != '\n')
        {
            if (c == ' ')
            {
                sum += atoi(str_num);
                k = 0;
                memset(str_num, 0, strlen(str_num));
                continue;
            }
            str_num[k++] = c;
        }

        if (sum == atoi(str_num))
            set_status(&statuses[i], "OK");
        else
        {
            set_status(&statuses[i], "BAD");
            fprintf(stderr, "ERROR: row number: %d, expected sum: %d, your sum: %s\n", i, sum, str_num);
        }

        free(str_num);
    }

    print_status(statuses, m);
    free(statuses);
}

void first()
{
    int m = 0;
    printf("Enter number of rows: ");
    scanf("%d", &m);

    if (m == 0)
        exit(EXIT_FAILURE);

#ifdef DEBUG
    fgetc(stdin);
#endif

    first_solution(m);
}
