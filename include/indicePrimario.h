/*
    indicePrimario.h
    Índice primário sobre o campo idPoPs, mantido em memória como um vetor
    ordenado de pares (idPoPs, byteOffset).
*/

#ifndef INDICEPRIMARIO_H
#define INDICEPRIMARIO_H
#include <stdint.h>
#include <stdio.h>

/* Entrada do índice (12 bytes no arquivo) */
typedef struct {
    int idPoPs;         /* chave de busca                          - 4 bytes */
    int64_t byteOffset; /* posição do registro no arquivo de dados - 8 bytes */
} IndiceEntry;

/* Vetor dinâmico com as entradas do índice, ordenado pelo idPoPs. */
typedef struct {
    IndiceEntry *entradas;  /* vetor de entradas                 */
    int tamanho;            /* quantidade de entradas usadas     */
    int capacidade;         /* quantidade de entradas alocadas   */
} IndiceArray;

/* Monta o índice a partir dos registros não removidos do arquivo de dados. */

IndiceArray* criarIndiceDeDados(FILE *f, int64_t limite);

/* Ordena as entradas pelo idPoPs. */

void ordenarIndice(IndiceArray *indice);

/* Grava o índice em disco, com status no primeiro byte. */

void gravarIndice(IndiceArray *indice, const char *arqIdx);

/* Carrega o índice do disco; retorna NULL se não existir ou estiver
   inconsistente. */

IndiceArray* carregarIndice(const char *arqIdx);

/* Libera a memória do índice. */

void liberarIndice(IndiceArray *indice);

/* Busca binária pelo idPoPs; retorna 1 e o byteOffset em outOffset se
   encontrar. */

int buscarNoIndice(IndiceArray *indice, int idBusca, int64_t *outOffset);

/* Insere um par (idPoPs, offset) mantendo o índice ordenado. */

void inserirNoIndice(IndiceArray *indice, int idPoPs, int64_t offset);

/* Remove a entrada do idPoPs informado, se existir. */

void removerDoIndice(IndiceArray *indice, int idPoPs);
#endif
