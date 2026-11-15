# Trabalhos Práticos — Estrutura de Dados III (SCC0607)

Manipulação de arquivos binários de dados sobre a topologia de rede da
Internet, com base no projeto Internet Topology Zoo. O projeto reúne os dois
trabalhos práticos da disciplina:

- **Trabalho 1** (funcionalidades 1 a 7): arquivo `conectaPoPs`, com registros
  de tamanho fixo, acesso direto por RRN e reaproveitamento de espaço por uma
  pilha de registros removidos.
- **Trabalho 2** (funcionalidades 8 a 14): arquivo `PoPs`, com registros de
  tamanho variável, endereçamento por byte offset, reaproveitamento de espaço
  pela estratégia *worst fit* e um índice primário sobre o campo `idPoPs`.

## Autores

| Nome                       | NUSP     |
| -------------------------- | -------- |
| Ítallo Kauê Barbosa Santos | 16839029 |
| Natália Yumi Watanabi      | 13725566 |

## Compilação e execução

```
make            # compila src/*.c e gera o executável programaTrab
make run        # executa o programa
make valgrind   # executa o programa sob o valgrind
make clean      # remove o executável
```

O programa lê tudo da entrada padrão. O primeiro valor é sempre o número da
funcionalidade, seguido dos parâmetros dela.

## Organização do código

```
include/   cabeçalhos (.h)
src/       implementação (.c)
```

| Módulo                                   | Responsabilidade                                                |
| ---------------------------------------- | --------------------------------------------------------------- |
| `main.c`                                 | lê o número da funcionalidade e chama a função correspondente   |
| `funcionalidade1.c` … `funcionalidade14.c` | uma funcionalidade por arquivo                                |
| `registro`, `pilha`, `busca`, `util`     | arquivo `conectaPoPs` (Trabalho 1)                              |
| `registroPoPs`, `listaRemovidos`, `buscaPoPs`, `utilPoPs` | arquivo `PoPs` (Trabalho 2)                    |
| `indicePrimario`                         | índice primário `indexaPoPs`                                    |
| `csvCampos`, `csv`, `csvPoPs`            | leitura dos arquivos CSV de entrada                             |
| `fornecidas`                             | funções disponibilizadas na disciplina (`binarioNaTela`, `scan_quote_string`) |

Toda a leitura e escrita dos arquivos binários é feita **campo a campo**, nunca
gravando a struct inteira de uma vez.

## Trabalho 1 — arquivo `conectaPoPs`

**Cabeçalho — 17 bytes**

| campo       | tipo | bytes | descrição                                  |
| ----------- | ---- | ----- | ------------------------------------------ |
| `status`    | char | 1     | `'0'` inconsistente, `'1'` consistente     |
| `topoPilha` | int  | 4     | RRN do topo da pilha de removidos, ou `-1` |
| `proxRRN`   | int  | 4     | próximo RRN disponível no fim do arquivo   |
| `nroRegRem` | int  | 4     | quantidade de registros removidos          |
| `nroPares`  | int  | 4     | quantidade de pares carregados do CSV      |

**Registro de dados — 18 bytes**

| campo               | tipo | bytes | descrição                        |
| ------------------- | ---- | ----- | -------------------------------- |
| `removido`          | char | 1     | `'0'` ou `'1'`                   |
| `encadeamentoPilha` | int  | 4     | RRN do próximo removido, ou `-1` |
| `idPoPs`            | int  | 4     | não pode ser nulo                |
| `idPoPsConectado`   | int  | 4     | não pode ser nulo                |
| `velocidade`        | int  | 4     | `-1` quando nulo                 |
| `unidadeMedida`     | char | 1     | `'$'` quando nulo                |

O byte offset de um registro é `17 + RRN * 18`, o que permite o acesso direto
da funcionalidade 4. A pilha de removidos fica dentro do próprio arquivo: o topo
é o campo `topoPilha` do cabeçalho e o encadeamento é o campo
`encadeamentoPilha` de cada registro removido.

## Trabalho 2 — arquivo `PoPs`

**Cabeçalho — 25 bytes**

| campo            | tipo    | bytes | descrição                                          |
| ---------------- | ------- | ----- | -------------------------------------------------- |
| `status`         | char    | 1     | `'0'` inconsistente, `'1'` consistente             |
| `topoLista`      | int64_t | 8     | byte offset do maior espaço removido, ou `-1`      |
| `proxByteOffset` | int64_t | 8     | próximo byte livre no fim do arquivo (começa em 25) |
| `nroRegRem`      | int     | 4     | quantidade de registros removidos                  |
| `nroPoPs`        | int     | 4     | quantidade de PoPs armazenados                     |

**Registro de dados — 19 bytes fixos + parte variável**

| campo               | tipo    | bytes    | descrição                                   |
| ------------------- | ------- | -------- | ------------------------------------------- |
| `removido`          | char    | 1        | `'0'` ou `'1'`                              |
| `tamanhoRegistro`   | int     | 4        | tamanho total do registro, em bytes         |
| `encadeamentoLista` | int64_t | 8        | byte offset do próximo removido, ou `-1`    |
| `idPoPs`            | int     | 4        | chave primária, não pode ser nulo           |
| `siglaPais`         | char[2] | 2        | `'$$'` quando nulo                          |
| `nomePoPs`          | string  | variável | terminada por `\|`; só o `\|` quando nula   |
| `nomePais`          | string  | variável | terminada por `\|`; só o `\|` quando nula   |

Os campos de 8 bytes usam `int64_t`, porque o tamanho de `long` varia entre
plataformas.

**Remoção e reaproveitamento (*worst fit*).** Um registro removido tem todos os
bytes depois dos campos de controle (`removido`, `tamanhoRegistro` e
`encadeamentoLista`) preenchidos com `'$'`. Os removidos formam uma lista
encadeada dentro do próprio arquivo, em ordem decrescente de tamanho: o topo é
sempre o maior espaço livre. Numa inserção, basta olhar o topo: se o novo
registro couber nele, o espaço é reaproveitado, o registro mantém o
`tamanhoRegistro` original e a sobra é preenchida com `'$'`; caso contrário, o
registro vai para o fim do arquivo.

**Atualização.** Se o registro atualizado couber no espaço que ocupava, ele é
regravado no mesmo lugar, com o lixo no fim. Se não couber, o antigo é removido
logicamente e o atualizado é inserido como um registro novo.

### Índice primário `indexaPoPs`

| parte      | conteúdo                                                   |
| ---------- | ---------------------------------------------------------- |
| cabeçalho  | `status` (1 byte)                                          |
| registros  | `idPoPs` (4 bytes) + `byteoffset` (8 bytes), 12 bytes cada |

As entradas ficam ordenadas por `idPoPs` em ordem crescente. Durante as
funcionalidades, o índice é carregado inteiro para a memória, consultado por
busca binária e gravado de volta no fim. As funcionalidades 11 a 14 recusam um
índice inexistente ou inconsistente antes de abrir o arquivo de dados.

## Funcionalidades

| #  | operação                                    | entrada                                                                   |
| -- | ------------------------------------------- | ------------------------------------------------------------------------- |
| 1  | carrega o CSV no arquivo `conectaPoPs`      | `1 arquivo.csv arquivo.bin`                                               |
| 2  | lista os registros não removidos            | `2 arquivo.bin`                                                           |
| 3  | busca por critérios                         | `3 arquivo.bin n` + n linhas `m campo valor ...`                          |
| 4  | busca por RRN                               | `4 arquivo.bin rrn`                                                       |
| 5  | remoção lógica                              | `5 arquivo.bin n` + n linhas `m campo valor ...`                          |
| 6  | inserção com reaproveitamento de espaço     | `6 arquivo.bin n` + n linhas com os 4 campos                              |
| 7  | atualização                                 | `7 arquivo.bin n` + n pares de linhas (busca / novos valores)             |
| 8  | carrega o CSV no arquivo `PoPs`             | `8 arquivo.csv arquivo.bin`                                               |
| 9  | cria o índice primário                      | `9 arquivo.bin indice.bin`                                                |
| 10 | lista os registros não removidos            | `10 arquivo.bin`                                                          |
| 11 | busca por critérios                         | `11 arquivo.bin indice.bin n` + n linhas `m campo valor ...`              |
| 12 | remoção lógica                              | `12 arquivo.bin indice.bin n` + n linhas `m campo valor ...`              |
| 13 | inserção com reaproveitamento de espaço     | `13 arquivo.bin indice.bin n` + n linhas `idPoPs nomePoPs nomePais siglaPais` |
| 14 | atualização                                 | `14 arquivo.bin indice.bin n` + n pares de linhas (busca / novos valores) |

- Campos de busca do Trabalho 1: `idPoPs`, `idPoPsConectado`, `velocidade` e
  `unidadeMedida`.
- Campos de busca do Trabalho 2: `idPoPs`, `nomePoPs`, `nomePais` e
  `siglaPais`. Quando o `idPoPs` está entre os critérios, a funcionalidade 11
  usa o índice; nos outros casos faz uma busca sequencial.
- Valores de texto são escritos entre aspas na entrada, e um campo nulo é
  informado como `NULO`.
- As funcionalidades 10 e 11 imprimem `idPoPs nomePoPs nomePais siglaPais`,
  sem aspas, com `NULO` nos campos nulos. As buscas (3 e 11) separam os
  resultados de cada busca com uma linha em branco.
- As funcionalidades que criam ou alteram arquivos terminam chamando
  `binarioNaTela`; as funcionalidades 12, 13 e 14 o fazem para o arquivo de
  dados e para o índice.

Mensagens de erro: `Falha no processamento do arquivo.` quando um arquivo não
pode ser processado, e `Registro inexistente.` quando uma busca não encontra
nada.

## Arquivos CSV de entrada

- `conectaPoPs.csv`: `idPoPs, idPoPsConectado, velocidade, unidadeMedida`.
- `PoPs.csv`: tem uma **coluna de índice no início**, que é ignorada, seguida de
  `idPoPs, nomePoPs, nomePais, siglaPais`. Alguns nomes terminam com espaço, e
  esse espaço faz parte do dado.

Nos dois arquivos, a primeira linha é o cabeçalho, e um campo vazio ou formado
só por espaços representa um valor nulo.

## Testes

A pasta `testes/` contém os casos de teste abertos do Trabalho 2 (`N.in` e
`N.out`).
