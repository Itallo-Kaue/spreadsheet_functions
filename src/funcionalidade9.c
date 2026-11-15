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

/* Entrada: o nome do arquivo de dados e o nome do arquivo de índice a ser
   criado. O arquivo de dados é percorrido uma única vez para montar, em
   memória, o vetor de pares (idPoPs, byteOffset) dos registros não removidos;
   em seguida o vetor é ordenado pelo idPoPs e gravado no arquivo de índice. */

void funcionalidade9() {
    char arqBin[TAM_NOME_ARQUIVO], arqIdx[TAM_NOME_ARQUIVO];
    if (scanf("%63s %63s", arqBin, arqIdx) != 2) return;
    FILE *f = fopen(arqBin, "rb");
    if(!f) { printf(MSG_FALHA); return; }
    /* O arquivo só é aceito se estiver consistente ('1'). Como ele é aberto
       apenas para leitura, o status não precisa ser alterado. */
    CabecalhoPoPs cab = lerCabecalhoPoPs(f);
    if(cab.status == '0') { printf(MSG_FALHA); fclose(f); return; }
    
    /* A varredura vai do fim do cabeçalho até proxByteOffset, que marca o fim
       da área ocupada do arquivo. */
    IndiceArray *idx = criarIndiceDeDados(f, cab.proxByteOffset);
    ordenarIndice(idx);
    gravarIndice(idx, arqIdx);
    
    liberarIndice(idx); fclose(f);
    binarioNaTela(arqIdx);
}
