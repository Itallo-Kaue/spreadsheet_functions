/*
    funcionalidade6.c
    Inserção de novos registros no arquivo de dados, com reaproveitamento
    dinâmico do espaço dos registros logicamente removidos. Corresponde ao
    comando INSERT INTO da linguagem SQL.
*/

#include <stdio.h>
#include <stdlib.h>

#include "funcionalidades.h"
#include "busca.h"
#include "fornecidas.h"
#include "pilha.h"
#include "registro.h"

/* Entrada: o nome do arquivo de dados e a quantidade n de inserções. Logo em
   seguida, é feito uma linha por registro com os quatro campos na seguinte
   ordem: idPoPs, idPoPsConectado, velocidade e unidadeMedida (usando NULO para
   os campos nulos). A inserção reaproveita o espaço dos registros removidos:
   só quando a pilha está vazia é que o registro vai para o fim do arquivo, no
   proxRRN. */

void Funcionalidade_6() {
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
        char valor[TAM_VALOR];
        Registro reg;

        /* Campos de controle: o registro entra ativo e fora da pilha. */

        reg.removido = NAO_REMOVIDO;
        reg.encadeamentoPilha = -1;

        /* Cada campo de dados recebe o valor lido da entrada. Os campos
           opcionais recebem o valor nulo (-1 para o inteiro e '$' para a
           unidade de medida) quando a entrada traz NULO. */

        LerValorNumerico(valor);
        reg.idPoPs = atoi(valor);

        LerValorNumerico(valor);
        reg.idPoPsConectado = atoi(valor);

        LerValorNumerico(valor);
        reg.velocidade = (valor[0] != '\0') ? atoi(valor) : INT_NULO;

        ScanQuoteString(valor);
        reg.unidadeMedida = (valor[0] != '\0') ? valor[0] : LIXO;

        /* Tenta reusar o topo da pilha de removidos; se não houver nenhum,
           o registro é acrescentado no fim do arquivo. */

        int rrn = Desempilhar(bin, &cab);
        if (rrn == -1) {
            rrn = cab.proxRRN;
            cab.proxRRN++;
        }

        escreverRegistroPorRRN(bin, &reg, rrn);
    }

    fecharArquivoBinario(bin, &cab);

    BinarioNaTela(nomeArquivo);
}
