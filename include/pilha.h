/*
    pilha.h
    Manipulação da pilha de registros logicamente removidos. A pilha não fica
    em memória: o topo é o campo topoPilha do cabeçalho e o encadeamento é o
    campo encadeamentoPilha de cada registro removido, ou seja, ela é uma lista
    encadeada gravada dentro do próprio arquivo de dados.
*/

#ifndef PILHA_H
#define PILHA_H

#include <stdio.h>

#include "registro.h"

/* Empilha o registro do RRN (os campos de dados já devem estar com lixo). */

void Empilhar(FILE *bin, Cabecalho *cab, int rrn);

/* Desempilha e retorna o RRN do topo, ou -1 se a pilha estiver vazia. */
   
int Desempilhar(FILE *bin, Cabecalho *cab);

#endif