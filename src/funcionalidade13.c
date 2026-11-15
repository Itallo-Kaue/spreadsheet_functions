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

/* Entrada: o nome do arquivo de dados, o nome do arquivo de índice e a
   quantidade n de inserções; para cada uma, os quatro campos do registro na
   ordem idPoPs, nomePoPs, nomePais e siglaPais.

   O espaço é escolhido pela estratégia worst fit: o topo da lista de removidos
   é sempre o maior registro removido, e ele é reaproveitado se o novo registro
   couber nele. Caso contrário, o registro é escrito no fim do arquivo. */

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
    /* O status vai para '0' enquanto o arquivo estiver em uso e só volta para
       '1' depois que todas as operações terminarem. */
    cab.status = '0'; 
    escreverCabecalhoPoPs(f, &cab);
    
    for (int i = 0; i < n; i++) {
        /* Cada novo registro começa como não removido e fora da lista de
           removidos. */
        char val[100]; RegistroPoPs reg = {0};
        reg.removido = NAO_REMOVIDO; reg.encadeamentoLista = -1;
        
        LerValorDoCampoPoPs("idPoPs", val); reg.idPoPs = atoi(val);
        
        /* Verifica duplicidade de ID no índice primário. Se o idPoPs já
           existir, os demais campos ainda são lidos da entrada, para não
           desalinhar a leitura das próximas inserções, e o registro é
           descartado. */
        int64_t dummyOff;
        if (buscarNoIndice(idx, reg.idPoPs, &dummyOff)) {
            LerValorDoCampoPoPs("nomePoPs", val);
            LerValorDoCampoPoPs("nomePais", val);
            LerValorDoCampoPoPs("siglaPais", val);
            liberarRegistroPoPs(&reg);
            continue;
        }
        
        /* Campos nulos ficam como NULL nas strings e com lixo ('$') na sigla
           do país. */
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
        
        /* Tenta reaproveitar o maior registro removido. Se ele for
           reaproveitado, o novo registro herda o tamanho do espaço recuperado
           e a sobra é preenchida com lixo; senão, ele é escrito no fim do
           arquivo e proxByteOffset avança. */
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
        /* Atualiza o contador de PoPs e insere o par (idPoPs, byteOffset) no
           índice, que é mantido ordenado. */
        cab.nroPoPs++;
        inserirNoIndice(idx, reg.idPoPs, offsetInsercao);
        liberarRegistroPoPs(&reg);
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
