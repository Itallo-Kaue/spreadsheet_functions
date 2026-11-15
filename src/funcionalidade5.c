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

/* Entrada: o nome do arquivo de dados e a quantidade n de operações de
   remoção. Logo após pegar essas entradas, é lido, para cada busca, a
   quantidade m de campos seguida dos m pares do "nomeCampo" e do "valor".
   Todo registro que satisfizer todos os campos de uma operação é marcado como
   removido, tem seus campos de dados preenchidos com lixo e passa a ser o topo
   da pilha de registros removidos.

   A remoção é feita assim que a varredura encontra o registro, de modo que o
   arquivo é percorrido uma única vez por operação e cada registro é lido uma
   única vez. */

void Funcionalidade_5() {
    char nomeArquivo[TAM_NOME_ARQUIVO];
    int n;
    if (scanf("%63s %d", nomeArquivo, &n) != 2) return;
    /* Abre o arquivo para leitura e escrita; o status vai para '0' enquanto
       ele estiver em uso e volta para '1' no fechamento. */
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
            /* Os campos de dados são sobrescritos com lixo e regravados; em
               seguida a pilha cuida dos campos de controle, colocando o
               registro no topo e atualizando o cabeçalho. */
            preencherComLixo(&reg);
            escreverRegistroPorRRN(bin, &reg, rrn);
            Empilhar(bin, &cab, rrn);

            /* As duas escritas acima moveram o ponteiro do arquivo, então a
               varredura é reposicionada no registro seguinte ao removido
               antes de continuar. */

            fseek(bin, byteOffset(rrn + 1), SEEK_SET);
        }
        free(crit);
    }

    /* Regrava o cabeçalho atualizado, marca o arquivo como consistente ('1')
       e o fecha antes de imprimir o resultado. */

    fecharArquivoBinario(bin, &cab);
    BinarioNaTela(nomeArquivo);
}
