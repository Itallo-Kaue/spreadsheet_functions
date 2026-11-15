/*
    registroPoPs.h
    Definição das estruturas do registro de cabeçalho e dos registros de dados
    de tamanho variável do arquivo dos PoPs, além das operações de
    leitura/escrita campo a campo.
*/

#ifndef REGISTROPOPS_H
#define REGISTROPOPS_H
#include <stdint.h>
#include <stdio.h>
/* Caractere usado para preencher os campos nulos e o espaço que sobra. */
#define LIXO '$'
#define REMOVIDO '1'
#define NAO_REMOVIDO '0'

/* Registro de cabeçalho (25 bytes, nesta ordem) */
typedef struct {
    char status;            /* '0' inconsistente, '1' consistente      - 1 byte  */
    int64_t topoLista;      /* O byteOffset do 1º removido, ou -1        - 8 bytes. */
    int64_t proxByteOffset; /* Próximo byte disponível no fim          - 8 bytes. */
    int nroRegRem;          /* Número de registros removidos           - 4 bytes. */
    int nroPoPs;            /* Número de PoPs não removidos            - 4 bytes. */
} CabecalhoPoPs;

/* Registro de dados: 19 bytes de parte fixa, seguidos das duas strings, cada
   uma terminada por '|', e de lixo ('$') até completar tamanhoRegistro. */
typedef struct {
    char removido;              /* '0' ou '1'                      - 1 byte  */
    int tamanhoRegistro;        /* Bytes ocupados pelo registro    - 4 bytes. */
    int64_t encadeamentoLista;  /* O byteOffset do próximo removido  - 8 bytes. */
    int idPoPs;                 /* Chave primária, não nula        - 4 bytes. */
    char siglaPais[2];          /* '$$' quando nula                - 2 bytes */
    char *nomePoPs;             /* NULL quando nulo        - tamanho variável */
    char *nomePais;             /* NULL quando nulo        - tamanho variável */
} RegistroPoPs;

/* Escreve o cabeçalho campo a campo na posição 0 do arquivo. */

void escreverCabecalhoPoPs(FILE *f, CabecalhoPoPs *c);

/* Lê o cabeçalho a partir da posição 0. Se o status for '0', o restante não é
   lido. */

CabecalhoPoPs lerCabecalhoPoPs(FILE *f);

/* Calcula quantos bytes o registro ocupa no arquivo, sem contar lixo. */

int tamanhoDoRegistroPoPs(RegistroPoPs *r);

/* Lê o registro da posição atual. Em caso de falha, removido vale '2'. */

RegistroPoPs lerRegistroPoPs(FILE *f);

/* Escreve o registro na posição atual, completando com lixo até
   tamanhoRegistro. */

void escreverRegistroPoPs(FILE *f, RegistroPoPs *r);

/* Libera as strings alocadas do registro. */

void liberarRegistroPoPs(RegistroPoPs *r);

/* Lê o registro que está no byteOffset informado. */

RegistroPoPs lerRegistroPorOffset(FILE *f, int64_t offset);

/* Escreve o registro no byteOffset informado. */

void escreverRegistroPorOffset(FILE *f, RegistroPoPs *r, int64_t offset);

/* Preenche os campos de dados com lixo ('$'), como na remoção lógica. */

void preencherComLixoRegistroPoPs(RegistroPoPs *r);

/* Cópia alocada de uma string; substitui a strdup, que não é C99. */

char *meu_strdup(const char *s);
#endif
