/*
    funcionalidade9.c
    Criação do arquivo de índice primário sobre o campo idPoPs a partir do
    arquivo de dados. Corresponde ao comando CREATE INDEX da linguagem SQL.
*/

#include <stdio.h>
#include "funcionalidades.h"
#include "registroPoPs.h"
#include "indicePrimario.h"
#include "fornecidas.h"


void funcionalidade9() {
    char arqBin[TAM_NOME_ARQUIVO], arqIdx[TAM_NOME_ARQUIVO];
    if (scanf("%63s %63s", arqBin, arqIdx) != 2) return;
    FILE *f = fopen(arqBin, "rb");
    if(!f) { printf(MSG_FALHA); return; }
    CabecalhoPoPs cab = lerCabecalhoPoPs(f);
    if(cab.status == '0') { printf(MSG_FALHA); fclose(f); return; }
    
    IndiceArray *idx = criarIndiceDeDados(f, cab.proxByteOffset);
    ordenarIndice(idx);
    gravarIndice(idx, arqIdx);
    
    liberarIndice(idx); fclose(f);
    binarioNaTela(arqIdx);
}
