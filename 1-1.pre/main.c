#include <stdio.h>

#include "sol.h"

int main(int argc, const char** argv)
{
    for (int i = 0; i < argc; ++i)
        printf("%s\n", argv[i]);

    test("TEST 1");

    return 0;
}
