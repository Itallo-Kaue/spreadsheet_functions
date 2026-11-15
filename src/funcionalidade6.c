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

        reg.removido = NAO_REMOVIDO;
        reg.encadeamentoPilha = -1;

        /* Nulo: -1 no inteiro e '$' na unidade. */

        LerValorNumerico(valor); reg.idPoPs = atoi(valor);
        LerValorNumerico(valor); reg.idPoPsConectado = atoi(valor);
        LerValorNumerico(valor); reg.velocidade = (valor[0] != '\0') ? atoi(valor) : INT_NULO;
        ScanQuoteString(valor);  reg.unidadeMedida = (valor[0] != '\0') ? valor[0] : LIXO;

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
