/*
    funcionalidade11.c
    Recuperação dos registros que satisfazem um critério de busca, usando o
    índice primário quando o idPoPs faz parte do critério e a busca sequencial
    nos demais casos. Corresponde ao comando SELECT ... WHERE da linguagem
    SQL.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funcionalidades.h"
#include "buscaPoPs.h"
#include "indicePrimario.h"
#include "utilPoPs.h"

/* Entrada: dados, índice e n buscas (m pares nomeCampo valor em cada). */

void funcionalidade11() {
    char arqBin[TAM_NOME_ARQUIVO], arqIdx[TAM_NOME_ARQUIVO];
    int n;
    if (scanf("%63s %63s %d", arqBin, arqIdx, &n) != 3) return;
    
    IndiceArray *idx = carregarIndice(arqIdx);
    if (!idx) {
        printf(MSG_FALHA);
        return;
    }
    
    FILE *f = fopen(arqBin, "rb");
    if(!f) { 
        printf(MSG_FALHA); 
        liberarIndice(idx);
        return; 
    }
    CabecalhoPoPs cab = lerCabecalhoPoPs(f);
    if(cab.status == '0') { 
        printf(MSG_FALHA); 
        fclose(f); 
        liberarIndice(idx);
        return; 
    }
    
    for (int i = 0; i < n; i++) {
        int m; if (scanf("%d", &m) != 1) break;
        CriterioPoPs *crit = lerCriteriosPoPs(m);
        if(!crit) continue;
        /* Se tiver idPoPs na busca dá pra usar o índice. */
        int temIdPoPs = 0, idBuscado = -1;
        for(int j=0; j<m; j++) {
            if(strcmp(crit[j].nomeCampo, "idPoPs") == 0 && crit[j].valor[0] != '\0') {
                temIdPoPs = 1; idBuscado = atoi(crit[j].valor); break;
            }
        }
        int encontrados = 0;
        if(temIdPoPs) {
            int64_t off;
            if(buscarNoIndice(idx, idBuscado, &off)) {
                /* Ainda tem que conferir os outros campos. */
                RegistroPoPs r = lerRegistroPorOffset(f, off);
                if(registroSatisfazCriteriosPoPs(&r, crit, m)) { ImprimirRegistroPoPs(&r); encontrados++; }
                liberarRegistroPoPs(&r);
            }
        } else {
            /* Sem idPoPs: busca sequencial. */
            iniciarBuscaPoPs(f);
            RegistroPoPs reg; int64_t offset_atual;
            while (buscarProximoPoPs(f, crit, m, &reg, &offset_atual, cab.proxByteOffset)) {
                ImprimirRegistroPoPs(&reg); encontrados++; liberarRegistroPoPs(&reg);
            }
        }
        if (encontrados == 0) printf(MSG_INEXISTENTE);
        printf("\n");
        free(crit);
    }
    liberarIndice(idx); 
    fclose(f);
}
