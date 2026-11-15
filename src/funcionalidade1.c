/*
    funcionalidade1.c
    Criação do arquivo de dados a partir do arquivo CSV de entrada.
    Corresponde ao comando CREATE TABLE da linguagem SQL.
*/

#include <stdio.h>
#include "funcionalidades.h"
#include "csv.h"
#include "fornecidas.h"


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
