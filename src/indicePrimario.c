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

/* Compara por idPoPs, pro qsort. */

static int compararIndice(const void *a, const void *b) {
    int idA = ((IndiceEntry*)a)->idPoPs;
    int idB = ((IndiceEntry*)b)->idPoPs;
    if (idA < idB) return -1;
    if (idA > idB) return 1;
    return 0;
}

/* Monta o vetor (idPoPs, byteOffset) dos não removidos, ainda sem ordenar. */

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

/* Grava o índice: status e depois os pares (4 + 8 bytes). */

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

/* Carrega o índice inteiro; NULL se não existir ou o status for '0'. */

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

/* Busca binária pelo idPoPs. */

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

/* Acha a posição por busca binária, desloca as entradas seguintes 
   uma casa para a frente e insere ali, sem reordenar nada. */

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

/* Remove a entrada e puxa as outras uma posição pra trás. */

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
