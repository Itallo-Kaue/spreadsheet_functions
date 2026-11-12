/*
    csv.c
    Implementação da carga inicial do arquivo de dados a partir do CSV.
    Cada linha do CSV vira um registro de tamanho fixo escrito campo a
    campo no arquivo binário, na ordem em que aparece na entrada.
*/

#include "csv.h"
#include "registro.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Quantidade de campos de uma linha do CSV: idPoPs, idPoPsConectado,
   velocidade e unidadeMedida, nesta ordem. */

#define NUM_CAMPOS 4

#define TAM_LINHA 1024

/* ------------------------------------------------------------------
    Quebra de uma linha do CSV em campos
   ------------------------------------------------------------------ */

/* Descarta os espaços em branco do início e do fim do campo e devolve um
   ponteiro para o primeiro caractere útil. Isso resolve dois casos de uma vez:
   a especificação representa um campo nulo por espaço em branco, que aqui vira
   uma string vazia; e a quebra de linha ('\r' e '\n') que acompanha o último
   campo da linha é descartada junto, inclusive no padrão Windows (CRLF). */

static char *removerEspacos(char *campo) {
    while (isspace((unsigned char)*campo)) campo++;

    size_t fim = strlen(campo);
    while (fim > 0 && isspace((unsigned char)campo[fim - 1])) {
        campo[fim - 1] = '\0';
        fim--;
    }

    return campo;
}

/* Separa a linha nos seus campos, guardando em campos[] um ponteiro para cada
   um deles. A separação é feita manualmente, e não com strtok, porque o strtok
   trata delimitadores consecutivos como um só: em uma linha como "3,15,,M" ele
   devolveria "M" como terceiro campo, e não o campo vazio que está ali. Como o
   campo vazio é justamente a representação de um valor nulo, cada delimitador
   precisa encerrar exatamente um campo.
   Os campos que não aparecerem na linha ficam como NULL.
   Retorna a quantidade de campos efetivamente encontrados. */

static int separarCampos(char *linha, char *campos[], int maxCampos) {
    int qtd = 0;
    char *inicio = linha;
    char *p = linha;

    while (qtd < maxCampos) {
        if (*p == ',' || *p == ';' || *p == '\0') {
            int ultimo = (*p == '\0');

            *p = '\0';
            campos[qtd] = removerEspacos(inicio);
            qtd++;

            if (ultimo) break;
            inicio = p + 1;
        }
        p++;
    }

    for (int i = qtd; i < maxCampos; i++) campos[i] = NULL;

    return qtd;
}

/* Um campo é nulo quando está ausente na linha ou ficou vazio depois de
   descartados os espaços em branco. */

static int campoNulo(const char *campo) {
    return (campo == NULL || campo[0] == '\0');
}

/* ------------------------------------------------------------------
    Carga do arquivo de dados a partir do CSV
 '  ------------------------------------------------------------------ */

/* Percorre o CSV de entrada linha a linha e grava um registro de dados para
   cada uma delas. O arquivo binário é criado com o cabeçalho inicial (status
   '0') e só é marcado como consistente no fechamento. Os contadores proxRRN e
   nroPares do cabeçalho são atualizados a cada registro inserido.
   Retorna 1 em caso de sucesso e 0 em caso de falha de abertura. */

int criarTabela(const char *nomeCSV, const char *nomeBin) {
    FILE *csv = fopen(nomeCSV, "r");
    if (csv == NULL) return 0;

    /* Cria o arquivo de dados já com o cabeçalho inicial gravado. */

    Cabecalho cab;
    FILE *bin = criarArquivoBinario(nomeBin, &cab);
    if (bin == NULL) {
        fclose(csv);
        return 0;
    }

    char linha[TAM_LINHA];

    /* A primeira linha do CSV é o cabeçalho com os nomes dos campos e não
       corresponde a nenhum registro, portanto é apenas descartada. */

    fgets(linha, sizeof(linha), csv);

    while (fgets(linha, sizeof(linha), csv) != NULL) {
        char *campos[NUM_CAMPOS];
        separarCampos(linha, campos, NUM_CAMPOS);

        char *pops = campos[0];
        char *cone = campos[1];
        char *velo = campos[2];
        char *unid = campos[3];

        /* idPoPs e idPoPsConectado não podem ser nulos: uma linha sem esses
           campos é ignorada. */

        if (campoNulo(pops) || campoNulo(cone)) continue;

        /* Cada campo do registro recebe o seu valor direto da linha do CSV.
           Os campos opcionais recebem o valor nulo (-1 para o inteiro e '$'
           para a unidade de medida) quando vêm nulos no CSV. */

        Registro reg;

        reg.removido = NAO_REMOVIDO;
        reg.encadeamentoPilha = -1;

        reg.idPoPs = atoi(pops);
        reg.idPoPsConectado = atoi(cone);

        reg.velocidade = campoNulo(velo) ? INT_NULO : atoi(velo);
        reg.unidadeMedida = campoNulo(unid) ? LIXO : unid[0];

        /* Esta funcionalidade apenas acrescenta registros no fim do arquivo,
           um atrás do outro. Como o fwrite já deixa o ponteiro no início do
           próximo registro, a escrita é sequencial e não precisa de fseek. */

        escreverRegistro(bin, &reg);
        cab.proxRRN++;
        cab.nroPares++;
    }

    fclose(csv);

    /* Regrava o cabeçalho com os contadores finais, marca o arquivo como
       consistente ('1') e o fecha. */

    fecharArquivoBinario(bin, &cab);

    return 1;
}
