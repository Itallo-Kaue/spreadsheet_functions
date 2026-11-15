/*
    utilPoPs.h
    Rotinas auxiliares das funcionalidades dos PoPs: impressão de um registro.
*/

#ifndef UTILPOPS_H
#define UTILPOPS_H
#include "registroPoPs.h"

/* Imprime os campos de dados do registro em uma linha, com NULO nos campos
   nulos. Retorna 0, sem imprimir nada, se o registro estiver removido. */

int ImprimirRegistroPoPs(RegistroPoPs *r);
#endif
