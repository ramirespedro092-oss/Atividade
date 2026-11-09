/*
 * ============================================================================
 *  Sistema de Caixa Eletronico em Linguagem C
 * ----------------------------------------------------------------------------
 *  Cadastro de contas correntes, movimentacoes financeiras e armazenamento
 *  permanente dos dados em um arquivo binario.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARQUIVO "contas.dat"

/* ------------------------- Estruturas de dados --------------------------- */

typedef enum
{
    CONTA_ENCERRADA,
    CONTA_ATIVA
} SituacaoConta;

typedef struct
{
    long          numero;
    char          titular[100];
    char          cpf[15];
    double        saldo;
    SituacaoConta situacao;
} Conta;

/* ------------------------- Prototipos das funcoes ------------------------ */

void cadastrarConta(void);
void consultarConta(void);
int  depositar(long numeroConta, double valor);
int  sacar(long numeroConta, double valor);
int  transferir(long contaOrigem, long contaDestino, double valor);
void listarContas(void);
int  encerrarConta(long numeroConta);

int  buscarConta(FILE *arquivo, long numeroConta, Conta *conta);
int  atualizarConta(FILE *arquivo, const Conta *conta);
int  numeroExiste(long numeroConta);

void menuDeposito(void);
void menuSaque(void);
void menuTransferencia(void);
void menuEncerramento(void);
void limparBuffer(void);

/* ============================== Programa ================================= */

int main(void)
{
    int opcao;

    do
    {
        printf("\n========================================\n");
        printf("        CAIXA ELETRONICO - MENU\n");
        printf("========================================\n");
        printf("1 - Cadastrar conta\n");
        printf("2 - Consultar conta\n");
        printf("3 - Realizar deposito\n");
        printf("4 - Realizar saque\n");
        printf("5 - Realizar transferencia\n");
        printf("6 - Listar contas\n");
        printf("7 - Encerrar conta\n");
        printf("0 - Sair\n");
        printf("----------------------------------------\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1)
        {
            limparBuffer();
            printf("\nOpcao invalida!\n");
            continue;
        }
        limparBuffer();

        switch (opcao)
        {
            case 1: cadastrarConta();      break;
            case 2: consultarConta();      break;
            case 3: menuDeposito();        break;
            case 4: menuSaque();           break;
            case 5: menuTransferencia();   break;
            case 6: listarContas();        break;
            case 7: menuEncerramento();    break;
            case 0: printf("\nEncerrando o sistema. Ate logo!\n"); break;
            default: printf("\nOpcao invalida! Tente novamente.\n");
        }

    } while (opcao != 0);

    return 0;
}

/* ======================= Funcoes utilitarias ============================ */

void limparBuffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

/* ----------------------- Manipulacao de arquivos ------------------------ */

int buscarConta(FILE *arquivo, long numeroConta, Conta *conta)
{
    Conta temp;

    rewind(arquivo);
    while (fread(&temp, sizeof(Conta), 1, arquivo) == 1)
    {
        if (temp.numero == numeroConta)
        {
            *conta = temp;
            return 1;
        }
    }
    return 0;
}

int atualizarConta(FILE *arquivo, const Conta *conta)
{
    Conta temp;
    long  pos = 0;

    rewind(arquivo);
    while (fread(&temp, sizeof(Conta), 1, arquivo) == 1)
    {
        if (temp.numero == conta->numero)
        {
            if (fseek(arquivo, pos * (long)sizeof(Conta), SEEK_SET) != 0)
                return 0;

            if (fwrite(conta, sizeof(Conta), 1, arquivo) != 1)
                return 0;

            fflush(arquivo);
            return 1;
        }
        pos++;
    }
    return 0;
}

int numeroExiste(long numeroConta)
{
    FILE  *arquivo;
    Conta  temp;
    int    existe = 0;

    arquivo = fopen(ARQUIVO, "rb");
    if (arquivo == NULL)
        return 0;   

    while (fread(&temp, sizeof(Conta), 1, arquivo) == 1)
    {
        if (temp.numero == numeroConta)
        {
            existe = 1;
            break;
        }
    }

    fclose(arquivo);
    return existe;
}

/* ============================ Cadastro =================================== */

void cadastrarConta(void)
{
    FILE  *arquivo;
    Conta  nova;
    long   numero;

    printf("\n--- Cadastro de Conta ---\n");
    printf("Numero da conta: ");
    if (scanf("%ld", &numero) != 1)
    {
        limparBuffer();
        printf("Numero invalido!\n");
        return;
    }
    limparBuffer();

    if (numeroExiste(numero))
    {
        printf("Erro: ja existe uma conta com o numero %ld.\n", numero);
        return;
    }

    nova.numero = numero;

    printf("Nome do titular: ");
    if (fgets(nova.titular, sizeof(nova.titular), stdin) != NULL)
        nova.titular[strcspn(nova.titular, "\n")] = '\0';
    else
        nova.titular[0] = '\0';

    printf("CPF: ");
    if (fgets(nova.cpf, sizeof(nova.cpf), stdin) != NULL)
        nova.cpf[strcspn(nova.cpf, "\n")] = '\0';
    else
        nova.cpf[0] = '\0';

    nova.saldo    = 0.0;
    nova.situacao = CONTA_ATIVA;

    arquivo = fopen(ARQUIVO, "ab");
    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo para gravacao.\n");
        return;
    }

    if (fwrite(&nova, sizeof(Conta), 1, arquivo) != 1)
        printf("Erro ao gravar a conta no arquivo.\n");
    else
        printf("Conta %ld cadastrada com sucesso!\n", nova.numero);

    fclose(arquivo);
}

/* ============================ Consulta ================================== */

void consultarConta(void)
{
    FILE  *arquivo;
    Conta  conta;
    long   numero;

    printf("\n--- Consulta de Conta ---\n");
    printf("Numero da conta: ");
    if (scanf("%ld", &numero) != 1)
    {
        limparBuffer();
        printf("Numero invalido!\n");
        return;
    }
    limparBuffer();

    arquivo = fopen(ARQUIVO, "rb");
    if (arquivo == NULL)
    {
        printf("Nenhuma conta cadastrada (arquivo inexistente).\n");
        return;
    }

    if (buscarConta(arquivo, numero, &conta))
    {
        printf("\nNumero da conta : %ld\n", conta.numero);
        printf("Titular         : %s\n", conta.titular);
        printf("CPF             : %s\n", conta.cpf);
        printf("Saldo atual     : R$ %.2f\n", conta.saldo);
        printf("Situacao        : %s\n",
               conta.situacao == CONTA_ATIVA ? "ATIVA" : "ENCERRADA");
    }
    else
    {
        printf("Conta %ld nao encontrada.\n", numero);
    }

    fclose(arquivo);
}

/* ============================ Deposito ================================== */

int depositar(long numeroConta, double valor)
{
    FILE  *arquivo;
    Conta  conta;

    if (valor <= 0)
    {
        printf("Erro: o valor do deposito deve ser maior que zero.\n");
        return 0;
    }

    arquivo = fopen(ARQUIVO, "rb+");
    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo.\n");
        return 0;
    }

    if (!buscarConta(arquivo, numeroConta, &conta))
    {
        printf("Erro: conta %ld nao encontrada.\n", numeroConta);
        fclose(arquivo);
        return 0;
    }

    if (conta.situacao != CONTA_ATIVA)
    {
        printf("Erro: a conta %ld esta encerrada.\n", numeroConta);
        fclose(arquivo);
        return 0;
    }

    conta.saldo += valor;

    if (!atualizarConta(arquivo, &conta))
    {
        printf("Erro ao atualizar o saldo no arquivo.\n");
        fclose(arquivo);
        return 0;
    }

    fclose(arquivo);
    return 1;
}

void menuDeposito(void)
{
    long   numero;
    double valor;

    printf("\n--- Deposito ---\n");
    printf("Numero da conta: ");
    if (scanf("%ld", &numero) != 1) { limparBuffer(); printf("Entrada invalida!\n"); return; }
    printf("Valor do deposito: ");
    if (scanf("%lf", &valor) != 1)  { limparBuffer(); printf("Entrada invalida!\n"); return; }
    limparBuffer();

    if (depositar(numero, valor))
        printf("Deposito de R$ %.2f realizado com sucesso na conta %ld.\n", valor, numero);
}

/* ============================== Saque =================================== */

int sacar(long numeroConta, double valor)
{
    FILE  *arquivo;
    Conta  conta;

    if (valor <= 0)
    {
        printf("Erro: o valor do saque deve ser maior que zero.\n");
        return 0;
    }

    arquivo = fopen(ARQUIVO, "rb+");
    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo.\n");
        return 0;
    }

    if (!buscarConta(arquivo, numeroConta, &conta))
    {
        printf("Erro: conta %ld nao encontrada.\n", numeroConta);
        fclose(arquivo);
        return 0;
    }

    if (conta.situacao != CONTA_ATIVA)
    {
        printf("Erro: a conta %ld esta encerrada.\n", numeroConta);
        fclose(arquivo);
        return 0;
    }

    if (conta.saldo < valor)
    {
        printf("Erro: saldo insuficiente (saldo atual: R$ %.2f).\n", conta.saldo);
        fclose(arquivo);
        return 0;
    }

    conta.saldo -= valor;

    if (!atualizarConta(arquivo, &conta))
    {
        printf("Erro ao atualizar o saldo no arquivo.\n");
        fclose(arquivo);
        return 0;
    }

    fclose(arquivo);
    return 1;
}

void menuSaque(void)
{
    long   numero;
    double valor;

    printf("\n--- Saque ---\n");
    printf("Numero da conta: ");
    if (scanf("%ld", &numero) != 1) { limparBuffer(); printf("Entrada invalida!\n"); return; }
    printf("Valor do saque: ");
    if (scanf("%lf", &valor) != 1)  { limparBuffer(); printf("Entrada invalida!\n"); return; }
    limparBuffer();

    if (sacar(numero, valor))
        printf("Saque de R$ %.2f realizado com sucesso na conta %ld.\n", valor, numero);
}

/* =========================== Transferencia ============================== */

int transferir(long contaOrigem, long contaDestino, double valor)
{
    FILE  *arquivo;
    Conta  origem, destino;

    if (contaOrigem == contaDestino)
    {
        printf("Erro: a conta de origem e destino devem ser diferentes.\n");
        return 0;
    }

    if (valor <= 0)
    {
        printf("Erro: o valor da transferencia deve ser maior que zero.\n");
        return 0;
    }

    arquivo = fopen(ARQUIVO, "rb+");
    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo.\n");
        return 0;
    }

    if (!buscarConta(arquivo, contaOrigem, &origem))
    {
        printf("Erro: conta de origem %ld nao encontrada.\n", contaOrigem);
        fclose(arquivo);
        return 0;
    }
    if (!buscarConta(arquivo, contaDestino, &destino))
    {
        printf("Erro: conta de destino %ld nao encontrada.\n", contaDestino);
        fclose(arquivo);
        return 0;
    }
    if (origem.situacao != CONTA_ATIVA)
    {
        printf("Erro: a conta de origem %ld esta encerrada.\n", contaOrigem);
        fclose(arquivo);
        return 0;
    }
    if (destino.situacao != CONTA_ATIVA)
    {
        printf("Erro: a conta de destino %ld esta encerrada.\n", contaDestino);
        fclose(arquivo);
        return 0;
    }
    if (origem.saldo < valor)
    {
        printf("Erro: saldo insuficiente na conta de origem (R$ %.2f).\n", origem.saldo);
        fclose(arquivo);
        return 0;
    }

    fclose(arquivo);

    if (!sacar(contaOrigem, valor))
    {
        printf("Erro: a transferencia foi cancelada (falha no saque).\n");
        return 0;
    }

    if (!depositar(contaDestino, valor))
    {
        depositar(contaOrigem, valor);
        printf("Erro: a transferencia foi cancelada (falha no deposito). Saldo restaurado.\n");
        return 0;
    }

    return 1;
}

void menuTransferencia(void)
{
    long   origem, destino;
    double valor;

    printf("\n--- Transferencia ---\n");
    printf("Conta de origem: ");
    if (scanf("%ld", &origem) != 1)  { limparBuffer(); printf("Entrada invalida!\n"); return; }
    printf("Conta de destino: ");
    if (scanf("%ld", &destino) != 1) { limparBuffer(); printf("Entrada invalida!\n"); return; }
    printf("Valor da transferencia: ");
    if (scanf("%lf", &valor) != 1)   { limparBuffer(); printf("Entrada invalida!\n"); return; }
    limparBuffer();

    if (transferir(origem, destino, valor))
        printf("Transferencia de R$ %.2f realizada de %ld para %ld.\n", valor, origem, destino);
}

/* ============================ Listagem ================================== */

void listarContas(void)
{
    FILE  *arquivo;
    Conta  conta;
    int    filtro;
    int    total = 0;

    printf("\n--- Listagem de Contas ---\n");
    printf("1 - Todas as contas\n");
    printf("2 - Somente contas ativas\n");
    printf("3 - Somente contas encerradas\n");
    printf("Escolha o filtro: ");
    if (scanf("%d", &filtro) != 1)
    {
        limparBuffer();
        printf("Filtro invalido!\n");
        return;
    }
    limparBuffer();

    arquivo = fopen(ARQUIVO, "rb");
    if (arquivo == NULL)
    {
        printf("Nenhuma conta cadastrada (arquivo inexistente).\n");
        return;
    }

    printf("\n%-10s %-30s %-16s %-12s %-10s\n",
           "NUMERO", "TITULAR", "CPF", "SALDO", "SITUACAO");
    printf("---------------------------------------------------------------------------------\n");

    while (fread(&conta, sizeof(Conta), 1, arquivo) == 1)
    {
        if (filtro == 2 && conta.situacao != CONTA_ATIVA)    continue;
        if (filtro == 3 && conta.situacao != CONTA_ENCERRADA) continue;

        printf("%-10ld %-30s %-16s R$ %-9.2f %-10s\n",
               conta.numero,
               conta.titular,
               conta.cpf,
               conta.saldo,
               conta.situacao == CONTA_ATIVA ? "ATIVA" : "ENCERRADA");
        total++;
    }

    if (total == 0)
        printf("Nenhuma conta encontrada para o filtro selecionado.\n");
    else
        printf("\nTotal de contas listadas: %d\n", total);

    fclose(arquivo);
}

/* =========================== Encerramento =============================== */

int encerrarConta(long numeroConta)
{
    FILE  *arquivo;
    Conta  conta;

    arquivo = fopen(ARQUIVO, "rb+");
    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo.\n");
        return 0;
    }

    if (!buscarConta(arquivo, numeroConta, &conta))
    {
        printf("Erro: conta %ld nao encontrada.\n", numeroConta);
        fclose(arquivo);
        return 0;
    }

    if (conta.situacao == CONTA_ENCERRADA)
    {
        printf("Erro: a conta %ld ja esta encerrada.\n", numeroConta);
        fclose(arquivo);
        return 0;
    }

    if (conta.saldo != 0.0)
    {
        printf("Erro: so e possivel encerrar contas com saldo igual a zero "
               "(saldo atual: R$ %.2f).\n", conta.saldo);
        fclose(arquivo);
        return 0;
    }

    conta.situacao = CONTA_ENCERRADA;

    if (!atualizarConta(arquivo, &conta))
    {
        printf("Erro ao atualizar a conta no arquivo.\n");
        fclose(arquivo);
        return 0;
    }

    fclose(arquivo);
    return 1;
}

void menuEncerramento(void)
{
    long numero;

    printf("\n--- Encerramento de Conta ---\n");
    printf("Numero da conta: ");
    if (scanf("%ld", &numero) != 1)
    {
        limparBuffer();
        printf("Entrada invalida!\n");
        return;
    }
    limparBuffer();

    if (encerrarConta(numero))
        printf("Conta %ld encerrada com sucesso.\n", numero);
}