/*
    funcionalidade1.c
    Criação do arquivo de dados a partir do arquivo CSV de entrada.
    Corresponde ao comando CREATE TABLE da linguagem SQL.
*/

#include <stdio.h>

#include "funcionalidades.h"
#include "csv.h"
#include "fornecidas.h"

/* Entrada: o nome do arquivo CSV de origem e o nome do arquivo binário a ser
   criado. Todo o trabalho é feito a partir da função criarTabela, imprimindo
   uma mensagem de erro se não for possível criar a tabela. */

void Funcionalidade_1() {
    char nomeCSV[TAM_NOME_ARQUIVO];
    char nomeBin[TAM_NOME_ARQUIVO];

    if (scanf("%63s %63s", nomeCSV, nomeBin) != 2) return;

    if (!criarTabela(nomeCSV, nomeBin)) {
        printf(MSG_FALHA);
        return;
    }

    BinarioNaTela(nomeBin);
}
