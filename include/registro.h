/*
    registro.h
    Definição das estruturas do registro de cabeçalho e dos registros de dados
    do arquivo CSV, além das operações de leitura/escrita campo a campo.
*/
#ifndef REGISTRO_H
#define REGISTRO_H
#include <stdio.h>

#define STATUS_CONSISTENTE '1'
#define STATUS_INCONSISTENTE '0'
#define TAM_CABECALHO 17
#define TAM_REGISTRO 18
#define REMOVIDO '1'
#define NAO_REMOVIDO '0'
#define LIXO '$'
#define INT_NULO -1
#define TAM_VALOR 64
typedef struct {
    char status;    /* '0' inconsistente, '1' consistente          - 1 byte  */
    int topoPilha;  /* RRN do topo da pilha de removidos, ou -1    - 4 bytes */
    int proxRRN;    /* Próximo RRN disponível                      - 4 bytes. */
    int nroRegRem;  /* Número de registros logicamente removidos   - 4 bytes. */
    int nroPares;   /* Qtd de pares (idPoPs, idPoPsConectado)      - 4 bytes. */
} Cabecalho;

/* Registro de dados (18 bytes, nesta ordem) */
typedef struct {
    char removido;          /* '0' ou '1'                          - 1 byte  */
    int encadeamentoPilha;  /* RRN do próximo removido, ou -1      - 4 bytes */
    int idPoPs;             /* Não pode ser nulo                   - 4 bytes. */
    int idPoPsConectado;    /* Não pode ser nulo                   - 4 bytes. */
    int velocidade;         /* -1 quando nulo                      - 4 bytes */
    char unidadeMedida;     /* '$' quando nulo                     - 1 byte  */
} Registro;

/* --- Cabeçalho --- */

/* Preenche o cabeçalho com os valores iniciais de um arquivo recém-criado. */

void inicializarCabecalho(Cabecalho *cab);

/* Escreve o cabeçalho campo a campo na posição 0 do arquivo. */

void escreverCabecalho(FILE *bin, Cabecalho *cab);

/* Lê o cabeçalho. Retorna 0 se o arquivo estiver inconsistente. */

int lerCabecalho(FILE *bin, Cabecalho *cab);

/* --- Registros de dados --- */

/* Escreve o registro campo a campo na posição corrente do arquivo. */

void escreverRegistro(FILE *bin, Registro *reg);

/* Escreve o registro campo a campo no byte offset correspondente ao RRN. */

void escreverRegistroPorRRN(FILE *bin, Registro *reg, int rrn);

/* Lê o registro da posição atual (se estiver removido, só pula).
   Retorna 0 no fim do arquivo. */

int lerRegistro(FILE *bin, Registro *reg);

/* Lê o registro de RRN informado, usando acesso direto.
   Retorna 1 em caso de sucesso e 0 se o RRN não existir no arquivo. */

int lerRegistroPorRRN(FILE *bin, Registro *reg, int rrn);

/* Sobrescreve os campos de dados do registro com lixo '$', preservando os
   campos de controle. */
void preencherComLixo(Registro *reg);

/* Calcula o byte offset onde começa o registro de RRN informado. */

long byteOffset(int rrn);

/* --- Abertura e fechamento do arquivo de dados --- */

/* Cria o arquivo de dados com o cabeçalho inicial (status '0'). NULL se falhar. */

FILE *criarArquivoBinario(const char *nomeArquivo, Cabecalho *cab);

/* Abre um arquivo existente para leitura e escrita, lê o cabeçalho e marca o
   status como '0' (inconsistente). Retorna NULL em caso de falha ou se o
   arquivo estiver inconsistente. */

FILE *abrirArquivoBinario(const char *nomeArquivo, Cabecalho *cab);

/* Abre um arquivo existente apenas para leitura e lê o cabeçalho.
   Retorna NULL em caso de falha ou se o arquivo estiver inconsistente. */

FILE *abrirArquivoBinarioLeitura(const char *nomeArquivo, Cabecalho *cab);

/* Regrava o cabeçalho com status '1' (consistente) e fecha o arquivo. */

void fecharArquivoBinario(FILE *bin, Cabecalho *cab);

#endif
