#include "random.h"

#include <stdlib.h>
#include <time.h>

int randomNumber(int lower, int upper)
{
    srand(time(NULL));
    return (rand() % (upper - lower + 1)) + lower;
}

int randomNumber(int max)
{
    return randomNumber(0, max);
}

int randomNumber()
{
    return randomNumber(0, 100);
}