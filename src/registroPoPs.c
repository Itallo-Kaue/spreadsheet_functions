/*
    registroPoPs.c
    Implementação da leitura e da escrita, campo a campo, do cabeçalho e dos
    registros de tamanho variável do arquivo de dados dos PoPs.
*/

#include <stdlib.h>
#include <string.h>
#include "registroPoPs.h"

/* Substitui a strdup, que não é C99. */

char *meu_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s) + 1;
    char *p = malloc(len);
    if (!p) return NULL;
    memcpy(p, s, len);
    return p;
}

/* Cabeçalho, 25 bytes. */

void escreverCabecalhoPoPs(FILE *f, CabecalhoPoPs *c) {
    if (!f || !c) return;
    fseek(f, 0, SEEK_SET);
    fwrite(&c->status, 1, 1, f);
    fwrite(&c->topoLista, 8, 1, f);
    fwrite(&c->proxByteOffset, 8, 1, f);
    fwrite(&c->nroRegRem, 4, 1, f);
    fwrite(&c->nroPoPs, 4, 1, f);
}

/* Lê o cabeçalho, para se o status for '0'. */

CabecalhoPoPs lerCabecalhoPoPs(FILE *f) {
    CabecalhoPoPs c = {'0', -1, 25, 0, 0};
    if (!f) return c;
    fseek(f, 0, SEEK_SET);
    if (fread(&c.status, 1, 1, f) != 1) {
        c.status = '0';
        return c;
    }
    if (c.status != '1') {
        return c;
    }
    fread(&c.topoLista, 8, 1, f);
    fread(&c.proxByteOffset, 8, 1, f);
    fread(&c.nroRegRem, 4, 1, f);
    fread(&c.nroPoPs, 4, 1, f);
    return c;
}

/* 19 bytes fixos + as strings + um '|' pra cada. */

int tamanhoDoRegistroPoPs(RegistroPoPs *r) {
    int size = 19;
    if (r->nomePoPs) size += strlen(r->nomePoPs);
    size += 1; /* Delimitador '|'. */
    if (r->nomePais) size += strlen(r->nomePais);
    size += 1; /* Delimitador '|'. */
    return size;
}

/* Lê o registro da posição atual. O removido vale '2' quando a leitura falha. */

RegistroPoPs lerRegistroPoPs(FILE *f) {
    RegistroPoPs r;
    r.nomePoPs = NULL;
    r.nomePais = NULL;
    r.removido = '2';
    
    if (!f) return r;
    if (fread(&r.removido, 1, 1, f) != 1) { r.removido = '2'; return r; }
    if (fread(&r.tamanhoRegistro, 4, 1, f) != 1) { r.removido = '2'; return r; }
    
    /* Removido: pula o resto. */
    if(r.removido == REMOVIDO) {
        fread(&r.encadeamentoLista, 8, 1, f);
        fread(&r.idPoPs, 4, 1, f);
        fread(&r.siglaPais, 2, 1, f);
        fseek(f, r.tamanhoRegistro - 19, SEEK_CUR);
        return r;
    }
    
    fread(&r.encadeamentoLista, 8, 1, f);
    fread(&r.idPoPs, 4, 1, f);
    fread(&r.siglaPais, 2, 1, f);
    
    /* Parte variável lida de uma vez e separada pelos '|'. String vazia = nulo. */
    int bytesRestantes = r.tamanhoRegistro - 19;
    if (bytesRestantes > 0) {
        char *buffer = malloc(bytesRestantes + 1);
        if (!buffer) return r;
        if (fread(buffer, 1, bytesRestantes, f) != (size_t)bytesRestantes) {
            free(buffer);
            return r;
        }
        buffer[bytesRestantes] = '\0';
        
        char *p = buffer;
        int lenPoPs = 0;
        while(p[lenPoPs] != '|' && p[lenPoPs] != '\0') lenPoPs++;
        if (lenPoPs > 0) {
            r.nomePoPs = malloc(lenPoPs + 1);
            if (r.nomePoPs) {
                strncpy(r.nomePoPs, p, lenPoPs);
                r.nomePoPs[lenPoPs] = '\0';
            }
        }
        
        p += lenPoPs + 1;
        int lenPais = 0;
        while(p[lenPais] != '|' && p[lenPais] != '\0') lenPais++;
        if (lenPais > 0) {
            r.nomePais = malloc(lenPais + 1);
            if (r.nomePais) {
                strncpy(r.nomePais, p, lenPais);
                r.nomePais[lenPais] = '\0';
            }
        }
        free(buffer);
    }
    return r;
}

/* Escreve o registro e completa com '$' até tamanhoRegistro. */

void escreverRegistroPoPs(FILE *f, RegistroPoPs *r) {
    if (!f || !r) return;
    fwrite(&r->removido, 1, 1, f);
    fwrite(&r->tamanhoRegistro, 4, 1, f);
    fwrite(&r->encadeamentoLista, 8, 1, f);
    fwrite(&r->idPoPs, 4, 1, f);
    fwrite(&r->siglaPais, 2, 1, f);
    
    if (r->removido == REMOVIDO) {
        int lixoSize = r->tamanhoRegistro - 19;
        if (lixoSize > 0) {
            char *lixo = malloc(lixoSize);
            if (lixo) {
                memset(lixo, LIXO, lixoSize);
                fwrite(lixo, 1, lixoSize, f);
                free(lixo);
            }
        }
        return;
    }
    
    int bytesVariaveis = 0;
    if (r->nomePoPs) {
        int len = strlen(r->nomePoPs);
        fwrite(r->nomePoPs, 1, len, f);
        bytesVariaveis += len;
    }
    char sep = '|';
    fwrite(&sep, 1, 1, f); bytesVariaveis++;
    
    if (r->nomePais) {
        int len = strlen(r->nomePais);
        fwrite(r->nomePais, 1, len, f);
        bytesVariaveis += len;
    }
    fwrite(&sep, 1, 1, f); bytesVariaveis++;
    
    /* A sobra vira lixo. */
    int lixoSize = r->tamanhoRegistro - (19 + bytesVariaveis);
    if(lixoSize > 0) {
        char *lixo = malloc(lixoSize);
        if (lixo) {
            memset(lixo, LIXO, lixoSize);
            fwrite(lixo, 1, lixoSize, f);
            free(lixo);
        }
    }
}

/* Posiciona o arquivo no byteOffset informado e lê o registro que está ali. */

RegistroPoPs lerRegistroPorOffset(FILE *f, int64_t offset) {
    RegistroPoPs r = {0};
    r.removido = '2';
    if (!f) return r;
    fseek(f, offset, SEEK_SET);
    return lerRegistroPoPs(f);
}

/* Posiciona o arquivo no byteOffset informado e escreve o registro ali. */

void escreverRegistroPorOffset(FILE *f, RegistroPoPs *r, int64_t offset) {
    if (!f || !r) return;
    fseek(f, offset, SEEK_SET);
    escreverRegistroPoPs(f, r);
}

/* Libera as strings alocadas do registro e as deixa como NULL. */

void liberarRegistroPoPs(RegistroPoPs *r) {
    if (!r) return;
    if (r->nomePoPs) { free(r->nomePoPs); r->nomePoPs = NULL; }
    if (r->nomePais) { free(r->nomePais); r->nomePais = NULL; }
}

/* Lixo nos campos de dados (remoção lógica). */

void preencherComLixoRegistroPoPs(RegistroPoPs *r) {
    if (!r) return;
    memset(&r->idPoPs, LIXO, sizeof(int));
    r->siglaPais[0] = LIXO; 
    r->siglaPais[1] = LIXO;
    if(r->nomePoPs) { free(r->nomePoPs); r->nomePoPs = NULL; }
    if(r->nomePais) { free(r->nomePais); r->nomePais = NULL; }
}
