#include <stdio.h>
#include "separador_digitos.h"

int main()
{
    int entrada = 256;
    int resultado0, resultado1, resultado2;

    resultado0 = separador_primeiro(entrada);
    resultado1 = separador_segundo(entrada);
    resultado2 = separador_terceiro(entrada);

    printf("\n");
    if(resultado0 == 6 && resultado1 == 5 && resultado2 == 2)
    {
        printf("PPPPPP AAAAA SSSSS SSSSS\n");
        printf("PP  PP A   A SS    SS   \n");
        printf("PPPPPP AAAAA SSSSS SSSSS\n");
        printf("PP     A   A    SS    SS\n");
        printf("PP     A   A SSSSS SSSSS\n");
    }
    else
    {
        printf("FFFFF AAAAA II LL    LL    \n");
        printf("FF    A   A II LL    LL    \n");
        printf("FFFFF AAAAA II LL    LL    \n");
        printf("FF    A   A II LL    LL    \n");
        printf("FF    A   A II LLLLL LLLLL \n");
    }
    printf("entrada  : %d \n", entrada);
    printf("resultado: %d %d %d \n", resultado2, resultado1, resultado0);
    return 0;
}

