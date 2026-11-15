/*
    buscaPoPs.c
    Implementação da leitura dos critérios de busca e da varredura sequencial
    do arquivo de dados dos PoPs. A comparação é feita campo a campo sobre o
    registro já lido em memória, e um registro só é considerado resposta
    quando satisfaz todos os critérios informados na entrada.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buscaPoPs.h"
#include "fornecidas.h"

/* Campos de texto vêm entre aspas; idPoPs é lido com %s e NULO vira string vazia. */

void LerValorDoCampoPoPs(const char *nomeCampo, char *destino) {
    if (strcmp(nomeCampo, "nomePoPs") == 0 || strcmp(nomeCampo, "nomePais") == 0 || strcmp(nomeCampo, "siglaPais") == 0) {
        scan_quote_string(destino);
    } else {
        if (scanf("%63s", destino) != 1) destino[0] = '\0';
        if (strcmp(destino, "NULO") == 0) destino[0] = '\0';
    }
}

/* Lê os n pares nomeCampo valor. */

CriterioPoPs *lerCriteriosPoPs(int n) {
    if (n <= 0) return NULL;
    CriterioPoPs *crit = malloc(n * sizeof(CriterioPoPs));
    for (int i = 0; i < n; i++) {
        if (scanf("%31s", crit[i].nomeCampo) != 1) { free(crit); return NULL; }
        LerValorDoCampoPoPs(crit[i].nomeCampo, crit[i].valor);
    }
    return crit;
}

/* Valor vazio = NULO (idPoPs nunca é nulo). */

static int campoCasaPoPs(RegistroPoPs *reg, CriterioPoPs *c) {
    int nulo = (c->valor[0] == '\0');
    if (strcmp(c->nomeCampo, "idPoPs") == 0) {
        if (nulo) return 0;
        return reg->idPoPs == atoi(c->valor);
    }
    if (strcmp(c->nomeCampo, "siglaPais") == 0) {
        if (nulo) return reg->siglaPais[0] == LIXO;
        return strncmp(reg->siglaPais, c->valor, 2) == 0;
    }
    if (strcmp(c->nomeCampo, "nomePoPs") == 0) {
        if (nulo) return reg->nomePoPs == NULL;
        return reg->nomePoPs != NULL && strcmp(reg->nomePoPs, c->valor) == 0;
    }
    if (strcmp(c->nomeCampo, "nomePais") == 0) {
        if (nulo) return reg->nomePais == NULL;
        return reg->nomePais != NULL && strcmp(reg->nomePais, c->valor) == 0;
    }
    return 0;
}

int registroSatisfazCriteriosPoPs(RegistroPoPs *reg, CriterioPoPs *crit, int n) {
    if (reg->removido == REMOVIDO) return 0;
    for (int i = 0; i < n; i++) {
        if (!campoCasaPoPs(reg, &crit[i])) return 0;
    }
    return 1;
}

/* Altera o campo no registro em memória. NULO no idPoPs é ignorado. */

void AtualizarCampoPoPs(RegistroPoPs *reg, CriterioPoPs *c) {
    int nulo = (c->valor[0] == '\0');
    if (strcmp(c->nomeCampo, "idPoPs") == 0) {
        if (!nulo) reg->idPoPs = atoi(c->valor);
    } else if (strcmp(c->nomeCampo, "siglaPais") == 0) {
        if(nulo) { reg->siglaPais[0] = LIXO; reg->siglaPais[1] = LIXO; }
        else { 
            reg->siglaPais[0] = c->valor[0]; 
            reg->siglaPais[1] = (strlen(c->valor) > 1) ? c->valor[1] : LIXO; 
        }
    } else if (strcmp(c->nomeCampo, "nomePoPs") == 0) {
        if(reg->nomePoPs) { free(reg->nomePoPs); reg->nomePoPs = NULL; }
        reg->nomePoPs = nulo ? NULL : meu_strdup(c->valor);
    } else if (strcmp(c->nomeCampo, "nomePais") == 0) {
        if(reg->nomePais) { free(reg->nomePais); reg->nomePais = NULL; }
        reg->nomePais = nulo ? NULL : meu_strdup(c->valor);
    }
}

/* Pula o cabeçalho (25 bytes). */

void iniciarBuscaPoPs(FILE *bin) {
    if (bin) fseek(bin, 25, SEEK_SET);
}

/* Próximo registro que satisfaz os critérios, até proxByteOffset.
   O registro devolvido precisa ser liberado depois. */

int buscarProximoPoPs(FILE *bin, CriterioPoPs *crit, int n, RegistroPoPs *reg, int64_t *offset_atual, int64_t limite) {
    while (ftell(bin) < limite) {
        *offset_atual = ftell(bin);
        *reg = lerRegistroPoPs(bin);
        if (reg->removido == '2') return 0;
        
        if (registroSatisfazCriteriosPoPs(reg, crit, n)) {
            return 1;
        }
        liberarRegistroPoPs(reg);
    }
    return 0;
}
