/*
    funcionalidade12.c
    Remoção lógica dos registros que satisfazem um critério de busca, com
    reaproveitamento dinâmico de espaço por meio da lista de registros
    removidos e atualização do índice primário. Corresponde ao comando DELETE
    da linguagem SQL.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funcionalidades.h"
#include "buscaPoPs.h"
#include "indicePrimario.h"
#include "listaRemovidos.h"
#include "fornecidas.h"

/* Entrada: dados, índice e n remoções (m pares nomeCampo valor em cada). */

void funcionalidade12() {
    char arqBin[TAM_NOME_ARQUIVO], arqIdx[TAM_NOME_ARQUIVO];
    int n;
    if (scanf("%63s %63s %d", arqBin, arqIdx, &n) != 3) return;
    
    /* Índice inválido: nem abre o arquivo de dados. */
    IndiceArray *idx = carregarIndice(arqIdx);
    if (!idx) {
        printf(MSG_FALHA);
        return;
    }
    
    FILE *f = fopen(arqBin, "rb+");
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
    /* Status '0' enquanto o arquivo está aberto. */
    cab.status = '0'; 
    escreverCabecalhoPoPs(f, &cab);
    
    for (int i = 0; i < n; i++) {
        int m; if (scanf("%d", &m) != 1) break;
        CriterioPoPs *crit = lerCriteriosPoPs(m);
        if(!crit) continue;
        RegistroPoPs reg; int64_t offset_atual;
        iniciarBuscaPoPs(f);
        while (buscarProximoPoPs(f, crit, m, &reg, &offset_atual, cab.proxByteOffset)) {
            /* Remove: sai do índice, enche de lixo e vai pra lista. */
            reg.removido = REMOVIDO;
            removerDoIndice(idx, reg.idPoPs);
            preencherComLixoRegistroPoPs(&reg);
            inserirNaLista(f, &cab, &reg, offset_atual);
            cab.nroPoPs--;
            /* As escritas mexeram no ponteiro, volta pro próximo registro. */
            fseek(f, offset_atual + reg.tamanhoRegistro, SEEK_SET);
            liberarRegistroPoPs(&reg);
        }
        free(crit);
    }
    cab.status = '1'; 
    escreverCabecalhoPoPs(f, &cab);
    gravarIndice(idx, arqIdx);
    liberarIndice(idx); 
    fclose(f);
    binarioNaTela(arqBin); 
    binarioNaTela(arqIdx);
}
