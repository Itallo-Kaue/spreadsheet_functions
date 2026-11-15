/*
    csvCampos.c
    Rotinas auxiliares para separar uma linha do CSV em campos e reconhecer
    campos nulos. São usadas tanto na carga do arquivo da funcionalidade 1
    quanto na da funcionalidade 8.
*/

#include "csvCampos.h"
#include <ctype.h>
#include <string.h>

/* Remove as quebras de linha ('\n' e '\r') do fim do campo, alterando a
   própria string, e devolve o mesmo ponteiro. */

char *removerQuebraDeLinha(char *campo) {
    size_t fim = strlen(campo);
    while (fim > 0 && (campo[fim - 1] == '\n' || campo[fim - 1] == '\r')) {
        campo[fim - 1] = '\0';
        fim--;
    }
    return campo;
}

/* Separa a linha em até maxCampos campos, usando ',' ou ';' como delimitador.
   A própria linha é alterada: cada delimitador vira '\0' e campos[i] aponta
   para o início do i-ésimo campo. Os campos que faltarem ficam NULL. Retorna a
   quantidade de campos encontrados. */

int separarCampos(char *linha, char *campos[], int maxCampos) {
    int qtd = 0;
    char *inicio = linha;
    char *p = linha;
    while (qtd < maxCampos) {
        if (*p == ',' || *p == ';' || *p == '\0') {
            int ultimo = (*p == '\0');
            *p = '\0';
            campos[qtd] = removerQuebraDeLinha(inicio);
            qtd++;
            if (ultimo) break;
            inicio = p + 1;
        }
        p++;
    }
    for (int i = qtd; i < maxCampos; i++) campos[i] = NULL;
    return qtd;
}

/* Retorna 1 se o campo for NULL, vazio ou formado apenas por espaços, ou seja,
   se ele representa um valor nulo no CSV. */

int campoNulo(const char *campo) {
    if (campo == NULL || campo[0] == '\0') return 1;
    for (int i = 0; campo[i] != '\0'; i++) {
        if (!isspace((unsigned char)campo[i])) {
            return 0;
        }
    }
    return 1;
}