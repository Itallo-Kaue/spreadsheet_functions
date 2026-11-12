/*
    funcionalidade7.c
    Atualização dos registros que satisfazem um critério de busca.
    Corresponde ao comando UPDATE da linguagem SQL.
*/

#include <stdio.h>
#include <stdlib.h>

#include "funcionalidades.h"
#include "busca.h"
#include "fornecidas.h"
#include "registro.h"

/* Entrada: o nome do arquivo de dados e a quantidade n de atualizações;
   seguinte a isso, para cada atualização, duas linhas no formato "nomeCampo"
   e "valor": a primeira com os campos de busca e a segunda com os campos a
   serem alterados. Como os registros têm tamanho fixo, a alteração é feita no
   próprio RRN, sem remover e reinserir.

   A atualização é feita assim que a varredura encontra o registro, de modo
   que o arquivo é percorrido uma única vez por operação e cada registro é
   lido uma única vez. */

void Funcionalidade_7() {
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
        Criterio *busca = lerCriterios(m);

        int p;
        if (scanf("%d", &p) != 1) {
            free(busca);
            break;
        }
        Criterio *novos = lerCriterios(p);

        if (busca == NULL || novos == NULL) {
            free(busca);
            free(novos);
            continue;
        }

        Registro reg;
        int rrn;

        iniciarBusca(bin);

        while (buscarProximo(bin, busca, m, &reg, &rrn)) {
            for (int k = 0; k < p; k++) AtualizarCampo(&reg, &novos[k]);

            escreverRegistroPorRRN(bin, &reg, rrn);

            /* A escrita acima moveu o ponteiro do arquivo, então a varredura
               é reposicionada no registro seguinte antes de continuar. */

            fseek(bin, byteOffset(rrn + 1), SEEK_SET);
        }

        free(novos);
        free(busca);
    }

    fecharArquivoBinario(bin, &cab);

    BinarioNaTela(nomeArquivo);
}
