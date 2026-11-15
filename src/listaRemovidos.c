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

/* Insere o registro removido r, que está no byteOffset offset, na posição
   certa da lista, percorrendo-a até achar o primeiro registro de tamanho menor
   ou igual ao dele. O registro é regravado já apontando para o seu sucessor, o
   antecessor (ou o topo, no cabeçalho) passa a apontar para ele e o contador
   de removidos é atualizado. */

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
    /* Neste ponto, prevOff é o registro que deve ficar antes de r (ou -1, se r
       vira o topo) e currOff é o que deve ficar depois dele. */
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

/* Tenta retirar da lista um espaço para um registro de tamanhoNecessario
   bytes. Como a lista está em ordem decrescente, basta olhar o topo: se ele
   couber, sai da lista, o cabeçalho é atualizado e a função devolve o seu
   byteOffset, com o tamanho do espaço em tamanhoRecuperado. Se nem o maior
   removido couber, devolve -1. */

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
