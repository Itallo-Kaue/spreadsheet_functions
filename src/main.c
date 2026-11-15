/*  Ítallo Kauê Barbosa Santos   16839029
    Natália Yumi Watanabi        13725566
*/

/*
    main.c
    Ponto de entrada do programa. Lê da entrada padrão o identificador da
    funcionalidade a ser executada e chama a função correspondente, que é
    quem lê os próprios parâmetros e imprime a sua saída. Cada funcionalidade
    é implementada em um arquivo .c próprio.
*/

#include <stdio.h>
#include "funcionalidades.h"

int main() {
    int opcao;
    if (scanf("%d", &opcao) != 1) return 0;

    switch(opcao) {
        case 1: Funcionalidade_1(); break;
        case 2: Funcionalidade_2(); break;
        case 3: Funcionalidade_3(); break;
        case 4: Funcionalidade_4(); break;
        case 5: Funcionalidade_5(); break;
        case 6: Funcionalidade_6(); break;
        case 7: Funcionalidade_7(); break;
        /* As funcionalidades 8 a 14 trabalham sobre o arquivo de dados dos
           PoPs, com registros de tamanho variável e índice primário. */
        case 8: funcionalidade8(); break;
        case 9: funcionalidade9(); break;
        case 10: funcionalidade10(); break;
        case 11: funcionalidade11(); break;
        case 12: funcionalidade12(); break;
        case 13: funcionalidade13(); break;
        case 14: funcionalidade14(); break;
    }
    return 0;
}
