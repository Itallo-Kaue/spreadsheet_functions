/*
    funcionalidade3.c
    Recuperação dos registros que satisfazem um critério de busca, por meio de
    varredura sequencial do arquivo de dados. Corresponde ao comando SELECT
    com a cláusula WHERE, sem o apoio de um índice.
*/

#include <stdio.h>
#include <stdlib.h>
#include "funcionalidades.h"
#include "busca.h"
#include "registro.h"
#include "util.h"

void Funcionalidade_3() {
    char nomeArquivo[TAM_NOME_ARQUIVO];
    int n;
    if (scanf("%63s %d", nomeArquivo, &n) != 2) return;
    Cabecalho cab;
    FILE *bin = abrirArquivoBinarioLeitura(nomeArquivo, &cab);
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
        int encontrados = 0;
        iniciarBusca(bin);
        while (buscarProximo(bin, crit, m, &reg, &rrn)) {
            ImprimirRegistros(&reg);
            encontrados++;
        }
        if (encontrados == 0) printf(MSG_INEXISTENTE);
        printf("\n");
        free(crit);
    }
    fclose(bin);
}
