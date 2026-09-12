#include <stdio.h>
#include <stdlib.h>

#include "registro.h"
#include "funcionalidades.h"
#include "fornecidas.h"

int main()
{
    char nome_arq_1[100];
    char nome_arq_2[87];
    int funcionalidade;
    int n;

    scanf("%d",&funcionalidade);
    scanf("%s",nome_arq_1);

    if( funcionalidade == 1 )
    {
        scanf("%s",nome_arq_2);
        CREATE_TABLE(nome_arq_1,nome_arq_2);
    }
    else if( funcionalidade == 2 )
    {
        SELECT(nome_arq_1);
    }
    else if( funcionalidade == 3 )
    {
        scanf("%d",&n);
        WHERE(nome_arq_1,n);
    }
    else if( funcionalidade == 4 )
    {
        scanf("%d",&n);
        ACCESS(nome_arq_1,n);
    }
    else if( funcionalidade == 5)
    {
        scanf("%d",&n);
        DELETE(nome_arq_1,n);
    }
    else if( funcionalidade == 6)
    {
        scanf("%d",&n);
        INSERT(nome_arq_1,n);
    }
    else if( funcionalidade == 7)
    {
        scanf("%d",&n);
        UPDATE(nome_arq_1,n);
    }
    return 0;
}
