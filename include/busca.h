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

/* Um par (nomeCampo, valor) informado na entrada. O valor é guardado como
   texto porque os campos têm tipos diferentes (inteiro ou caractere); a
   conversão é feita na hora da comparação, de acordo com o nome do campo. */

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

/* Posiciona o arquivo no primeiro registro, logo após o cabeçalho, para
   iniciar uma varredura sequencial. Deve ser chamada uma vez antes da
   primeira chamada de buscarProximo de cada busca. */

void iniciarBusca(FILE *bin);

/* Avança a varredura sequencial até o próximo registro que satisfaz todos os
   n critérios e o devolve em *reg, com o seu RRN em *rrn.
   Retorna 1 quando encontra um registro e 0 ao chegar ao fim do arquivo.

   A varredura é retomada do ponto em que parou, de modo que quem chama trata
   cada registro assim que ele é encontrado (imprimindo, removendo ou
   atualizando). Com isso o arquivo é percorrido uma única vez por busca e
   cada registro é lido uma única vez, sem a necessidade de guardar os RRNs
   encontrados para relê-los depois. */

int buscarProximo(FILE *bin, Criterio *crit, int n, Registro *reg, int *rrn);

#endif