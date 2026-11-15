/*
    funcionalidade10.c
    Listagem de todos os registros não removidos do arquivo de dados dos PoPs.
    Corresponde ao comando SELECT da linguagem SQL, sem a cláusula WHERE.
*/

#include <stdio.h>
#include "funcionalidades.h"
#include "registroPoPs.h"
#include "utilPoPs.h"

/* Entrada: arquivo de dados. */

void funcionalidade10() {
    char arqBin[TAM_NOME_ARQUIVO];
    if (scanf("%63s", arqBin) != 1) return;
    FILE *f = fopen(arqBin, "rb");
    if(!f) { printf(MSG_FALHA); return; }
    CabecalhoPoPs c = lerCabecalhoPoPs(f);
    if(c.status == '0') { printf(MSG_FALHA); fclose(f); return; }
    
    /* Removido == '2' -> acabou o arquivo. */
    fseek(f, 25, SEEK_SET);
    int found = 0;
    while(ftell(f) < c.proxByteOffset) {
        RegistroPoPs r = lerRegistroPoPs(f);
        if(r.removido == '2') break;
        if(ImprimirRegistroPoPs(&r)) found++;
        liberarRegistroPoPs(&r);
    }
    if(!found) printf(MSG_INEXISTENTE);
    fclose(f);
}
