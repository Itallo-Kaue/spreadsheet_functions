/*
    csvPoPs.h
    Leitura do arquivo CSV dos PoPs e carga dos seus registros no arquivo de
    dados binário.
*/

#ifndef CSVPOPS_H
#define CSVPOPS_H

/* Lê o arquivo CSV informado e grava cada uma de suas linhas como um registro
   no arquivo binário de saída, que é criado do zero. Retorna 1 em caso de
   sucesso e 0 em caso de falha. */

int criarTabelaPoPs(const char *nomeCSV, const char *nomeBin);
#endif
