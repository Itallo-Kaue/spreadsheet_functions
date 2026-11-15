/*
    listaRemovidos.c
    Implementação da lista encadeada de registros logicamente removidos, usada
    no reaproveitamento dinâmico de espaço. A lista fica no próprio arquivo de
    dados: o cabeçalho guarda o byteOffset do primeiro removido (topoLista) e
    cada removido guarda o byteOffset do próximo em encadeamentoLista. Ela é
    mantida em ordem decrescente de tamanho, de modo que o topo é sempre o
    maior espaço disponível (estratégia worst fit).
*/

#include <stdio.h>
#include "listaRemovidos.h"

/* Insere o removido na lista mantendo a ordem decrescente de tamanho. */

void inserirNaLista(FILE *f, CabecalhoPoPs *c, RegistroPoPs *r, int64_t offset) {
    int64_t currOff = c->topoLista;
    int64_t prevOff = -1;
    while(currOff != -1) {
        fseek(f, currOff, SEEK_SET);
        RegistroPoPs temp = lerRegistroPoPs(f);
        if(r->tamanhoRegistro >= temp.tamanhoRegistro) break;
        prevOff = currOff;
        currOff = temp.encadeamentoLista;
    }
    /* O prevOff fica antes de r e currOff depois (prevOff -1 = r vira o topo). */
    r->encadeamentoLista = currOff;
    escreverRegistroPorOffset(f, r, offset);
    if(prevOff == -1) c->topoLista = offset;
    else {
        fseek(f, prevOff + 5, SEEK_SET);
        fwrite(&offset, 8, 1, f);
    }
    c->nroRegRem++;
    escreverCabecalhoPoPs(f, c);
}

/* Worst fit: se couber no topo, tira da lista e devolve o offset; senão -1. */

int64_t retirarMaior(FILE *f, CabecalhoPoPs *c, int tamanhoNecessario, int *tamanhoRecuperado) {
    if(c->topoLista == -1) return -1;
    fseek(f, c->topoLista, SEEK_SET);
    RegistroPoPs topo = lerRegistroPoPs(f);
    if(topo.tamanhoRegistro >= tamanhoNecessario) {
        int64_t offsetRetornado = c->topoLista;
        c->topoLista = topo.encadeamentoLista;
        c->nroRegRem--;
        escreverCabecalhoPoPs(f, c);
        *tamanhoRecuperado = topo.tamanhoRegistro;
        return offsetRetornado;
    }
    return -1;
}
