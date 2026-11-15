/*
    funcionalidade14.c
    Atualização dos registros que satisfazem um critério de busca. Como os
    registros têm tamanho variável, o registro é atualizado no próprio lugar
    quando cabe e, caso contrário, é removido e reinserido em outra posição.
    Corresponde ao comando UPDATE da linguagem SQL.
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

/* Cópia com as strings duplicadas. */

static RegistroPoPs clonarRegistroPoPs(RegistroPoPs *r) {
    RegistroPoPs copia = *r;
    copia.nomePoPs = meu_strdup(r->nomePoPs);
    copia.nomePais = meu_strdup(r->nomePais);
    return copia;
}

/* Aplica os campos novos e regrava. Se não couber no espaço antigo,
   remove e insere de novo. */

static void processarAtualizacaoRegistro(FILE *f, CabecalhoPoPs *cab, IndiceArray *idx, int64_t offset_atual, RegistroPoPs *reg, CriterioPoPs *novos, int p) {
    int oldId = reg->idPoPs; 
    int oldTam = reg->tamanhoRegistro;
    
    for(int k=0; k<p; k++) {
        AtualizarCampoPoPs(reg, &novos[k]);
    }
    
    /* Não deixa repetir idPoPs. */
    if (reg->idPoPs != oldId) {
        int64_t dummy;
        if (buscarNoIndice(idx, reg->idPoPs, &dummy)) {
            reg->idPoPs = oldId;
        }
    }
    
    int novoTamNecessario = tamanhoDoRegistroPoPs(reg);
    
    /* Cabe no mesmo lugar. */
    if(novoTamNecessario <= oldTam) {
        reg->tamanhoRegistro = oldTam;
        escreverRegistroPorOffset(f, reg, offset_atual);
        if(oldId != reg->idPoPs) {
            removerDoIndice(idx, oldId);
            inserirNoIndice(idx, reg->idPoPs, offset_atual);
        }
    } else {
        /* Não cabe: remove o antigo e insere a cópia como registro novo. */
        RegistroPoPs backup = clonarRegistroPoPs(reg);
        
        reg->idPoPs = oldId; 
        reg->removido = REMOVIDO;
        removerDoIndice(idx, oldId); 
        preencherComLixoRegistroPoPs(reg); 
        reg->tamanhoRegistro = oldTam;
        
        inserirNaLista(f, cab, reg, offset_atual);
        
        int tamRecuperado = 0;
        int64_t offsetInsercao = retirarMaior(f, cab, novoTamNecessario, &tamRecuperado);
        if(offsetInsercao != -1) { 
            backup.tamanhoRegistro = tamRecuperado; 
            escreverRegistroPorOffset(f, &backup, offsetInsercao); 
        } else { 
            backup.tamanhoRegistro = novoTamNecessario; 
            offsetInsercao = cab->proxByteOffset; 
            escreverRegistroPorOffset(f, &backup, offsetInsercao); 
            cab->proxByteOffset += novoTamNecessario; 
        }
        inserirNoIndice(idx, backup.idPoPs, offsetInsercao); 
        liberarRegistroPoPs(&backup);
    }
}

/* Entrada: dados, índice e n atualizações (busca / campos novos). */

void funcionalidade14() {
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
        int m; if (scanf("%d", &m) != 1) break;
        CriterioPoPs *busca = lerCriteriosPoPs(m);
        int p; if (scanf("%d", &p) != 1) { free(busca); break; }
        CriterioPoPs *novos = lerCriteriosPoPs(p);
        if(!busca || !novos) { if(busca) free(busca); if(novos) free(novos); continue; }
        
        /* Se tiver idPoPs na busca dá pra usar o índice. */
        int temIdPoPs = 0, idBuscado = -1;
        for(int j=0; j<m; j++) {
            if(strcmp(busca[j].nomeCampo, "idPoPs") == 0 && busca[j].valor[0] != '\0') {
                temIdPoPs = 1; idBuscado = atoi(busca[j].valor); break;
            }
        }
        
        /* Valores de lidoDireto: 1 achou pelo índice, -1 achou mas não bate, 0 não usou o índice. */
        RegistroPoPs reg; int64_t offset_atual = -1; int lidoDireto = 0;
        if(temIdPoPs && buscarNoIndice(idx, idBuscado, &offset_atual)) {
            reg = lerRegistroPorOffset(f, offset_atual);
            if(registroSatisfazCriteriosPoPs(&reg, busca, m)) lidoDireto = 1;
            else { liberarRegistroPoPs(&reg); lidoDireto = -1; }
        }
        
        if (lidoDireto == 1) {
            processarAtualizacaoRegistro(f, &cab, idx, offset_atual, &reg, novos, p);
            liberarRegistroPoPs(&reg);
        } else if (lidoDireto == 0) {
            /* Primeiro guarda os offsets e só depois atualiza, senão um registro que
               foi pra frente no arquivo seria atualizado de novo. */
            iniciarBuscaPoPs(f);
            int cap_offsets = 10;
            int num_offsets = 0;
            int64_t *offsets = malloc(cap_offsets * sizeof(int64_t));
            if (!offsets) { free(busca); free(novos); break; }
            
            while(buscarProximoPoPs(f, busca, m, &reg, &offset_atual, cab.proxByteOffset)) {
                if(num_offsets >= cap_offsets) {
                    cap_offsets *= 2;
                    int64_t *temp = realloc(offsets, cap_offsets * sizeof(int64_t));
                    if (!temp) break;
                    offsets = temp;
                }
                offsets[num_offsets++] = offset_atual; 
                liberarRegistroPoPs(&reg);
            }
            
            for(int j=0; j<num_offsets; j++) {
                reg = lerRegistroPorOffset(f, offsets[j]);
                processarAtualizacaoRegistro(f, &cab, idx, offsets[j], &reg, novos, p);
                liberarRegistroPoPs(&reg);
            }
            free(offsets);
        }
        free(busca); 
        free(novos);
    }
    cab.status = '1'; 
    escreverCabecalhoPoPs(f, &cab);
    gravarIndice(idx, arqIdx);
    liberarIndice(idx); 
    fclose(f);
    binarioNaTela(arqBin); 
    binarioNaTela(arqIdx);
}
