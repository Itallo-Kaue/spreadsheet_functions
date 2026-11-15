/*
    funcionalidade8.c
    Criação do arquivo de dados dos PoPs a partir do arquivo CSV de entrada.
    Corresponde ao comando CREATE TABLE da linguagem SQL.
*/

#include <stdio.h>
#include "funcionalidades.h"
#include "csvPoPs.h"
#include "fornecidas.h"

/* Entrada: csv e binário. */

void funcionalidade8() {
    char arqIn[TAM_NOME_ARQUIVO], arqOut[TAM_NOME_ARQUIVO];
    if (scanf("%63s %63s", arqIn, arqOut) != 2) return;
    if(criarTabelaPoPs(arqIn, arqOut)) {
        binarioNaTela(arqOut);
    } else {
        printf(MSG_FALHA);
    }
}
