/*
    funcionalidades.h
    Declaração das funcionalidades do programa. Cada uma corresponde a uma
    opção lida no início da entrada pelo main e é responsável por ler os
    próprios parâmetros da entrada padrão e por imprimir a sua saída.

    Cada funcionalidade é implementada em um arquivo .c próprio
    (funcionalidade1.c a funcionalidade14.c), de modo que este cabeçalho é o
    único ponto em comum entre elas e o main. As constantes abaixo são as
    mensagens e os limites compartilhados por todas.
*/
#ifndef FUNCIONALIDADES_H
#define FUNCIONALIDADES_H
#include <stdio.h>
#include <stdlib.h>
/* Mensagem exibida quando o arquivo de dados não pode ser processado. */
#define MSG_FALHA "Falha no processamento do arquivo.\n"
/* Mensagem exibida quando uma consulta não devolve nenhum registro. */
#define MSG_INEXISTENTE "Registro inexistente.\n"
/* Tamanho máximo do nome de um arquivo lido da entrada padrão. */
#define TAM_NOME_ARQUIVO 64

/* 1º Cria o arquivo de dados a partir do CSV de entrada.
   Entrada: nome do arquivo CSV e nome do arquivo binário a ser gerado. */

void Funcionalidade_1();

/* 2º Lista todos os registros não removidos do arquivo de dados.
   Entrada: nome do arquivo de dados. */

void Funcionalidade_2();

/* 3º Recupera os registros que satisfazem um conjunto de critérios.
   Entrada: nome do arquivo, quantidade n de buscas e, para cada busca, a
   quantidade de campos seguida dos pares "nomeCampo" e "valor". */

void Funcionalidade_3();

/* 4º Recupera um único registro a partir do seu RRN, por acesso direto.
   Entrada: nome do arquivo de dados e o RRN desejado. */

void Funcionalidade_4();

/* 5º Remove logicamente os registros que satisfazem os critérios,
   inserindo-os na pilha de registros removidos.
   Entrada: nome do arquivo, quantidade n de remoções e, para cada remoção, a
   quantidade de campos seguida dos pares "nomeCampo" e "valor". */

void Funcionalidade_5();

/* 6º Insere novos registros, reaproveitando o espaço dos removidos.
   Entrada: nome do arquivo, quantidade n de inserções e, para cada uma, os
   quatro campos do registro na ordem da especificação. */

void Funcionalidade_6();

/* 7º Atualiza os registros que satisfazem os critérios de busca.
   Entrada: nome do arquivo, quantidade n de atualizações e, para cada uma,
   uma linha com os campos de busca e outra com os campos a alterar. */

void Funcionalidade_7();

/* 8º Cria o arquivo de dados dos PoPs a partir do CSV de entrada.
   Entrada: nome do arquivo CSV e nome do arquivo binário a ser gerado. */

void funcionalidade8();

/* 9º Cria o arquivo de índice primário sobre o campo idPoPs.
   Entrada: nome do arquivo de dados e nome do arquivo de índice. */

void funcionalidade9();

/* 10º Lista todos os registros não removidos do arquivo de dados dos PoPs.
   Entrada: nome do arquivo de dados. */

void funcionalidade10();

/* 11º Recupera os registros que satisfazem um conjunto de critérios, usando o
   índice quando a busca inclui o idPoPs.
   Entrada: nome do arquivo de dados, nome do índice, quantidade n de buscas e,
   para cada busca, a quantidade de campos seguida dos pares "nomeCampo" e
   "valor". */

void funcionalidade11();

/* 12º Remove logicamente os registros que satisfazem os critérios, inserindo-os
   na lista de registros removidos e retirando-os do índice.
   Entrada: nome do arquivo de dados, nome do índice, quantidade n de remoções e,
   para cada remoção, a quantidade de campos seguida dos pares "nomeCampo" e
   "valor". */

void funcionalidade12();

/* 13º Insere novos registros, reaproveitando o espaço dos removidos e
   atualizando o índice.
   Entrada: nome do arquivo de dados, nome do índice, quantidade n de inserções
   e, para cada uma, os quatro campos do registro na ordem da especificação. */

void funcionalidade13();

/* 14º Atualiza os registros que satisfazem os critérios de busca, mantendo o
   índice coerente.
   Entrada: nome do arquivo de dados, nome do índice, quantidade n de
   atualizações e, para cada uma, os campos de busca e os campos a alterar. */

void funcionalidade14();
#endif
