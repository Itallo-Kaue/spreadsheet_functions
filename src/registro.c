/*
    registro.c
    Implementação da manipulação do registro de cabeçalho e dos registros de
    dados. Toda a entrada/saída é feita campo a campo.
*/

#include "registro.h"

#include <stdio.h>

/* ------------------------------------------------------------------
    Cabeçalho
   ------------------------------------------------------------------ */

/* Cabeçalho inicial: status '0', pilha vazia e contadores zerados. */

void inicializarCabecalho(Cabecalho *cab) {
    cab->status = STATUS_INCONSISTENTE;
    cab->topoPilha = -1;
    cab->proxRRN = 0;
    cab->nroRegRem = 0;
    cab->nroPares = 0;
}

/* Grava o cabeçalho (17 bytes). */

void escreverCabecalho(FILE *bin, Cabecalho *cab) {
    fseek(bin, 0, SEEK_SET);

    fwrite(&cab->status, sizeof(char), 1, bin);
    fwrite(&cab->topoPilha, sizeof(int), 1, bin);
    fwrite(&cab->proxRRN, sizeof(int), 1, bin);
    fwrite(&cab->nroRegRem, sizeof(int), 1, bin);
    fwrite(&cab->nroPares, sizeof(int), 1, bin);
}

/* Lê o cabeçalho, para logo se o status for '0'. */

int lerCabecalho(FILE *bin, Cabecalho *cab) {
    fseek(bin, 0, SEEK_SET);

    if (fread(&cab->status, sizeof(char), 1, bin) != 1) return 0;
    if (cab->status != STATUS_CONSISTENTE) return 0;

    if (fread(&cab->topoPilha, sizeof(int), 1, bin) != 1) return 0;
    if (fread(&cab->proxRRN, sizeof(int), 1, bin) != 1) return 0;
    if (fread(&cab->nroRegRem, sizeof(int), 1, bin) != 1) return 0;
    if (fread(&cab->nroPares, sizeof(int), 1, bin) != 1) return 0;

    return 1;
}

/* ------------------------------------------------------------------
    Registros de dados
   ------------------------------------------------------------------ */

/* 17 do cabeçalho + 18 por registro. */

long byteOffset(int rrn) {
    return (long)TAM_CABECALHO + (long)rrn * TAM_REGISTRO;
}

/* Grava os 18 bytes do registro na posição atual. */

void escreverRegistro(FILE *bin, Registro *reg) {
    fwrite(&reg->removido, sizeof(char), 1, bin);
    fwrite(&reg->encadeamentoPilha, sizeof(int), 1, bin);
    fwrite(&reg->idPoPs, sizeof(int), 1, bin);
    fwrite(&reg->idPoPsConectado, sizeof(int), 1, bin);
    fwrite(&reg->velocidade, sizeof(int), 1, bin);
    fwrite(&reg->unidadeMedida, sizeof(char), 1, bin);
}

/* Acesso direto para escrita: posiciona no byte offset do RRN e grava. */

void escreverRegistroPorRRN(FILE *bin, Registro *reg, int rrn) {
    fseek(bin, byteOffset(rrn), SEEK_SET);
    escreverRegistro(bin, reg);
}

/* Lê o registro na posição atual. Se for removido só pula o resto.
   Retorna 0 no fim do arquivo. */

int lerRegistro(FILE *bin, Registro *reg) {
    if (fread(&reg->removido, sizeof(char), 1, bin) != 1) return 0;

    if (reg->removido == REMOVIDO) {
        if (fseek(bin, (long)(TAM_REGISTRO - 1), SEEK_CUR) != 0) return 0;
        return 1;
    }

    if (fread(&reg->encadeamentoPilha, sizeof(int), 1, bin) != 1) return 0;
    if (fread(&reg->idPoPs, sizeof(int), 1, bin) != 1) return 0;
    if (fread(&reg->idPoPsConectado, sizeof(int), 1, bin) != 1) return 0;
    if (fread(&reg->velocidade, sizeof(int), 1, bin) != 1) return 0;
    if (fread(&reg->unidadeMedida, sizeof(char), 1, bin) != 1) return 0;

    return 1;
}

/* Lê pelo RRN. Retorna 0 se o RRN for inválido. */

int lerRegistroPorRRN(FILE *bin, Registro *reg, int rrn) {
    if (rrn < 0) return 0;
    if (fseek(bin, byteOffset(rrn), SEEK_SET) != 0) return 0;

    return lerRegistro(bin, reg);
}

/* Enche os campos de dados com '$' (os inteiros byte a byte). */

void preencherComLixo(Registro *reg) {
    unsigned char *p;
    int i;

    p = (unsigned char *)&reg->idPoPs;
    for (i = 0; i < (int)sizeof(int); i++) p[i] = (unsigned char)LIXO;

    p = (unsigned char *)&reg->idPoPsConectado;
    for (i = 0; i < (int)sizeof(int); i++) p[i] = (unsigned char)LIXO;

    p = (unsigned char *)&reg->velocidade;
    for (i = 0; i < (int)sizeof(int); i++) p[i] = (unsigned char)LIXO;

    reg->unidadeMedida = LIXO;
}

/* ------------------------------------------------------------------
    Abertura e fechamento do arquivo de dados
   ------------------------------------------------------------------ */

/* Cria o arquivo e já grava o cabeçalho com status '0'. */

FILE *criarArquivoBinario(const char *nomeArquivo, Cabecalho *cab) {
    FILE *bin = fopen(nomeArquivo, "wb");
    if (bin == NULL) return NULL;

    inicializarCabecalho(cab);
    escreverCabecalho(bin, cab);

    return bin;
}

/* Abre com rb+ e marca o status como '0'. NULL se não existir ou estiver inconsistente. */

FILE *abrirArquivoBinario(const char *nomeArquivo, Cabecalho *cab) {
    FILE *bin = fopen(nomeArquivo, "rb+");
    if (bin == NULL) return NULL;

    if (!lerCabecalho(bin, cab)) {
        fclose(bin);
        return NULL;
    }

    cab->status = STATUS_INCONSISTENTE;
    escreverCabecalho(bin, cab);

    return bin;
}

/* Abre só pra leitura. */

FILE *abrirArquivoBinarioLeitura(const char *nomeArquivo, Cabecalho *cab) {
    FILE *bin = fopen(nomeArquivo, "rb");
    if (bin == NULL) return NULL;

    if (!lerCabecalho(bin, cab)) {
        fclose(bin);
        return NULL;
    }

    return bin;
}

/* Regrava o cabeçalho com status '1' e fecha. */

void fecharArquivoBinario(FILE *bin, Cabecalho *cab) {
    if (bin == NULL) return;

    cab->status = STATUS_CONSISTENTE;
    escreverCabecalho(bin, cab);

    fclose(bin);
}
