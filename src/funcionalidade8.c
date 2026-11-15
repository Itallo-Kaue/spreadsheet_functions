/*
    funcionalidade8.c
    Criação do arquivo de dados dos PoPs a partir do arquivo CSV de entrada.
    Corresponde ao comando CREATE TABLE da linguagem SQL.
*/

#include <stdio.h>
#include "funcionalidades.h"
#include "csvPoPs.h"
#include "fornecidas.h"

/* Entrada: o nome do arquivo CSV de origem e o nome do arquivo binário a ser
   criado. Todo o trabalho é feito a partir da função criarTabelaPoPs, que
   grava os registros de tamanho variável um atrás do outro, imprimindo uma
   mensagem de erro se não for possível criar a tabela. */

void funcionalidade8() {
    char arqIn[TAM_NOME_ARQUIVO], arqOut[TAM_NOME_ARQUIVO];
    if (scanf("%63s %63s", arqIn, arqOut) != 2) return;
    if(criarTabelaPoPs(arqIn, arqOut)) {
        binarioNaTela(arqOut);
    } else {
        printf(MSG_FALHA);
    }
}
