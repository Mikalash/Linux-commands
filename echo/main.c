#include <stdio.h>
#include <stdlib.h>

int comp_n(char* a)
{
    if (a[0] == '-')
        if (a[1] == 'n')
            if (a[2] == '\0')
                return 1;
    return 0;
}

int main(int argc, char* argv[])
{
    for (int i = 1; i < argc; i++)
        if (!comp_n(argv[i]))
        {
            if (i == argc - 1)
                printf("%s", argv[i]);
            else
                printf("%s ", argv[i]);
        }

    if (argc > 1)
    {
        if (!comp_n(argv[1]))
            printf("\n");
    }
    else
        printf("\n");
}
