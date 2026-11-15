/*
    funcionalidade13.c
    Inserção de novos registros, reaproveitando o espaço dos registros
    removidos e impedindo a inserção de um idPoPs repetido. Corresponde ao
    comando INSERT INTO da linguagem SQL.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funcionalidades.h"
#include "buscaPoPs.h"
#include "indicePrimario.h"
#include "listaRemovidos.h"
#include "fornecidas.h"
#include "registroPoPs.h"

/* Entrada: dados, índice e n inserções (idPoPs nomePoPs nomePais siglaPais). */

void funcionalidade13() {
    char arqBin[TAM_NOME_ARQUIVO], arqIdx[TAM_NOME_ARQUIVO];
    int n;
    if (scanf("%63s %63s %d", arqBin, arqIdx, &n) != 3) return;
    
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
        char val[100]; RegistroPoPs reg = {0};
        reg.removido = NAO_REMOVIDO; reg.encadeamentoLista = -1;
        
        LerValorDoCampoPoPs("idPoPs", val); reg.idPoPs = atoi(val);
        
        /* ID repetido: lê o resto da linha mesmo assim e descarta. */
        int64_t dummyOff;
        if (buscarNoIndice(idx, reg.idPoPs, &dummyOff)) {
            LerValorDoCampoPoPs("nomePoPs", val);
            LerValorDoCampoPoPs("nomePais", val);
            LerValorDoCampoPoPs("siglaPais", val);
            liberarRegistroPoPs(&reg);
            continue;
        }
        
        LerValorDoCampoPoPs("nomePoPs", val); reg.nomePoPs = (val[0] != '\0') ? meu_strdup(val) : NULL;
        LerValorDoCampoPoPs("nomePais", val); reg.nomePais = (val[0] != '\0') ? meu_strdup(val) : NULL;
        LerValorDoCampoPoPs("siglaPais", val); 
        if (val[0] != '\0') {
            reg.siglaPais[0] = val[0];
            reg.siglaPais[1] = (strlen(val) > 1) ? val[1] : LIXO;
        } else {
            reg.siglaPais[0] = LIXO;
            reg.siglaPais[1] = LIXO;
        }
        
        /* Worst fit: tenta usar o maior removido, senão vai pro fim. */
        int tamNecessario = tamanhoDoRegistroPoPs(&reg);
        int tamRecuperado = 0;
        int64_t offsetInsercao = retirarMaior(f, &cab, tamNecessario, &tamRecuperado);
        if (offsetInsercao != -1) {
            reg.tamanhoRegistro = tamRecuperado;
            escreverRegistroPorOffset(f, &reg, offsetInsercao);
        } else {
            reg.tamanhoRegistro = tamNecessario;
            offsetInsercao = cab.proxByteOffset;
            escreverRegistroPorOffset(f, &reg, offsetInsercao);
            cab.proxByteOffset += tamNecessario;
        }
        cab.nroPoPs++;
        inserirNoIndice(idx, reg.idPoPs, offsetInsercao);
        liberarRegistroPoPs(&reg);
    }
    cab.status = '1'; 
    escreverCabecalhoPoPs(f, &cab);
    gravarIndice(idx, arqIdx);
    liberarIndice(idx); 
    fclose(f);
    binarioNaTela(arqBin); 
    binarioNaTela(arqIdx);
}
