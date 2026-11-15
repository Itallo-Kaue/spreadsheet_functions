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

/* Lê o arquivo CSV informado e grava cada uma de suas linhas como um registro
   de tamanho variável no arquivo binário de saída, que é criado do zero.
   Retorna 1 em caso de sucesso e 0 se algum dos arquivos não puder ser aberto
   ou se o CSV estiver vazio. */

int criarTabelaPoPs(const char *nomeCSV, const char *nomeBin) {
    FILE *fIn = fopen(nomeCSV, "r");
    FILE *fOut = fopen(nomeBin, "wb");
    if(!fIn || !fOut) { if(fIn) fclose(fIn); if(fOut) fclose(fOut); return 0; }
    
    /* O cabeçalho é gravado logo de início com status '0', a lista de
       removidos vazia e o próximo byte livre logo após os seus 25 bytes. */
    CabecalhoPoPs c = {'0', -1, 25, 0, 0};
    escreverCabecalhoPoPs(fOut, &c);
    
    /* A primeira linha do CSV traz os nomes das colunas e é descartada. */
    char linha[1024];
    if (!fgets(linha, sizeof(linha), fIn)) {
        fclose(fIn);
        fclose(fOut);
        return 0;
    }
    
    while (fgets(linha, sizeof(linha), fIn)) {
        /* Remove a quebra de linha do fim, seja ela '\n' ou '\r\n'. */
        linha[strcspn(linha, "\n")] = 0;
        if(linha[strcspn(linha, "\r")] == '\r') linha[strcspn(linha, "\r")] = 0;
        
        /* A linha tem cinco colunas; a primeira não faz parte do registro
           PoPs, então os campos do registro começam em campos + 1. Linhas sem
           idPoPs são ignoradas. */
        char *campos[5];
        separarCampos(linha, campos, 5);
        char **campos_reg = campos + 1;
        if(campoNulo(campos_reg[0])) continue;
        
        /* Cada campo do registro recebe o seu valor direto da linha do CSV. Os
           campos opcionais ficam como NULL (strings) ou com lixo ('$', na
           sigla) quando vêm nulos no CSV. */
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
        
        /* Na carga inicial o registro ocupa exatamente o seu tamanho, sem lixo
           no fim, e é escrito logo após o anterior, sem precisar de fseek. */
        r.tamanhoRegistro = tamanhoDoRegistroPoPs(&r);
        escreverRegistroPoPs(fOut, &r);
        
        c.proxByteOffset += r.tamanhoRegistro;
        c.nroPoPs++;
        liberarRegistroPoPs(&r);
    }
    
    /* Regrava o cabeçalho com os contadores finais, marca o arquivo como
       consistente ('1') e fecha os dois arquivos. */
    c.status = '1';
    escreverCabecalhoPoPs(fOut, &c);
    fclose(fIn);
    fclose(fOut);
    return 1;
}
