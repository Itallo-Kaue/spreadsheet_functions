/*
    csvCampos.h
    Rotinas auxiliares para separar uma linha do CSV em campos e reconhecer
    campos nulos.
*/

#ifndef CSVCAMPOS_H
#define CSVCAMPOS_H

/* Remove as quebras de linha do fim do campo. */

char *removerQuebraDeLinha(char *campo);

/* Separa a linha em até maxCampos campos, usando ',' ou ';' como delimitador. */

int separarCampos(char *linha, char **campos, int maxCampos);

/* Retorna 1 se o campo for NULL, vazio ou só com espaços. */

int campoNulo(const char *campo);
#endif
