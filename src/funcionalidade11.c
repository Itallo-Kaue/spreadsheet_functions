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

/* Entrada: o nome do arquivo de dados, o nome do arquivo de índice e a
   quantidade n de buscas. Logo após pegar essas entradas, é lido, para cada
   busca, a quantidade m de campos seguida dos m pares do "nomeCampo" e do
   "valor". Ao final de cada busca é impressa uma linha em branco, separando as
   respostas.

   Como o idPoPs é chave primária, no máximo um registro pode satisfazer uma
   busca que o contenha: nesse caso o byteOffset é obtido por busca binária no
   índice e o registro é lido por acesso direto, sem varrer o arquivo. */

void funcionalidade11() {
    char arqBin[TAM_NOME_ARQUIVO], arqIdx[TAM_NOME_ARQUIVO];
    int n;
    if (scanf("%63s %63s %d", arqBin, arqIdx, &n) != 3) return;
    
    /* O índice é carregado inteiro para a memória; se ele não existir ou
       estiver inconsistente, nada mais é feito. */
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
        /* Verifica se algum dos campos da busca é o idPoPs com valor não nulo,
           o que permite localizar o registro pelo índice. */
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
                /* O registro achado pelo índice ainda precisa satisfazer os
                   demais campos da busca. */
                RegistroPoPs r = lerRegistroPorOffset(f, off);
                if(registroSatisfazCriteriosPoPs(&r, crit, m)) { ImprimirRegistroPoPs(&r); encontrados++; }
                liberarRegistroPoPs(&r);
            }
        } else {
            /* Sem o idPoPs, a busca é sequencial por todo o arquivo de dados. */
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
