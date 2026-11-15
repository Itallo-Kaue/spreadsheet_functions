/*
    funcionalidade2.c
    Listagem sequencial de todos os registros não removidos do arquivo de
    dados. Corresponde ao comando SELECT sem a cláusula WHERE.
*/

#include <stdio.h>
#include "funcionalidades.h"
#include "registro.h"
#include "util.h"

/* Entrada: o nome do arquivo de dados. Percorre o arquivo sequencialmente, do
   primeiro registro até o fim, e imprime os que não estão removidos. O
   arquivo é aberto apenas para leitura. */

void Funcionalidade_2() {
    char nomeArquivo[TAM_NOME_ARQUIVO];
    if (scanf("%63s", nomeArquivo) != 1) return;
    Cabecalho cab;
    FILE *bin = abrirArquivoBinarioLeitura(nomeArquivo, &cab);
    if (bin == NULL) {
        printf(MSG_FALHA);
        return;
    }

    /* O cabeçalho acabou de ser lido por inteiro na abertura do arquivo, de
       modo que o ponteiro já está no byte 17, início do primeiro registro:
       não é preciso reposicioná-lo. */

    Registro reg;
    int impressos = 0;
    while (lerRegistro(bin, &reg)) {
        impressos += ImprimirRegistros(&reg);
    }
    if (impressos == 0) printf(MSG_INEXISTENTE);
    fclose(bin);
}
