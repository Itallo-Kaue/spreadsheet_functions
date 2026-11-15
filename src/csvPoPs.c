/*
    csvPoPs.c
    Implementação da carga inicial do arquivo de dados dos PoPs a partir do
    CSV.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csvPoPs.h"
#include "registroPoPs.h"
#include "csvCampos.h"

/* Carrega o CSV dos PoPs no binário. Retorna 0 se der erro ou o CSV estiver vazio. */

int criarTabelaPoPs(const char *nomeCSV, const char *nomeBin) {
    FILE *fIn = fopen(nomeCSV, "r");
    FILE *fOut = fopen(nomeBin, "wb");
    if(!fIn || !fOut) { if(fIn) fclose(fIn); if(fOut) fclose(fOut); return 0; }
    
    /* Cabeçalho inicial com status '0'. */
    CabecalhoPoPs c = {'0', -1, 25, 0, 0};
    escreverCabecalhoPoPs(fOut, &c);
    
    /* Pula a linha de títulos do CSV. */
    char linha[1024];
    if (!fgets(linha, sizeof(linha), fIn)) {
        fclose(fIn);
        fclose(fOut);
        return 0;
    }
    
    while (fgets(linha, sizeof(linha), fIn)) {
        /* Tira o \n ou \r\n. */
        linha[strcspn(linha, "\n")] = 0;
        if(linha[strcspn(linha, "\r")] == '\r') linha[strcspn(linha, "\r")] = 0;
        
        /* A primeira coluna não é usada, por isso campos + 1. Sem idPoPs a linha é ignorada. */
        char *campos[5];
        separarCampos(linha, campos, 5);
        char **campos_reg = campos + 1;
        if(campoNulo(campos_reg[0])) continue;
        
        RegistroPoPs r = {0};
        r.removido = NAO_REMOVIDO;
        r.encadeamentoLista = -1;
        r.idPoPs = atoi(campos_reg[0]);
        
        r.nomePoPs = !campoNulo(campos_reg[1]) ? meu_strdup(campos_reg[1]) : NULL;
        r.nomePais = !campoNulo(campos_reg[2]) ? meu_strdup(campos_reg[2]) : NULL;
        
        if (!campoNulo(campos_reg[3]) && strlen(campos_reg[3]) > 0) {
            r.siglaPais[0] = campos_reg[3][0];
            r.siglaPais[1] = (strlen(campos_reg[3]) > 1) ? campos_reg[3][1] : LIXO;
        } else {
            r.siglaPais[0] = LIXO;
            r.siglaPais[1] = LIXO;
        }
        
        /* Sem lixo na carga inicial. */
        r.tamanhoRegistro = tamanhoDoRegistroPoPs(&r);
        escreverRegistroPoPs(fOut, &r);
        
        c.proxByteOffset += r.tamanhoRegistro;
        c.nroPoPs++;
        liberarRegistroPoPs(&r);
    }
    
    /* Atualiza o cabeçalho, status '1' e fecha. */
    c.status = '1';
    escreverCabecalhoPoPs(fOut, &c);
    fclose(fIn);
    fclose(fOut);
    return 1;
}
