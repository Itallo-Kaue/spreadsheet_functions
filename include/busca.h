/*
    busca.h
    Leitura dos critérios de busca informados na entrada padrão e varredura do
    arquivo de dados em busca dos registros que satisfazem todos eles.
    Usado pelas funcionalidades de consulta, de remoção lógica e de atualização.
*/

#ifndef BUSCA_H
#define BUSCA_H

#include <stdio.h>

#include "registro.h"

#define TAM_NOME_CAMPO 32
#define TAM_VALOR      64

/* Par (nomeCampo, valor); o valor fica como texto e é convertido na comparação. */

typedef struct {
    char nomeCampo[32];
    char valor[100];
} Criterio;

/* Lê da entrada o valor de um campo numérico, convertendo NULO em string
   vazia. */

void LerValorNumerico(char *destino);

/* Lê da entrada o valor do campo informado, escolhendo entre a leitura
   numérica e a função fornecida ScanQuoteString conforme o nome do campo. */

void LerValorDoCampo(const char *nomeCampo, char *destino);

/* Lê n pares "nomeCampo valor" da entrada padrão e devolve um vetor alocado
   dinamicamente com eles. Retorna NULL em caso de falha de alocação. */

Criterio *lerCriterios(int n);

/* Retorna 1 se o registro satisfaz TODOS os n critérios e 0 caso contrário.
   Registros logicamente removidos nunca satisfazem a busca. */

int registroSatisfazCriterios(Registro *reg, Criterio *crit, int n);

/* Grava no registro, em memória, o valor de um critério, usando o nome do
   campo para saber onde escrever. Serve para a atualização de registros, que
   reaproveita o mesmo formato "nomeCampo valor" da busca.*/

void AtualizarCampo(Registro *reg, Criterio *c);

/* Posiciona no primeiro registro, logo após o cabeçalho. */

void iniciarBusca(FILE *bin);

/* Avança até o próximo registro que satisfaz os n critérios e devolve ele em *reg,
   com o RRN em *rrn. Retorna 0 no fim do arquivo. */

int buscarProximo(FILE *bin, Criterio *crit, int n, Registro *reg, int *rrn);

#endif