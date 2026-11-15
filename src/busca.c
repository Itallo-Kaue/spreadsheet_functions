/*
    busca.c
    Implementação da leitura dos critérios de busca e da varredura sequencial
    do arquivo de dados. A comparação é feita campo a campo sobre o registro já
    lido em memória, e um registro só é considerado resposta quando satisfaz
    todos os critérios informados na entrada.
*/

#include "busca.h"
#include "fornecidas.h"
#include "registro.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------------
    Leitura dos critérios
   ------------------------------------------------------------------ */

/* Campos numéricos são lidos com %s, porque o scan_quote_string junta um
   dígito sozinho com a próxima palavra da entrada. NULO vira string vazia. */

void LerValorNumerico(char *destino) {
    if (scanf("%63s", destino) != 1) {
        destino[0] = '\0';
        return;
    }
    if (strcmp(destino, "NULO") == 0) destino[0] = '\0';
}

/* A unidadeMedida vem entre aspas, o resto é número. */

void LerValorDoCampo(const char *nomeCampo, char *destino) {
    if (strcmp(nomeCampo, "unidadeMedida") == 0) ScanQuoteString(destino);
    else                                         LerValorNumerico(destino);
}

/* Lê os n pares nomeCampo valor. */

Criterio *lerCriterios(int n) {
    if (n <= 0) return NULL;
    Criterio *crit = (Criterio *)malloc((size_t)n * sizeof(Criterio));
    if (crit == NULL) return NULL;
    for (int i = 0; i < n; i++) {
        if (scanf("%31s", crit[i].nomeCampo) != 1) {
            free(crit);
            return NULL;
        }
        LerValorDoCampo(crit[i].nomeCampo, crit[i].valor);
    }
    return crit;
}

/* ------------------------------------------------------------------
    Comparação de um registro com os critérios
   ------------------------------------------------------------------ */

/* Compara um critério com o registro, retorna 1 se bate. */

static int campoCasa(Registro *reg, Criterio *c) {
    int nulo = (c->valor[0] == '\0');
    if (strcmp(c->nomeCampo, "idPoPs") == 0) {
        /* Campo obrigatório: nunca é nulo no arquivo. */
        if (nulo) return 0;
        return reg->idPoPs == atoi(c->valor);
    }
    if (strcmp(c->nomeCampo, "idPoPsConectado") == 0) {
        if (nulo) return 0;
        return reg->idPoPsConectado == atoi(c->valor);
    }
    if (strcmp(c->nomeCampo, "velocidade") == 0) {
        if (nulo) return reg->velocidade == INT_NULO;
        return reg->velocidade == atoi(c->valor);
    }
    if (strcmp(c->nomeCampo, "unidadeMedida") == 0) {
        if (nulo) return reg->unidadeMedida == LIXO;
        return reg->unidadeMedida == c->valor[0];
    }
    return 0;
}

int registroSatisfazCriterios(Registro *reg, Criterio *crit, int n) {
    if (reg->removido == REMOVIDO) return 0;
    for (int i = 0; i < n; i++) {
        if (!campoCasa(reg, &crit[i])) return 0;
    }
    return 1;
}

/* ------------------------------------------------------------------
    Atualização de um campo
   ------------------------------------------------------------------ */

/* Atualiza o campo no registro em memória. idPoPs e idPoPsConectado
   não podem ser nulos, então NULO é ignorado pra eles. */

void AtualizarCampo(Registro *reg, Criterio *c) {
    int nulo = (c->valor[0] == '\0');
    if (strcmp(c->nomeCampo, "idPoPs") == 0) {
        if (!nulo) reg->idPoPs = atoi(c->valor);
        return;
    }
    if (strcmp(c->nomeCampo, "idPoPsConectado") == 0) {
        if (!nulo) reg->idPoPsConectado = atoi(c->valor);
        return;
    }
    if (strcmp(c->nomeCampo, "velocidade") == 0) {
        reg->velocidade = nulo ? INT_NULO : atoi(c->valor);
        return;
    }
    if (strcmp(c->nomeCampo, "unidadeMedida") == 0) {
        reg->unidadeMedida = nulo ? LIXO : c->valor[0];
        return;
    }
}

/* ------------------------------------------------------------------
    Varredura do arquivo de dados
   ------------------------------------------------------------------ */

/* Pula o cabeçalho (17 bytes). */

void iniciarBusca(FILE *bin) {
    if (bin == NULL) return;
    fseek(bin, (long)TAM_CABECALHO, SEEK_SET);
}

/* Continua de onde parou e devolve o próximo registro que satisfaz os
   critérios, junto com o RRN. */

int buscarProximo(FILE *bin, Criterio *crit, int n, Registro *reg, int *rrn) {
    if (bin == NULL) return 0;
    while (1) {
        long posicao = ftell(bin);
        int rrnAtual = (int)((posicao - TAM_CABECALHO) / TAM_REGISTRO);
        if (!lerRegistro(bin, reg)) return 0;
        if (registroSatisfazCriterios(reg, crit, n)) {
            *rrn = rrnAtual;
            return 1;
        }
    }
}
