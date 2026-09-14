#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Enumeração para representar as colunas do Bingo (Requisito Obrigatório)
enum Coluna {
    COL_B = 0,
    COL_I,
    COL_N,
    COL_G,
    COL_O
};

// Estrutura para representar uma cartela (Desafio 3)
typedef struct {
    int numeros[5][5];
} Cartela;

// Função para gerar um número aleatório dentro de um intervalo
int gerarNumero(int minimo, int maximo) {
    return minimo + rand() % (maximo - minimo + 1);
}

// Função para verificar se um número já existe na coluna (evita repetições)
int numeroExiste(int cartela[5][5], int coluna, int numero) {
    for (int i = 0; i < 5; i++) {
        if (cartela[i][coluna] == numero) {
            return 1; // Verdadeiro (já existe)
        }
    }
    return 0; // Falso (não existe)
}

// Função para ordenar os números de cada coluna em ordem crescente (Desafio 4)
void ordenarColuna(int cartela[5][5], int coluna) {
    for (int i = 0; i < 4; i++) {
        for (int j = i + 1; j < 5; j++) {
            if (cartela[i][coluna] > cartela[j][coluna]) {
                int temp = cartela[i][coluna];
                cartela[i][coluna] = cartela[j][coluna];
                cartela[j][coluna] = temp;
            }
        }
    }
}

// Função para gerar uma cartela respeitando as regras e faixas de valores
void gerarCartela(Cartela *c) {
    // Faixas de valores para cada coluna: B, I, N, G, O
    int min[] = {1, 16, 31, 46, 61};
    int max[] = {15, 30, 45, 60, 75};

    for (int col = 0; col < 5; col++) {
        for (int linha = 0; linha < 5; linha++) {
            // A posição central (linha 2, coluna N) é o espaço livre (0)
            if (linha == 2 && col == COL_N) {
                c->numeros[linha][col] = 0;
                continue;
            }

            int num;
            do {
                num = gerarNumero(min[col], max[col]);
            } while (numeroExiste(c->numeros, col, num));

            c->numeros[linha][col] = num;
        }
        // Ordena os números da coluna gerada
        ordenarColuna(c->numeros, col);
    }
}

// Função para exibir a cartela de forma organizada no terminal
void imprimirCartela(Cartela c, int numeroCartela) {
    printf("=================================\n");
    printf("           CARTELA %d            \n", numeroCartela);
    printf("=================================\n");
    printf("  B     I     N     G     O    \n");
    printf("---------------------------------\n");

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            // Se for o centro da cartela (linha 2, coluna N), exibe 'X'
            if (i == 2 && j == COL_N) {
                printf("  X   ");
            } else {
                printf("%3d   ", c.numeros[i][j]); // Corrigido de c-> para c.
            }
        }
        printf("\n\n");
    }
    printf("=================================\n\n");
}

int main() {
    // Inicializa a semente geradora de números aleatórios com o tempo atual
    srand(time(NULL));

    int quantidade;
    printf("Informe quantas cartelas deseja gerar: ");
    if (scanf("%d", &quantidade) != 1 || quantidade <= 0) {
        printf("Quantidade inválida!\n");
        return 1;
    }

    // Aloca dinamicamente o vetor de cartelas
    Cartela *cartelas = (Cartela *)malloc(quantidade * sizeof(Cartela));
    if (cartelas == NULL) {
        printf("Erro de alocação de memória!\n");
        return 1;
    }

    // Gera e imprime cada cartela solicitada
    for (int i = 0; i < quantidade; i++) {
        gerarCartela(&cartelas[i]);
        imprimirCartela(cartelas[i], i + 1);
    }

    // Libera a memória alocada
    free(cartelas);

    return 0;
}