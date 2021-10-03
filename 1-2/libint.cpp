#include <cstdio>
#include <cstdlib>

#define LEN 1000

int calculate(int a, int b, char operation)
{
    if (operation == '+')
    {
        return a + b;
    } else
    {
        return a - b;
    }
}
