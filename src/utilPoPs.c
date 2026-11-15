/*
    utilPoPs.c
    Rotinas auxiliares compartilhadas pelas funcionalidades dos PoPs:
    impressão de um registro de dados na saída padrão.
*/

#include <stdio.h>
#include "utilPoPs.h"

/* Imprime o registro numa linha, NULO nos campos nulos. Retorna 0 se for removido. */

int ImprimirRegistroPoPs(RegistroPoPs *r) {
    if(r->removido == REMOVIDO) return 0;
    printf("%d ", r->idPoPs);
    if(r->nomePoPs) printf("%s ", r->nomePoPs); else printf("NULO ");
    if(r->nomePais) printf("%s ", r->nomePais); else printf("NULO ");
    if(r->siglaPais[0] != LIXO) printf("%.2s\n", r->siglaPais); else printf("NULO\n");
    return 1;
}
