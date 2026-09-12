#include "funcionalidades.h"
#include "registro.h"
#include "fornecidas.h"

#include <string.h>
#include <stdlib.h>

/*
    Implementação da Funcionalidade 1
*/
void CREATE_TABLE(char* nome_arq_entrada, char* nome_arq_saida)
{
    FILE *arq_entrada = fopen(nome_arq_entrada,"rt");

    if(arq_entrada == NULL )
    {
        printf("Falha no processamento do arquivo.");
        return;
    }

    FILE *arq_saida = fopen(nome_arq_saida,"wb");

    if(arq_saida == NULL )
    {
        printf("Falha no processamento do arquivo.");
        fclose(arq_entrada);
        return;
    }

    HEADER_REG head = innit_header_reg();
    DATA_REG reg = innit_data_reg();

    // Lê a primeira linha do arquivo de entrada
    // que não contém dados
    char buffer[Tamanho_STRING];

    fgets(buffer,sizeof(buffer),arq_entrada);

    // Escreve o registro de cabeçalho no inicio do arquivo .bin
    Escrever_HEADER_bin(arq_saida,&head);

    // Loop para leitura do arquivo .csv
    while(Ler_registro_csv(arq_entrada,&reg) != 1)
    {
        head.proxRRN++;

        reg.removido = SALVO;

        Escrever_DATA_bin(arq_saida,&reg);
    }

    // Volta ao início do arquivo .bin e atualiza o registro de cabeçalho
    fseek(arq_saida,0,SEEK_SET);

    head.status = CONSISTENTE;

    Escrever_HEADER_bin(arq_saida,&head);

    fclose(arq_entrada);
    fclose(arq_saida);

    nroPares(nome_arq_saida);

    BinarioNaTela(nome_arq_saida);
}

/*
    Implementação da Funcionalidade 2
*/
void SELECT(char* nome_arq_bin)
{
    FILE* arq = fopen(nome_arq_bin,"rb");

    if(arq == NULL)
    {
        printf("Falha no processamento do arquivo.");
        return;
    }

    HEADER_REG head = innit_header_reg();

    DATA_REG reg = innit_data_reg();

    Ler_HEADER_bin(arq,&head);

    if(head.proxRRN == 0)
    {
        printf("Registro inexistente.");
        fclose(arq);
        return;
    }

    int status_leitura;

    // Loop de leitura dos registros de dados
    while( (status_leitura = Ler_DATA_bin(arq,&reg)) != 1)
    {
        // Printa o registro somente se não está logicamente removido
        if (status_leitura == -1)
        continue;

        Printar_DATA_REG(&reg);
    }

    // Fecha o arquivo
    fclose(arq);
}

/*
    Função auxiliar que irá ler os dados no terminal do registro dado como referência
    para buscá-lo em seguida
    Utilizado nas funções WHERE() , DELETE() e UPDATE()
*/
ASSIST_REG Registro_de_referencia()
{
    // Inicializa variáveis auxiliares
    DATA_REG r = innit_data_reg();
    ASSIST_REG r_ref = (ASSIST_REG){0,r,NAO,NAO,NAO,NAO};

    // Buffers para leitura
    char nome_do_campo[100];
    char valor[10];

    // Quantidade de campos
    int m;

    scanf("%d",&m);

    while(m--)
    {
        scanf("%99s",nome_do_campo);
        scanf("%9s",valor);

        // Processo lógico para definir o registro de referência
        if(strcmp(nome_do_campo,"unidadeMedida") == 0)
        {
            if(strcmp(valor,"NULO") == 0)
                r_ref.r.unidadeMedida = LIXO;
            else
                r_ref.r.unidadeMedida = valor[1];
            r_ref.usa_un_medida = SIM;
        }
        else if( strcmp(nome_do_campo,"idPoPs") == 0)
        {
            if(strcmp(valor,"NULO")== 0)
                r_ref.r.idPoPs = NULO;
            else
                r_ref.r.idPoPs = atoi(valor);

            r_ref.usa_idpops = SIM;
        }
        else if( strcmp(nome_do_campo,"idPoPsConectado") == 0)
        {
            if(strcmp(valor,"NULO") == 0)
                r_ref.r.idPopsConectado = NULO;
            else
                r_ref.r.idPopsConectado = atoi(valor);

            r_ref.usa_idconecta = SIM;
        }
        else if( strcmp(nome_do_campo,"velocidade") == 0)
        {
            if(strcmp(valor,"NULO") == 0)
                r_ref.r.velocidade = NULO;
            else
                r_ref.r.velocidade = atoi(valor);

            r_ref.usa_velocidade = SIM;
        }
        else
        {
            printf("CAMPO INVALIDO\n");
        }
    }

    // Retorna o registro de referência
    return r_ref;
}

/*
    Implementação da Funcionalidade 3
*/
void WHERE(char *nome_arq_bin, int n)
{
    FILE *arq = fopen(nome_arq_bin,"rb");

    if(arq == NULL)
    {
        printf("Falha no processamento do arquivo.");
        return;
    }

    HEADER_REG head = innit_header_reg();

    Ler_HEADER_bin(arq,&head);

    if(head.proxRRN == 0)
    {
        printf("Registro inexistente.");
        fclose(arq);
        return;
    }

    while(n--)
    {
        DATA_REG r = innit_data_reg();

        ASSIST_REG r_ref = Registro_de_referencia();
        ASSIST_REG r_copia = (ASSIST_REG){0,r,NAO,NAO,NAO,NAO};

        int encontrado = 0;

        // Loop que irá buscar o registro
        while(Busca_Sequencial(arq,&r_ref,&r_copia) == 0)
        {
            int RRN = r_copia.RRN;

            // Caso encontre o registro será printado
            Printar_DATA_REG(&(r_copia.r));
            encontrado = 1;

            r_ref.RRN = RRN + 1;
        }

        // Condição de registro não encontrado
        if(encontrado == 0)
        {
            printf("Registro inexistente.");
        }

        // Volta o ponteiro do arquivo para o início dos registros de dados
        // para realizar um nova busca de um registro de referência
        fseek(arq, sizeofHEADER , SEEK_SET);

        printf("\n");
    }

    // Fecha o arquivo
    fclose(arq);
}

/*
    Implementação da Funcionalidade 4
*/
void ACCESS(char *nome_arq_bin,int RRN)
{
    FILE *arq = fopen(nome_arq_bin,"rb");

    if(arq == NULL)
    {
        printf("Falha no processamento do arquivo.");
        return;
    }

    HEADER_REG head = innit_header_reg();
    DATA_REG reg = innit_data_reg();

    Ler_HEADER_bin(arq,&head);

    if(head.proxRRN == 0 || RRN >= head.proxRRN )
    {
        printf("Registro inexistente.");
        fclose(arq);
        return;
    }

    // Posiciona o ponteiro para o local correto
    fseek(arq, sizeofHEADER + RRN * sizeofDATA, SEEK_SET);

    // Faz a leitura
    int status_leitura = Ler_DATA_bin(arq,&reg);

    // Fecha o arquivo
    fclose(arq);

    if( status_leitura != 0 )
        printf("Registro inexistente.");
    else
        Printar_DATA_REG(&reg);
}


/*
    Implementação da Funcionalidade 5
*/
void DELETE(char *nome_arq_bin, int n)
{
    FILE *arq = fopen(nome_arq_bin, "r+b");

    if (arq == NULL)
    {
        printf("Falha no processamento do arquivo.");
        return;
    }

    HEADER_REG head = innit_header_reg();
    Ler_HEADER_bin(arq, &head);

    if (head.proxRRN == 0)
    {
        printf("Registro inexistente.");
        fclose(arq);
        return;
    }

    // Marca o arquivo como inconsistente antes da escrita.
    head.status = INCONSISTENTE;

    fseek(arq, 0, SEEK_SET);
    Escrever_HEADER_bin(arq, &head);

    for (int i = 0; i < n; i++)
    {
        ASSIST_REG r_ref = Registro_de_referencia();

        DATA_REG r = innit_data_reg();

        ASSIST_REG r_copia = (ASSIST_REG){0, r, NAO, NAO, NAO, NAO};

        // Começa a busca no primeiro registro.
        fseek(arq, sizeofHEADER, SEEK_SET);

        while (Busca_Sequencial(arq, &r_ref, &r_copia) == 0)
        {
            int RRN = r_copia.RRN;

            Remover_DATA_bin(arq, &head, RRN);

            // Continua a busca a partir do próximo RRN.
            r_ref.RRN = RRN + 1;
        }
    }

    // Atualiza o cabeçalho com os valores finais.
    head.status = CONSISTENTE;

    fseek(arq, 0, SEEK_SET);
    Escrever_HEADER_bin(arq, &head);

    fclose(arq);

    nroPares(nome_arq_bin);

    BinarioNaTela(nome_arq_bin);
}

void INSERT(char *nome_arq_bin, int n)
{
    FILE *arq = fopen(nome_arq_bin, "r+b");

    if (arq == NULL)
    {
        printf("Falha no processamento do arquivo.");
        return;
    }

    HEADER_REG head = innit_header_reg();
    Ler_HEADER_bin(arq, &head);

    // Marca o arquivo como inconsistente antes da escrita.
    head.status = INCONSISTENTE;

    fseek(arq, 0, SEEK_SET);
    Escrever_HEADER_bin(arq, &head);

    for(int i = 0 ; i < n ; i++)
    {
        DATA_REG r_novo = innit_data_reg();
        DATA_REG r_aux = innit_data_reg();

        Ler_NOVO_DATA_REG(&r_novo);

        if( head.topoPilha == -1)
        {
            fseek(arq, sizeofHEADER + (head.proxRRN * sizeofDATA), SEEK_SET);

            Escrever_DATA_bin(arq,&r_novo);

            head.proxRRN++;
        }
        else
        {
            long posicao = sizeofHEADER + (head.topoPilha * sizeofDATA);

            fseek(arq, posicao, SEEK_SET);

            Ler_DATA_bin(arq,&r_aux);

            head.topoPilha = r_aux.encadeamentoPilha;
            head.nroRegRem--;

            fseek(arq, posicao, SEEK_SET);

            Escrever_DATA_bin(arq,&r_novo);
        }
    }

    head.status = CONSISTENTE;

    fseek(arq, 0, SEEK_SET);
    Escrever_HEADER_bin(arq, &head);

    fclose(arq);

    nroPares(nome_arq_bin);
    BinarioNaTela(nome_arq_bin);
}


/*
    Implementação da Funcionalidade 7
*/
void UPDATE(char *nome_arq_bin, int n)
{
    FILE *arq = fopen(nome_arq_bin, "r+b");

    if (arq == NULL)
    {
        printf("Falha no processamento do arquivo.");
        return;
    }

    HEADER_REG head = innit_header_reg();
    Ler_HEADER_bin(arq, &head);

    if (head.proxRRN == 0)
    {
        printf("Registro inexistente.");
        fclose(arq);
        return;
    }

    // Marca o arquivo como inconsistente.
    head.status = INCONSISTENTE;

    fseek(arq, 0, SEEK_SET);
    Escrever_HEADER_bin(arq, &head);

    for (int i = 0; i < n; i++)
    {
        // Obtém os registros que deverão ser buscados
        ASSIST_REG r_ref = Registro_de_referencia();

        // Obtém as atualizações dos registros
        ASSIST_REG r_atualizacoes = Registro_de_referencia();

        r_ref.RRN = 0;

        DATA_REG r = innit_data_reg();

        ASSIST_REG r_copia = (ASSIST_REG){0, r, NAO, NAO, NAO, NAO};

        // Reinicia a busca no início dos dados.
        fseek(arq, sizeofHEADER, SEEK_SET);

        while (Busca_Sequencial(arq, &r_ref, &r_copia) == 0)
        {
            int RRN = r_copia.RRN;

            // Atualiza o registro encontrado.
            Atualizar_DATA_bin(arq, RRN,&r_atualizacoes, &r_copia);

            // Continua a busca após o registro atualizado.
            r_ref.RRN = RRN + 1;
        }
    }

    head.status = CONSISTENTE;

    fseek(arq, 0, SEEK_SET);
    Escrever_HEADER_bin(arq, &head);

    fclose(arq);

    nroPares(nome_arq_bin);
    BinarioNaTela(nome_arq_bin);
}
