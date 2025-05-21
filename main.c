#include <stdio.h>
#include <stdlib.h>
#include "livros.h"

//Proposito - Exibe o menu de opções para o usuário.
//Pré-Condições - Nenhuma.
//Pós-Condições - O menu é exibido no console.
void exibir_menu() {
    printf("=========================================\n");
    printf("       Bem-vindo ao Sistema de Gestão    \n");
    printf("=========================================\n");
    printf(" Escolha a opção desejada: \n");
    printf("  [1] - Cadastrar novo livro\n");
    printf("  [2] - Remover livro\n");
    printf("  [3] - Imprimir dados do livro por código\n");
    printf("  [4] - Listar todos os livros em ordem\n");
    printf("  [5] - Imprimir árvore por níveis\n");
    printf("  [6] - Imprimir árvore formatada como lista\n");
    printf("  [7] - Busca por autor\n");
    printf("  [8] - Busca por título\n");
    printf("  [9] - Calcular total de livros cadastrados\n");
    printf("  [10] - Carregar livros de um arquivo texto\n");
    printf("  [11] - Imprimir lista de registros livres\n");
    printf("  [0] - Sair do Programa\n");
    printf("=========================================\n");
    printf("Digite o número da opção desejada: ");
}

//Proposito - Função principal do sistema de gestão de livros.
//Pré-Condições - O arquivo "livros.bin" deve ser acessível.
//Pós-Condições - O programa termina e o arquivo é fechado.
int main() {
    FILE *arq_bin = fopen("livros.bin", "r+b");
    if (!arq_bin) {
        // Se o arquivo não existir, cria e inicializa a estrutura
        arq_bin = fopen("livros.bin", "w+b");
        if (!arq_bin) {
            perror("Erro ao criar o arquivo binário");
            return EXIT_FAILURE;
        }
        criar_arvore_vazia(arq_bin); // Cria e inicializa o cabeçalho da árvore binária
    }

    int opcao;
    int codigo;
    char autor[200];
    char titulo[150];
    char nome_arquivo[100];

    do {
        exibir_menu();
        scanf("%d", &opcao);
        getchar(); // Limpar buffer

        switch (opcao) {
            case 1: {
                // Cadastrar novo livro
                Livro novo_livro;
                printf("Digite o código: ");
                scanf("%d", &novo_livro.codigo);
                getchar();

                printf("Digite o título: ");
                fgets(novo_livro.titulo, sizeof(novo_livro.titulo), stdin);
                trim(novo_livro.titulo);

                printf("Digite o autor: ");
                fgets(novo_livro.autor, sizeof(novo_livro.autor), stdin);
                trim(novo_livro.autor);

                printf("Digite a editora: ");
                fgets(novo_livro.editora, sizeof(novo_livro.editora), stdin);
                trim(novo_livro.editora);

                printf("Digite a edição: ");
                scanf("%d", &novo_livro.edicao);

                printf("Digite o ano: ");
                scanf("%d", &novo_livro.ano);

                printf("Digite o preço: ");
                scanf("%f", &novo_livro.preco);

                printf("Digite o estoque: ");
                scanf("%d", &novo_livro.estoque);

                insere(arq_bin, &novo_livro);
                break;
            }
            case 2:
                // Remover livro
                printf("Digite o código do livro a ser removido: ");
                scanf("%d", &codigo);
                remover(arq_bin, codigo);
                break;
            case 3:
                // Imprimir dados do livro por código
                printf("Digite o código do livro: ");
                scanf("%d", &codigo);
                imprimir_dados_livro(arq_bin, codigo);
                break;
            case 4:
                // Listar todos os livros em ordem
                listar_todos(arq_bin);
                break;
            case 5:
                // Imprimir árvore por níveis
                imprimir_por_niveis(arq_bin);
                break;
            case 6:
                // Imprimir árvore formatada como lista
                imprimir_arvore_formatada_como_lista(arq_bin);
                break;
            case 7:
                // Busca por autor
                printf("Digite o nome do autor: ");
                fgets(autor, sizeof(autor), stdin);
                trim(autor);
                busca_por_autor(arq_bin, autor);
                break;
            case 8:
                // Busca por título
                printf("Digite o título do livro: ");
                fgets(titulo, sizeof(titulo), stdin);
                trim(titulo);
                busca_por_titulo(arq_bin, titulo);
                break;
            case 9:
                // Calcular total de livros cadastrados
                calcular_total(arq_bin);
                break;
            case 10:
                // Carregar livros de um arquivo texto
                printf("Digite o nome do arquivo de texto: ");
                fgets(nome_arquivo, sizeof(nome_arquivo), stdin);
                trim(nome_arquivo);
                carregar_arquivo(arq_bin, nome_arquivo);
                break;
            case 11:
                // Imprimir lista de registros livres
                imprimir_lista_livres(arq_bin);
                break;
            case 0:
                printf("Saindo...\n");
                break;
            default:
                printf("Opção inválida!\n");
                break;
        }
    } while (opcao != 0);

    fclose(arq_bin);
    return EXIT_SUCCESS;
}
