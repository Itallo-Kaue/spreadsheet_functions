# Makefile
# Compilação do trabalho de Estrutura de Dados III.
# Os arquivos .c ficam em src/ e os .h em include/.

CC     = gcc
CFLAGS = -std=c99 -Wall -Wextra -Iinclude
EXEC   = programaTrab

# A diretiva all contém apenas o comando de compilação e a diretiva run
# apenas o comando de execução, conforme exigido pela especificação.
all:
	$(CC) $(CFLAGS) -o $(EXEC) src/*.c

run:
	./$(EXEC)

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all ./$(EXEC)

# Remove os arquivos gerados pela compilação.
clean:
	rm -f $(EXEC)

.PHONY: all run valgrind clean
