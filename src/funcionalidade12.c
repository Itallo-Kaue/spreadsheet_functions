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

/* Entrada: o nome do arquivo de dados, o nome do arquivo de índice e a
   quantidade n de operações de remoção. Logo após pegar essas entradas, é
   lido, para cada busca, a quantidade m de campos seguida dos m pares do
   "nomeCampo" e do "valor". Todo registro que satisfizer todos os campos de
   uma operação é marcado como removido, sai do índice, tem seus campos de
   dados preenchidos com lixo e é inserido na lista de registros removidos, que
   é mantida em ordem decrescente de tamanho.

   A remoção é feita assim que a varredura encontra o registro, de modo que o
   arquivo é percorrido uma única vez por operação. */

void funcionalidade12() {
    char arqBin[TAM_NOME_ARQUIVO], arqIdx[TAM_NOME_ARQUIVO];
    int n;
    if (scanf("%63s %63s %d", arqBin, arqIdx, &n) != 3) return;
    
    /* Se o índice não existir ou estiver inconsistente, o arquivo de 
       dados nem chega a ser aberto e o seu status não é alterado. */
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
        CriterioPoPs *crit = lerCriteriosPoPs(m);
        if(!crit) continue;
        RegistroPoPs reg; int64_t offset_atual;
        iniciarBuscaPoPs(f);
        while (buscarProximoPoPs(f, crit, m, &reg, &offset_atual, cab.proxByteOffset)) {
            /* O registro é marcado como removido e sai do índice; em seguida
               os campos de dados são sobrescritos com lixo e a lista cuida do
               encadeamento, colocando o registro na posição certa e
               atualizando o cabeçalho. */
            reg.removido = REMOVIDO;
            removerDoIndice(idx, reg.idPoPs);
            preencherComLixoRegistroPoPs(&reg);
            inserirNaLista(f, &cab, &reg, offset_atual);
            cab.nroPoPs--;
            /* As escritas acima moveram o ponteiro do arquivo, então a
               varredura é reposicionada no registro seguinte ao removido antes
               de continuar. */
            fseek(f, offset_atual + reg.tamanhoRegistro, SEEK_SET);
            liberarRegistroPoPs(&reg);
        }
        free(crit);
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
