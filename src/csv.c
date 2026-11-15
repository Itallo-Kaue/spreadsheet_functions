/*
    csv.c
    Implementação da carga inicial do arquivo de dados a partir do CSV.
*/

#include "csv.h"
#include "registro.h"
#include "csvCampos.h"

#include <stdio.h>
#include <stdlib.h>

/* Campos: idPoPs, idPoPsConectado, velocidade, unidadeMedida. */

#define NUM_CAMPOS 4
#define TAM_LINHA 1024

int criarTabela(const char *nomeCSV, const char *nomeBin) {
    FILE *csv = fopen(nomeCSV, "r");
    if (csv == NULL) return 0;

    Cabecalho cab;
    FILE *bin = criarArquivoBinario(nomeBin, &cab);
    if (bin == NULL) {
        fclose(csv);
        return 0;
    }

    char linha[TAM_LINHA];
    fgets(linha, sizeof(linha), csv);

    while (fgets(linha, sizeof(linha), csv) != NULL) {
        char *campos[NUM_CAMPOS];
        separarCampos(linha, campos, NUM_CAMPOS);

        char *pops = campos[0];
        char *cone = campos[1];
        char *velo = campos[2];
        char *unid = campos[3];

        if (campoNulo(pops) || campoNulo(cone)) continue;

        /* Nulo: -1 no inteiro e '$' na unidade. */

        Registro reg;
        reg.removido = NAO_REMOVIDO;
        reg.encadeamentoPilha = -1;
        reg.idPoPs = atoi(pops);
        reg.idPoPsConectado = atoi(cone);
        reg.velocidade = campoNulo(velo) ? INT_NULO : atoi(velo);
        reg.unidadeMedida = campoNulo(unid) ? LIXO : unid[0];

        escreverRegistro(bin, &reg);
        cab.proxRRN++;
        cab.nroPares++;
    }

    fclose(csv);

    /* Atualiza o cabeçalho e fecha com status '1'. */

    fecharArquivoBinario(bin, &cab);
    return 1;
}
