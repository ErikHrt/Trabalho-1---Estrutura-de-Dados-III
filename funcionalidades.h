#ifndef FUNCIONALIDADES_H_INCLUDED
#define FUNCIONALIDADES_H_INCLUDED

void CREATE_TABLE(char* nome_arq_entrada, char* nome_arq_saida);

void SELECT(char* nome_arq_bin);

void WHERE(char *nome_arq_bin, int n);

void ACCESS(char *nome_arq_bin,int RRN);

void DELETE(char *nome_arq_bin, int n);

void INSERT(char *nome_arq_bin, int n);

void UPDATE(char* nome_arq_bin, int n);


#endif // FUNCIONALIDADES_H_INCLUDED
