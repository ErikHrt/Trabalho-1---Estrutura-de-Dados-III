#ifndef REGISTRO_H_INCLUDED
#define REGISTRO_H_INCLUDED

#include <stdio.h>

typedef struct
{
    char status;
    int topoPilha;
    int proxRRN;
    int nroRegRem;
    int nroPares;

}HEADER_REG;

#define INCONSISTENTE '0'
#define CONSISTENTE '1'

#define sizeofHEADER 17

typedef struct
{
    char removido;
    int encadeamentoPilha;

    int idPoPs;
    int idPopsConectado;
    int velocidade;
    char unidadeMedida;

}DATA_REG;

#define REMOVIDO '1'
#define SALVO '0'
#define LIXO '$'

#define NULO -1

#define sizeofDATA 18

typedef struct
{
    int RRN;
    DATA_REG r;

    int usa_idpops;
    int usa_idconecta;
    int usa_velocidade;
    int usa_un_medida;
}ASSIST_REG;

#define SIM 1
#define NAO 0

#define Tamanho_STRING 100


int Ler_registro_csv(FILE* arq_entrada ,DATA_REG* reg);

HEADER_REG innit_header_reg();
void Escrever_HEADER_bin(FILE* arq, HEADER_REG* hreg);
int Ler_HEADER_bin(FILE* arq, HEADER_REG* hreg);

DATA_REG innit_data_reg();
void Escrever_DATA_bin(FILE* arq,DATA_REG* reg);
int Ler_DATA_bin(FILE* arq, DATA_REG* reg);

void Ler_NOVO_DATA_REG(DATA_REG* reg);

void Atualizar_DATA_bin(FILE *arq, int RRN, ASSIST_REG *r_atualizacoes, ASSIST_REG *r_copia);
void Remover_DATA_bin(FILE *arq, HEADER_REG *head, int RRN);
void Printar_DATA_REG(DATA_REG* reg);


int Busca_Sequencial(FILE *arq, ASSIST_REG *r_ref, ASSIST_REG *r_copia);


void nroPares(char *nome_arq_bin);

#endif // REGISTRO_H_INCLUDED
