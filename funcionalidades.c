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
    FILE *arq_saida = fopen(nome_arq_saida,"wb");

    if(arq_entrada == NULL || arq_saida == NULL)
    {
        printf("Falha no processamento do arquivo.");
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
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    HEADER_REG head = innit_header_reg();

    DATA_REG reg = innit_data_reg();

    Ler_HEADER_bin(arq,&head);

    if(head.proxRRN == 0)
    {
        printf("Registro inexistente.\n");
        return;
    }

    // Loop de leitura dos registros de dados
    while(Ler_DATA_bin(arq,&reg) != 1)
    {
        // Printa o registro somente se não está logicamente removido
        if(reg.removido != REMOVIDO)
            PRINTAR_REGISTRO(&reg);
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
        scanf("%s",nome_do_campo);

        // Processo lógico para definir o registro de referência
        if(strcmp(nome_do_campo,"unidadeMedida") == 0)
        {
            ScanQuoteString(valor);
            if(strcmp(valor,"NULO") == 0)
                r_ref.r.unidadeMedida = LIXO;
            else
                r_ref.r.unidadeMedida = valor[0];
            r_ref.usa_un_medida = SIM;
        }
        else if( strcmp(nome_do_campo,"idPoPs") == 0)
        {
            scanf("%s",valor);
            if(strcmp(valor,"NULO")== 0)
                r_ref.r.idPoPs = NULO;
            else
                r_ref.r.idPoPs = atoi(valor);

            r_ref.usa_idpops = SIM;
        }
        else if( strcmp(nome_do_campo,"idPoPsConectado") == 0)
        {
            scanf("%s",valor);
            if(strcmp(valor,"NULO") == 0)
                r_ref.r.idPopsConectado = NULO;
            else
                r_ref.r.idPopsConectado = atoi(valor);

            r_ref.usa_idconecta = SIM;
        }
        else if( strcmp(nome_do_campo,"velocidade") == 0)
        {
            scanf("%s",valor);
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
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    HEADER_REG head = innit_header_reg();

    Ler_HEADER_bin(arq,&head);

    if(head.proxRRN == 0)
    {
        printf("Registro inexistente.\n");
        return;
    }

    while(n--)
    {
        DATA_REG r = innit_data_reg();

        ASSIST_REG r_ref = Registro_de_referencia();
        ASSIST_REG r_copia = (ASSIST_REG){0,r,NAO,NAO,NAO,NAO};

        // Flag
        // 0 se registro não foi encontrado
        // 1 caso contrário
        int encontrado = 0;

        // Loop que irá buscar o registro
        while(Busca_Sequencial(arq,&r_ref,&r_copia) != 1)
        {
            // Caso encontre o registro será printado
            PRINTAR_REGISTRO(&(r_copia.r));
            encontrado = 1;
        }

        // Condição de registro não encontrado
        if(encontrado == 0)
        {
            printf("Registro inexistente.\n");
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
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    HEADER_REG head = innit_header_reg();
    DATA_REG reg = innit_data_reg();

    Ler_HEADER_bin(arq,&head);

    if(head.proxRRN == 0 || RRN >= head.proxRRN )
    {
        printf("Registro inexistente.\n");
        return;
    }

    // Posiciona o ponteiro para o local correto
    fseek(arq, RRN * sizeofDATA ,SEEK_CUR);

    // Faz a leitura
    Ler_DATA_bin(arq,&reg);

    // Fecha o arquivo
    fclose(arq);

    // Printa o registro
    PRINTAR_REGISTRO(&reg);
}

// Função auxiliar da funcionalidade DELETE()
// Recebe um array de inteiros de RRNs para remover e atualizar o cabeçalho
void Remocao(char *nome_arq_bin, int qtd_removidos, int* RRNs)
{
    FILE* arq = fopen(nome_arq_bin, "r+b");

    if(arq == NULL)
    {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    HEADER_REG head = innit_header_reg();

    Ler_HEADER_bin(arq, &head);

    int topoPilha = head.topoPilha;

    for(int i = 0; i < qtd_removidos; i++)
    {
        // Altera o cursor do arquivo na posição do RRN para remoção
        fseek(arq, sizeofHEADER + (RRNs[i] * sizeofDATA), SEEK_SET);

        // Escreve 1 no campo de removido
        fwrite("1", sizeof(char), 1, arq);
        // Escreve o topoPilha no campo encadeamentoPilha
        fwrite(&topoPilha, sizeof(int), 1, arq);

        //Atualiza o topoPilha
        topoPilha = RRNs[i];

        // Incrementa a quantidade de registros removidos
        head.nroRegRem++;
    }

    // Atribui o topoPilha no cabeçalho
    head.topoPilha = topoPilha;

    //Volta o cursor no local do cabeçalho
    fseek(arq, 0, SEEK_SET);

    // Escreve o cabeçalho atualizado
    Escrever_HEADER_bin(arq, &head);

    // Fecha o arquivo
    fclose(arq);
}

/*
    Implementação da Funcionalidade 5
*/
void DELETE(char *nome_arq_bin, int n)
{
    FILE *arq = fopen(nome_arq_bin, "rb");

    if(arq == NULL)
    {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    HEADER_REG head = innit_header_reg();
    Ler_HEADER_bin(arq, &head);

    if(head.proxRRN == 0)
    {
        printf("Registro inexistente.\n");
        fclose(arq);
        return;
    }

    // Aloca tamanho seguro (o máximo possível de registros)
    int* RRNs_Removidos = (int*)malloc(head.proxRRN * sizeof(int));

    // Contador independente para o vetor
    int qtd_removidos = 0;

    // O fseek inicial
    fseek(arq, sizeofHEADER, SEEK_SET);

    for(int i = 0; i < n; i++)
    {
        DATA_REG r = innit_data_reg();
        ASSIST_REG r_ref = Registro_de_referencia();
        ASSIST_REG r_copia = (ASSIST_REG){0, r, NAO, NAO, NAO, NAO};

        //int encontrado = 0;

        // Assumindo que Busca_Sequencial retorna 0 ao encontrar
        while(Busca_Sequencial(arq, &r_ref, &r_copia) == 0)
        {
            //encontrado = 1;

            // Salva o RRN e incrementa o contador do vetor
            RRNs_Removidos[qtd_removidos] = r_copia.RRN;
            qtd_removidos++;
        }

        /*
        if(encontrado == 0)
        {
            printf("Registro inexistente.\n");
        }
        */

        // Volta o ponteiro do arquivo para o início dos registros de dados
        // para realizar um nova busca de um registro de referência
        fseek(arq, sizeofHEADER, SEEK_SET);
    }

    // Fechar o arquivo de leitura ANTES de abrir o de escrita na Remocao()!
    fclose(arq);

    if (qtd_removidos > 0)
    {
        Remocao(nome_arq_bin, qtd_removidos, RRNs_Removidos);
    }

    // Libera a memória alocada no heap!
    free(RRNs_Removidos);

    BinarioNaTela(nome_arq_bin);
}

void Concatena_reg(UPDATE_REG* r_dst, ASSIST_REG* r_atualizacoes, ASSIST_REG* r_copia)
{
    r_dst->RRN = r_copia->RRN;

    r_dst->r = r_copia->r;

    if(r_atualizacoes->usa_idpops == SIM)
    {
        r_dst->r.idPoPs = r_atualizacoes->r.idPoPs;
    }
    if(r_atualizacoes->usa_idconecta == SIM)
    {
        r_dst->r.idPopsConectado = r_atualizacoes->r.idPopsConectado;
    }
    if(r_atualizacoes->usa_velocidade == SIM)
    {
        r_dst->r.velocidade = r_atualizacoes->r.velocidade;
    }
    if(r_atualizacoes->usa_un_medida == SIM)
    {
        r_dst->r.unidadeMedida = r_atualizacoes->r.unidadeMedida;
    }
}

void Atualizacao(char* nome_arq_bin,int qtd_atualizacoes ,UPDATE_REG* reg)
{
    FILE* arq = fopen(nome_arq_bin, "r+b");

    if(arq == NULL)
    {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    for(int i = 0 ; i < qtd_atualizacoes ; i++)
    {
        fseek(arq,sizeofHEADER + (reg[i].RRN * sizeofDATA), SEEK_SET);

        Escrever_DATA_bin(arq,&(reg[i].r));
    }

    fclose(arq);
}

void UPDATE(char* nome_arq_bin, int n)
{
    FILE *arq = fopen(nome_arq_bin, "rb");

    if(arq == NULL)
    {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    HEADER_REG head = innit_header_reg();
    Ler_HEADER_bin(arq, &head);

    if(head.proxRRN == 0)
    {
        printf("Registro inexistente.\n");
        fclose(arq);
        return;
    }

    UPDATE_REG* Reg_Atualizados = (UPDATE_REG*)malloc(head.proxRRN * sizeof(UPDATE_REG));

    int qtd_atualizacoes = 0;

    for(int i = 0 ; i < n ; i++)
    {
        ASSIST_REG r_ref = Registro_de_referencia();
        ASSIST_REG r_atualizacoes = Registro_de_referencia();

        r_ref.RRN = 0;

        DATA_REG r = innit_data_reg();
        ASSIST_REG r_copia = (ASSIST_REG){0, r, NAO, NAO, NAO, NAO};

        //int encontrado = 0;

        while(Busca_Sequencial(arq, &r_ref, &r_copia) == 0)
        {
            //encontrado = 1;

            Concatena_reg(&(Reg_Atualizados[qtd_atualizacoes]),&r_atualizacoes,&r_copia);

            qtd_atualizacoes++;

            r_ref.RRN = r_copia.RRN + 1;
        }

        /*
        if(encontrado == 0)
        {
            printf("Registro inexistente.\n");
        }
        */
        // Volta o ponteiro do arquivo para o início dos registros de dados
        // para realizar um nova busca de um registro de referência
        fseek(arq, sizeofHEADER, SEEK_SET);
    }

    fclose(arq);

    if(qtd_atualizacoes > 0)
    {
        Atualizacao(nome_arq_bin,qtd_atualizacoes,Reg_Atualizados);
    }

    free(Reg_Atualizados);

    BinarioNaTela(nome_arq_bin);
}
