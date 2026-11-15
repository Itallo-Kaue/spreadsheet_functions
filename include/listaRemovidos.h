/*
    listaRemovidos.h
    Lista de registros removidos, mantida no próprio arquivo de dados em ordem
    decrescente de tamanho (estratégia worst fit).
*/

#ifndef LISTAREMOVIDOS_H
#define LISTAREMOVIDOS_H
#include "registroPoPs.h"

/* Insere o registro removido que está em offset na posição certa da lista e
   atualiza o cabeçalho. */

void inserirNaLista(FILE *f, CabecalhoPoPs *c, RegistroPoPs *r, int64_t offset);

/* Retira o topo da lista se o registro couber nele e devolve o seu byteOffset,
   ou -1 se não couber. */

int64_t retirarMaior(FILE *f, CabecalhoPoPs *c, int tamanhoNecessario, int *tamanhoRecuperado);
#endif
