/*
    funcionalidade5.c
    Remoção lógica dos registros que satisfazem um critério de busca, com
    reaproveitamento dinâmico de espaço por meio da pilha de registros
    removidos. Corresponde ao comando DELETE da linguagem SQL.
*/

#include <stdio.h>
#include <stdlib.h>
#include "funcionalidades.h"
#include "busca.h"
#include "fornecidas.h"
#include "pilha.h"
#include "registro.h"



void Funcionalidade_5() {
    char nomeArquivo[TAM_NOME_ARQUIVO];
    int n;
    if (scanf("%63s %d", nomeArquivo, &n) != 2) return;
    Cabecalho cab;
    FILE *bin = abrirArquivoBinario(nomeArquivo, &cab);
    if (bin == NULL) {
        printf(MSG_FALHA);
        return;
    }
    for (int i = 0; i < n; i++) {
        int m;
        if (scanf("%d", &m) != 1) break;
        Criterio *crit = lerCriterios(m);
        if (crit == NULL) continue;
        Registro reg;
        int rrn;
        iniciarBusca(bin);
        while (buscarProximo(bin, crit, m, &reg, &rrn)) {
            preencherComLixo(&reg);
            escreverRegistroPorRRN(bin, &reg, rrn);
            Empilhar(bin, &cab, rrn);


            fseek(bin, byteOffset(rrn + 1), SEEK_SET);
        }
        free(crit);
    }

    fecharArquivoBinario(bin, &cab);
    BinarioNaTela(nomeArquivo);
}
