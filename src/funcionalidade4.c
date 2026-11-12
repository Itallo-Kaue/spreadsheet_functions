/*
    funcionalidade4.c
    Recuperação de um único registro por acesso direto, a partir do seu RRN.
    O acesso direto só é possível porque os registros têm tamanho fixo.
*/

#include <stdio.h>

#include "funcionalidades.h"
#include "registro.h"
#include "util.h"

/* Entrada: o nome do arquivo de dados e o RRN do registro desejado.
   Sem varredura, o offset é calculado diretamente a partir do RRN,
   devido ao registro ter tamanho fixo. Um RRN fora do arquivo
   ou um registro logicamente removido não serão considerados. */

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

    /* O RRN precisa estar dentro da faixa já usada do arquivo. */
    if (rrn < 0 || rrn >= cab.proxRRN || !lerRegistroPorRRN(bin, &reg, rrn)) {
        printf(MSG_INEXISTENTE);
    } else if (!ImprimirRegistros(&reg)) {
        printf(MSG_INEXISTENTE);
    }

    fclose(bin);
}
