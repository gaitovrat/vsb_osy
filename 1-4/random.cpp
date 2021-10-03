#include "random.h"

#include <stdlib.h>
#include <time.h>

int randomNumbers(int lower, int upper)
{
    srand(time(NULL));
    return (rand() % (upper - lower + 1)) + lower;
}

int randomNumbers(int max)
{
    return randomNumbers(0, max);
}

int randomNumbers()
{
    return randomNumbers(0, 100);
}