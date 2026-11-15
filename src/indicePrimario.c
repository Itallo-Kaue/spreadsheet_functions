/*
    indicePrimario.c
    Implementação do índice primário sobre o campo idPoPs. O índice é mantido
    inteiro em memória como um vetor de pares (idPoPs, byteOffset) ordenado
    pelo idPoPs, o que permite a busca binária. Em disco, ele é gravado como
    um byte de status seguido dos pares, cada um com 12 bytes.
*/

#include <stdlib.h>
#include "indicePrimario.h"
#include "registroPoPs.h"

/* Função de comparação usada pelo qsort: ordena as entradas pelo idPoPs em
   ordem crescente. */

static int compararIndice(const void *a, const void *b) {
    int idA = ((IndiceEntry*)a)->idPoPs;
    int idB = ((IndiceEntry*)b)->idPoPs;
    if (idA < idB) return -1;
    if (idA > idB) return 1;
    return 0;
}

/* Percorre o arquivo de dados do fim do cabeçalho até o limite
   (proxByteOffset) e monta o vetor com um par (idPoPs, byteOffset) para cada
   registro não removido. O vetor começa com capacidade para 1000 entradas e
   dobra sempre que enche. Ele é devolvido ainda sem ordenação. */

IndiceArray* criarIndiceDeDados(FILE *f, int64_t limite) {
    IndiceArray *idx = malloc(sizeof(IndiceArray));
    idx->capacidade = 1000;
    idx->tamanho = 0;
    idx->entradas = malloc(idx->capacidade * sizeof(IndiceEntry));
    if (!idx->entradas) { free(idx); return NULL; }

    fseek(f, 25, SEEK_SET);
    while (ftell(f) < limite) {
        int64_t curOffset = ftell(f);
        RegistroPoPs r = lerRegistroPoPs(f);
        if(r.removido == '2') break;
        if(r.removido == NAO_REMOVIDO) {
            if(idx->tamanho == idx->capacidade) {
                idx->capacidade *= 2;
                idx->entradas = realloc(idx->entradas, idx->capacidade * sizeof(IndiceEntry));
            }
            idx->entradas[idx->tamanho].idPoPs = r.idPoPs;
            idx->entradas[idx->tamanho].byteOffset = curOffset;
            idx->tamanho++;
        }
        liberarRegistroPoPs(&r);
    }
    return idx;
}

/* Ordena as entradas do índice pelo idPoPs. */

void ordenarIndice(IndiceArray *idx) {
    if (!idx || idx->tamanho <= 1) return;
    qsort(idx->entradas, idx->tamanho, sizeof(IndiceEntry), compararIndice);
}

/* Grava o índice no arquivo informado, criando-o do zero: primeiro o status
   '0', depois cada par com o idPoPs em 4 bytes e o byteOffset em 8 bytes e,
   por fim, o status volta para '1', indicando que o arquivo está consistente. */

void gravarIndice(IndiceArray *idx, const char *arqIdx) {
    if (!idx) return;
    FILE *f = fopen(arqIdx, "wb");
    if(!f) return;
    char status = '0';
    fwrite(&status, 1, 1, f);
    for(int i=0; i<idx->tamanho; i++) {
        fwrite(&idx->entradas[i].idPoPs, 4, 1, f);
        fwrite(&idx->entradas[i].byteOffset, 8, 1, f);
    }
    status = '1';
    fseek(f, 0, SEEK_SET);
    fwrite(&status, 1, 1, f);
    fclose(f);
}

/* Lê o arquivo de índice inteiro para a memória. Retorna NULL se o arquivo não
   existir ou estiver inconsistente (status '0'). */

IndiceArray* carregarIndice(const char *arqIdx) {
    FILE *f = fopen(arqIdx, "rb");
    if(!f) return NULL;
    char status;
    if (fread(&status, 1, 1, f) != 1 || status == '0') { 
        fclose(f); 
        return NULL; 
    }
    
    IndiceArray *idx = malloc(sizeof(IndiceArray));
    idx->capacidade = 1000;
    idx->tamanho = 0;
    idx->entradas = malloc(idx->capacidade * sizeof(IndiceEntry));
    if (!idx->entradas) { free(idx); fclose(f); return NULL; }

    IndiceEntry e;
    while(fread(&e.idPoPs, 4, 1, f) == 1) {
        if (fread(&e.byteOffset, 8, 1, f) != 1) break;
        if(idx->tamanho == idx->capacidade) {
            idx->capacidade *= 2;
            IndiceEntry *temp = realloc(idx->entradas, idx->capacidade * sizeof(IndiceEntry));
            if (!temp) break;
            idx->entradas = temp;
        }
        idx->entradas[idx->tamanho++] = e;
    }
    fclose(f);
    return idx;
}

/* Libera a memória ocupada pelo índice. */

void liberarIndice(IndiceArray *idx) {
    if(idx) { 
        if(idx->entradas) free(idx->entradas); 
        free(idx); 
    }
}

/* Busca binária pelo idPoPs no vetor ordenado. Retorna 1 e o byteOffset
   correspondente em outOffset quando a chave é encontrada, e 0 caso contrário. */

int buscarNoIndice(IndiceArray *idx, int idBusca, int64_t *outOffset) {
    if (!idx) return 0;
    int ini = 0, fim = idx->tamanho - 1;
    while(ini <= fim) {
        int meio = (ini + fim) / 2;
        if(idx->entradas[meio].idPoPs == idBusca) {
            *outOffset = idx->entradas[meio].byteOffset;
            return 1;
        } else if(idx->entradas[meio].idPoPs < idBusca) {
            ini = meio + 1;
        } else {
            fim = meio - 1;
        }
    }
    return 0;
}

/* acha a posição por busca binária, desloca as entradas seguintes 
   uma casa para a frente e insere ali, sem reordenar nada */

void inserirNoIndice(IndiceArray *idx, int idPoPs, int64_t offset) {
    if(!idx) return;
    if(idx->tamanho == idx->capacidade) {
        idx->capacidade *= 2;
        IndiceEntry *temp = realloc(idx->entradas, idx->capacidade * sizeof(IndiceEntry));
        if (!temp) return;
        idx->entradas = temp;
    }
    
    int pos = idx->tamanho;
    int ini = 0, fim = idx->tamanho - 1;
    while (ini <= fim) {
        int meio = (ini + fim) / 2;
        if (idx->entradas[meio].idPoPs >= idPoPs) {
            pos = meio;
            fim = meio - 1;
        } else {
            ini = meio + 1;
        }
    }
    
    for (int i = idx->tamanho; i > pos; i--) {
        idx->entradas[i] = idx->entradas[i - 1];
    }

    idx->entradas[pos].idPoPs = idPoPs;
    idx->entradas[pos].byteOffset = offset;
    idx->tamanho++;
}

/* Remove a entrada com o idPoPs informado, deslocando as seguintes uma posição
   para trás, o que mantém o vetor ordenado. Se a chave não existir, nada é
   feito. */

void removerDoIndice(IndiceArray *idx, int idPoPs) {
    if(!idx) return;
    int pos = -1;
    for(int i=0; i<idx->tamanho; i++) {
        if(idx->entradas[i].idPoPs == idPoPs) { pos = i; break; }
    }
    if(pos != -1) {
        for(int i=pos; i<idx->tamanho-1; i++) {
            idx->entradas[i] = idx->entradas[i+1];
        }
        idx->tamanho--;
    }
}
