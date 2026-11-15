/*
    funcionalidade4.c
    Recuperação de um único registro por acesso direto, a partir do seu RRN.
    O acesso direto só é possível porque os registros têm tamanho fixo.
*/

#include <stdio.h>
#include "funcionalidades.h"
#include "registro.h"
#include "util.h"


void Funcionalidade_4() {
    char nomeArquivo[TAM_NOME_ARQUIVO];
    int rrn;
    if (scanf("%63s %d", nomeArquivo, &rrn) != 2) return;
    Cabecalho cab;
    FILE *bin = abrirArquivoBinarioLeitura(nomeArquivo, &cab);
    if (bin == NULL) {
        printf(MSG_FALHA);
        return;
    }
    Registro reg;
    
    if (rrn < 0 || rrn >= cab.proxRRN || !lerRegistroPorRRN(bin, &reg, rrn)) {
        printf(MSG_INEXISTENTE);
    } else if (!ImprimirRegistros(&reg)) {
        printf(MSG_INEXISTENTE);
    }
    fclose(bin);
}
