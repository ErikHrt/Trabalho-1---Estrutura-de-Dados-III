#include "registro.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char delimitador[] = ", \n";

// Inicialização do Registro de Cabeçalho
HEADER_REG innit_header_reg()
{
    HEADER_REG h_reg = (HEADER_REG)
    {
        INCONSISTENTE,
        NULO,
        0,
        0,
        0
    };

    return h_reg;
}

// Inicialização do Registro de Dados
DATA_REG innit_data_reg()
{
    DATA_REG reg = (DATA_REG)
    {
        SALVO,
        NULO,
        NULO,
        NULO,
        NULO,
        LIXO
    };

    return reg;
}

/*
    Função auxiliar que imita a funcao strsep() não disponível no Windows
    Necessária para lidar com campos vazios nos arquivos .csv
*/
char *strsep_WIN(char **stringp, const char *delim)
{
    char *start;
    char *p;

    if (stringp == NULL || *stringp == NULL)
        return NULL;

    start = *stringp;
    p = strpbrk(start, delim);

    if (p != NULL)
    {
        *p = '\0';
        *stringp = p + 1;
    }
    else
        *stringp = NULL;

    return start;
}

/*
    Lê uma linha do arquivo .csv de entrada
    faz o parsing e armazena na struct de registro de dados

    Retorna 1 se chegou ao fim do arquivo,
    ou 0 caso contrário
*/
int Ler_registro_csv(FILE *arq_entrada, DATA_REG *reg)
{
    char buffer[Tamanho_STRING];

    if (fgets(buffer, Tamanho_STRING, arq_entrada) == NULL)
        return 1;

    char *ptr_leitura = buffer;
    char *token;

    token = strsep_WIN(&ptr_leitura, delimitador);
    if (token != NULL && token[0] != '\0' && token[0] != ' ')
        reg->idPoPs = atoi(token);
    else reg->idPoPs = NULO;

    token = strsep_WIN(&ptr_leitura, delimitador);
    if (token != NULL && token[0] != '\0' && token[0] != ' ')
        reg->idPopsConectado = atoi(token);
    else reg->idPopsConectado = NULO;

    token = strsep_WIN(&ptr_leitura, delimitador);
    if (token != NULL && token[0] != '\0' && token[0] != ' ')
        reg->velocidade = atoi(token);
    else reg->velocidade = NULO;

    token = strsep_WIN(&ptr_leitura, delimitador);
    if (token != NULL && token[0] != '\0' && token[0] != ' ')
        reg->unidadeMedida = token[0];
    else reg->unidadeMedida = LIXO;

    return 0;
}


// Escreve um registro de dado no arquivo binário
void Escrever_DATA_bin(FILE* arq,DATA_REG* reg)
{
    fwrite(&(reg->removido),sizeof(char),1,arq);

    fwrite(&(reg->encadeamentoPilha),sizeof(int),1,arq);

    fwrite(&(reg->idPoPs),sizeof(int),1,arq);

    fwrite(&(reg->idPopsConectado),sizeof(int),1,arq);

    fwrite(&(reg->velocidade),sizeof(int),1,arq);

    fwrite(&(reg->unidadeMedida),sizeof(char),1,arq);
}

// Escreve um registro de cabeçalho no arquivo binário
void Escrever_HEADER_bin(FILE* arq, HEADER_REG* hreg)
{
    fwrite(&(hreg->status),sizeof(char),1,arq);

    fwrite(&(hreg->topoPilha),sizeof(int),1,arq);

    fwrite(&(hreg->proxRRN),sizeof(int),1,arq);

    fwrite(&(hreg->nroRegRem),sizeof(int),1,arq);

    fwrite(&(hreg->nroPares),sizeof(int),1,arq);
}

/*
    Lê um registro de dado do arquivo binário
    Retorna:
    0  - caso bem-sucedido
    -1 - caso o registro foi logicamente removido
    1  - caso tenha chegado ao fim do arquivo
*/
int Ler_DATA_bin(FILE *arq, DATA_REG *reg)
{
    if (arq == NULL || reg == NULL)
        return 1;

    if (fread(&reg->removido, sizeof(char), 1, arq) != 1)
        return 1;

    if (reg->removido == REMOVIDO)
    {
        if (fseek(arq, sizeofDATA - sizeof(char), SEEK_CUR) != 0)
            return 1;

        return -1;
    }

    if (fread(&reg->encadeamentoPilha, sizeof(int), 1, arq) != 1)
        return 1;

    if (fread(&reg->idPoPs, sizeof(int), 1, arq) != 1)
        return 1;

    if (fread(&reg->idPopsConectado, sizeof(int), 1, arq) != 1)
        return 1;

    if (fread(&reg->velocidade, sizeof(int), 1, arq) != 1)
        return 1;

    if (fread(&reg->unidadeMedida, sizeof(char), 1, arq) != 1)
        return 1;

    return 0;
}

/*
    Lê um registro de cabeçalho
    Retorna:
    0 - caso bem-sucedido
    1 - caso contrário
*/
int Ler_HEADER_bin(FILE* arq, HEADER_REG* hreg)
{
    if (fread(&hreg->status, sizeof(char), 1, arq) != 1)
        return 1;

    if (fread(&hreg->topoPilha, sizeof(int), 1, arq) != 1)
        return 1;

    if (fread(&hreg->proxRRN, sizeof(int), 1, arq) != 1)
        return 1;

    if (fread(&hreg->nroRegRem, sizeof(int), 1, arq) != 1)
        return 1;

    if (fread(&hreg->nroPares, sizeof(int), 1, arq) != 1)
        return 1;

    return 0;
}

/*
    Realiza um busca sequencial até achar um registro descrito pelo
    registro de referência (r_ref) e copia-o todo no registro r_copia
    Retorna:
    0 - caso tenha encontrado algum
    1 - caso tenha ocorrido algum erro ou chegado ao fim do arquivo
*/
int Busca_Sequencial(FILE *arq, ASSIST_REG *r_ref, ASSIST_REG *r_copia)
{
    int rrn_atual = r_ref->RRN;
    int status_leitura;

    if (arq == NULL || r_ref == NULL || r_copia == NULL)
        return 1;

    if (fseek(arq, rrn_atual * sizeofDATA + sizeofHEADER, SEEK_SET) != 0)
        return 1;

    while ((status_leitura = Ler_DATA_bin(arq, &(r_copia->r))) != 1)
    {
        if (status_leitura == -1)
        {
            rrn_atual++;
            continue;
        }

        if (r_ref->usa_idpops == SIM && r_ref->r.idPoPs != r_copia->r.idPoPs)
        {
            rrn_atual++; continue;
        }

        if (r_ref->usa_idconecta == SIM && r_ref->r.idPopsConectado != r_copia->r.idPopsConectado)
        {
            rrn_atual++; continue;
        }

        if (r_ref->usa_velocidade == SIM && r_ref->r.velocidade != r_copia->r.velocidade)
        {
            rrn_atual++; continue;
        }

        if (r_ref->usa_un_medida == SIM && r_ref->r.unidadeMedida != r_copia->r.unidadeMedida)
        {
            rrn_atual++; continue;
        }

        r_copia->RRN = rrn_atual;
        return 0;
    }

    return 1;
}

void Ler_NOVO_DATA_REG(DATA_REG* reg)
{
    char buffer[Tamanho_STRING];

    scanf("%99s",buffer);

    if(strcmp(buffer,"NULO") == 0)
        reg->idPoPs = NULO;
    else
        reg->idPoPs = atoi(buffer);

    scanf("%99s",buffer);

    if(strcmp(buffer,"NULO") == 0)
        reg->idPopsConectado = NULO;
    else
        reg->idPopsConectado = atoi(buffer);

    scanf("%99s",buffer);

    if(strcmp(buffer,"NULO")== 0)
        reg->velocidade = NULO;
    else
        reg->velocidade = atoi(buffer);

    scanf("%99s",buffer);

    if(strcmp(buffer,"NULO")== 0)
        reg->unidadeMedida = LIXO;
    else
        reg->unidadeMedida = buffer[1];
}


// Print do Registro
void Printar_DATA_REG(DATA_REG* reg)
{
    if(reg->idPoPs == NULO)
        printf("NULO ");
    else
        printf("%d ",reg->idPoPs);

    if(reg->idPopsConectado == NULO)
        printf("NULO ");
    else
        printf("%d ",reg->idPopsConectado);

    if(reg->velocidade == NULO)
        printf("NULO ");
    else
        printf("%d ",reg->velocidade);

    if(reg->unidadeMedida == LIXO)
        printf("NULO\n");
    else
        printf("\"%c\"\n",reg->unidadeMedida);
}

void Remover_DATA_bin(FILE *arq, HEADER_REG *head, int RRN)
{
    char removido = REMOVIDO;

    int tamanho_lixo = sizeofDATA - 5;
    char lixo[tamanho_lixo];
    memset(lixo, LIXO, tamanho_lixo);

    // Posiciona no início do registro a ser removido.
    fseek(arq, sizeofHEADER + (RRN * sizeofDATA), SEEK_SET);

    // Campo removido.
    fwrite(&removido, sizeof(char), 1, arq);

    // Encadeamento da pilha de removidos.
    fwrite(&head->topoPilha, sizeof(int), 1, arq);

    // Campos de dados preenchidos com lixo.
    fwrite(lixo, sizeof(char), tamanho_lixo, arq);

    // Atualiza o cabeçalho em memória.
    head->topoPilha = RRN;
    head->nroRegRem++;
}

void Concatena_reg(DATA_REG* r_dst, ASSIST_REG* r_atualizacoes, ASSIST_REG* r_copia)
{
    *r_dst = r_copia->r;

    if(r_atualizacoes->usa_idpops == SIM)
    {
        r_dst->idPoPs = r_atualizacoes->r.idPoPs;
    }
    if(r_atualizacoes->usa_idconecta == SIM)
    {
        r_dst->idPopsConectado = r_atualizacoes->r.idPopsConectado;
    }
    if(r_atualizacoes->usa_velocidade == SIM)
    {
        r_dst->velocidade = r_atualizacoes->r.velocidade;
    }
    if(r_atualizacoes->usa_un_medida == SIM)
    {
        r_dst->unidadeMedida = r_atualizacoes->r.unidadeMedida;
    }
}

void Atualizar_DATA_bin(FILE *arq, int RRN, ASSIST_REG *r_atualizacoes, ASSIST_REG *r_copia)
{
    DATA_REG reg_atualizado = innit_data_reg();

    // Combina os dados antigos com os novos valores.
    Concatena_reg(&reg_atualizado, r_atualizacoes, r_copia);

    // Posiciona no registro original.
    fseek(arq, sizeofHEADER + (RRN * sizeofDATA), SEEK_SET);

    // Escreve imediatamente os campos atualizados.
    Escrever_DATA_bin(arq, &reg_atualizado);
}


void nroPares(char *nome_arq_bin)
{
    FILE * arq = fopen(nome_arq_bin, "r+b");

    if(arq == NULL)
    {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    HEADER_REG head = innit_header_reg();
    DATA_REG reg = innit_data_reg();

    Ler_HEADER_bin(arq, &head);

    int nro = 0;

    int (*par)[2] = malloc(head.proxRRN * sizeof(*par));

    if (par == NULL)
    {
        printf("Falha na alocacao de memoria.\n");
        fclose(arq);
        return;
    }

    int status_leitura = 0;

    while( (status_leitura = Ler_DATA_bin(arq, &reg)) != 1)
    {
        if( status_leitura == -1)
        {
            continue;
        }

        int encontrou = 0;

        for(int j = 0; j < nro; j++)
        {
            if(reg.idPoPs == par[j][0] && reg.idPopsConectado == par[j][1])
            {
                encontrou = 1;
                break;
            }
        }

        if(!encontrou)
        {
            par[nro][0] = reg.idPoPs;
            par[nro][1] = reg.idPopsConectado;
            nro++;
        }
    }

    head.nroPares = nro;

    fseek(arq, 0, SEEK_SET);
    Escrever_HEADER_bin(arq, &head);

    fclose(arq);

    free(par);
}


