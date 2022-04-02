int separador_primeiro(int numero)
{
    int aux;
    
    aux = numero%10;

    return aux;
};

int separador_segundo(int numero)
{
    int aux;

    aux = numero/10;
    aux = aux%10;

    return aux;
};

int separador_terceiro(int numero)
{
    int aux;

    aux = numero/10;
    aux = aux/10;
    aux = aux%10;

    return aux;
};