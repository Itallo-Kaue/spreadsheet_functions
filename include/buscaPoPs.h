/*
    buscaPoPs.h
    Leitura dos critérios de busca da entrada padrão e varredura sequencial do
    arquivo de dados dos PoPs.
*/

#ifndef BUSCAPOPS_H
#define BUSCAPOPS_H
#include "registroPoPs.h"
/* Par "nomeCampo valor" lido da entrada. Um valor vazio representa NULO. */
typedef struct {
    char nomeCampo[50];
    char valor[100];
} CriterioPoPs;

/* Lê da entrada o valor de um campo, entre aspas para os campos de texto. */

void LerValorDoCampoPoPs(const char *nomeCampo, char *destino);

/* Lê n pares "nomeCampo valor" e devolve o vetor de critérios. */

CriterioPoPs *lerCriteriosPoPs(int n);

/* Retorna 1 se o registro não estiver removido e satisfizer os n critérios. */

int registroSatisfazCriteriosPoPs(RegistroPoPs *reg, CriterioPoPs *crit, int n);

/* Altera no registro em memória o campo indicado pelo critério. */

void AtualizarCampoPoPs(RegistroPoPs *reg, CriterioPoPs *c);

/* Posiciona o arquivo no primeiro registro, logo após o cabeçalho. */

void iniciarBuscaPoPs(FILE *bin);

/* Devolve em reg o próximo registro que satisfaz os critérios, com o seu
   byteOffset em offset_atual. Retorna 0 ao chegar no limite. */

int buscarProximoPoPs(FILE *bin, CriterioPoPs *crit, int n, RegistroPoPs *reg, int64_t *offset_atual, int64_t limite);
#endif
