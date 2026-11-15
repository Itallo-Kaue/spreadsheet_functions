/*
    pilha.c
    Implementação da pilha de registros removidos. Todas as operações mexem
    apenas nos campos de controle (removido e encadeamentoPilha) do registro e
    nos campos topoPilha e nroRegRem do cabeçalho; os campos de dados não são
    tocados aqui.
*/

#include "pilha.h"
#include "registro.h"

#include <stdio.h>
#include <stdlib.h>

/* O encadeamentoPilha fica logo depois do byte de removido. */

#define DESLOC_ENCADEAMENTO ((long)sizeof(char))

/* ------------------------------------------------------------------ 
    Inserção na pilha                                                  
   ------------------------------------------------------------------ */

/* O removido vira o novo topo. */

void Empilhar(FILE *bin, Cabecalho *cab, int rrn) {
    if (bin == NULL || rrn < 0) return;

    char removido = REMOVIDO;
    int encadeamento = cab->topoPilha;

    fseek(bin, byteOffset(rrn), SEEK_SET);
    fwrite(&removido, sizeof(char), 1, bin);
    fwrite(&encadeamento, sizeof(int), 1, bin);

    cab->topoPilha = rrn;
    cab->nroRegRem++;
}

/* ------------------------------------------------------------------
    Remoção da pilha                                                   
   ------------------------------------------------------------------ */

/* Tira o topo da pilha e retorna o RRN, ou -1 se estiver vazia. */

int Desempilhar(FILE *bin, Cabecalho *cab) {
    if (bin == NULL || cab->topoPilha == -1) return -1;

    int rrn = cab->topoPilha;
    int proximoTopo;

    /* Posiciona no campo encadeamentoPilha do registro do topo. */
    fseek(bin, byteOffset(rrn) + DESLOC_ENCADEAMENTO, SEEK_SET);
    if (fread(&proximoTopo, sizeof(int), 1, bin) != 1) return -1;

    cab->topoPilha = proximoTopo;
    cab->nroRegRem--;

    return rrn;
}
