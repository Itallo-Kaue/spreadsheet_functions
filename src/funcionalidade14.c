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

/* Devolve uma cópia do registro com as strings duplicadas, de modo que a cópia
   continue válida depois que as strings do original forem liberadas. */

static RegistroPoPs clonarRegistroPoPs(RegistroPoPs *r) {
    RegistroPoPs copia = *r;
    copia.nomePoPs = meu_strdup(r->nomePoPs);
    copia.nomePais = meu_strdup(r->nomePais);
    return copia;
}

/* Aplica os p campos novos a um registro já lido, que está no byteOffset
   offset_atual, e o regrava no arquivo, mantendo o índice coerente.

   Se o novo idPoPs já pertencer a outro registro, a alteração da chave é
   desfeita. Se o registro atualizado couber no espaço antigo, ele é regravado
   no mesmo lugar e a sobra fica com lixo; senão, o espaço antigo vai para a
   lista de removidos e o registro é reinserido como em uma inserção. */

static void processarAtualizacaoRegistro(FILE *f, CabecalhoPoPs *cab, IndiceArray *idx, int64_t offset_atual, RegistroPoPs *reg, CriterioPoPs *novos, int p) {
    int oldId = reg->idPoPs; 
    int oldTam = reg->tamanhoRegistro;
    
    for(int k=0; k<p; k++) {
        AtualizarCampoPoPs(reg, &novos[k]);
    }
    
    /* Impede que a atualização gere um idPoPs repetido. */
    if (reg->idPoPs != oldId) {
        int64_t dummy;
        if (buscarNoIndice(idx, reg->idPoPs, &dummy)) {
            reg->idPoPs = oldId;
        }
    }
    
    int novoTamNecessario = tamanhoDoRegistroPoPs(reg);
    
    /* Cabe no espaço antigo: o tamanho do registro é mantido e só a chave do
       índice muda, caso o idPoPs tenha sido alterado. */
    if(novoTamNecessario <= oldTam) {
        reg->tamanhoRegistro = oldTam;
        escreverRegistroPorOffset(f, reg, offset_atual);
        if(oldId != reg->idPoPs) {
            removerDoIndice(idx, oldId);
            inserirNoIndice(idx, reg->idPoPs, offset_atual);
        }
    } else {
        /* Não cabe: guarda uma cópia do registro já atualizado, remove
           logicamente o antigo (com o id original, para tirá-lo do índice) e
           insere o espaço dele na lista de removidos. */
        RegistroPoPs backup = clonarRegistroPoPs(reg);
        
        reg->idPoPs = oldId; 
        reg->removido = REMOVIDO;
        removerDoIndice(idx, oldId); 
        preencherComLixoRegistroPoPs(reg); 
        reg->tamanhoRegistro = oldTam;
        
        inserirNaLista(f, cab, reg, offset_atual);
        
        /* A cópia é reinserida como um novo registro, reaproveitando o maior
           removido se couber ou indo para o fim do arquivo. */
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
        /* nroPoPs não é incrementado, pois o registro já existia. */
        liberarRegistroPoPs(&backup);
    }
}

/* Entrada: o nome do arquivo de dados, o nome do arquivo de índice e a
   quantidade n de atualizações; seguinte a isso, para cada atualização, a
   quantidade m de campos de busca com seus pares "nomeCampo" e "valor" e, em
   seguida, a quantidade p de campos a serem alterados com seus pares.

   Assim como na busca, quando o idPoPs faz parte do critério o registro é
   localizado diretamente pelo índice. Nos demais casos é feita uma varredura
   sequencial. */

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
    /* O status vai para '0' enquanto o arquivo estiver em uso e só volta para
       '1' depois que todas as operações terminarem. */
    cab.status = '0'; 
    escreverCabecalhoPoPs(f, &cab);
    
    for (int i = 0; i < n; i++) {
        int m; if (scanf("%d", &m) != 1) break;
        CriterioPoPs *busca = lerCriteriosPoPs(m);
        int p; if (scanf("%d", &p) != 1) { free(busca); break; }
        CriterioPoPs *novos = lerCriteriosPoPs(p);
        if(!busca || !novos) { if(busca) free(busca); if(novos) free(novos); continue; }
        
        /* Verifica se algum dos campos da busca é o idPoPs com valor não nulo,
           o que permite localizar o registro pelo índice. */
        int temIdPoPs = 0, idBuscado = -1;
        for(int j=0; j<m; j++) {
            if(strcmp(busca[j].nomeCampo, "idPoPs") == 0 && busca[j].valor[0] != '\0') {
                temIdPoPs = 1; idBuscado = atoi(busca[j].valor); break;
            }
        }
        
        /* lidoDireto guarda o resultado da tentativa pelo índice: 1 quando o
           registro foi encontrado e satisfaz a busca, -1 quando foi encontrado
           mas não satisfaz os demais campos (e então nenhum outro pode
           satisfazer) e 0 quando o índice não foi usado ou não contém o
           idPoPs, caso em que a busca é sequencial. */
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
            /* 
               Justificativa da varredura dupla: um registro que cresce pode ser regravado 
               num espaço livre mais à frente no arquivo, e uma varredura única o encontraria de novo e o atualizaria duas vezes. */
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
            
            /* Segunda etapa: cada registro coletado é relido pelo seu
               byteOffset e atualizado. */
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
    /* Marca o arquivo de dados como consistente ('1'), regrava o índice já
       atualizado e imprime os dois arquivos. */
    cab.status = '1'; 
    escreverCabecalhoPoPs(f, &cab);
    gravarIndice(idx, arqIdx);
    liberarIndice(idx); 
    fclose(f);
    binarioNaTela(arqBin); 
    binarioNaTela(arqIdx);
}
