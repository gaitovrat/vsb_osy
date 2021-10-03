#include <cstdio>
#include <cstdlib>
#define LEN 1000

int pocitej(char *vysledek, char *priklad)
{
  int number1;
  int number2;
  char operation;
  if(sscanf(priklad, "%d%c%d", &number1, &operation, &number2))


}

int main()
{
    char* buffer = (char*)malloc(sizeof(char) * LEN);

    while(fgets(buffer, LEN, stdin))
    {
        // sscanf(buffer, "%d%c%d", &number1, &operation, &number2);
    }

    return 0;
}
