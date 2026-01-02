#include <stdio.h>

int main()
{
    char mass[100] = {0};

    scanf("%[A-Za-z0-9_]", mass);

    printf("%s\n", mass);

    return 0;
}