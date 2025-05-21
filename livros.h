#ifndef LIVROS_H
#define LIVROS_H

#include <stdio.h>

#define MAX_TITULO 151
#define MAX_AUTOR 201
#define MAX_EDITORA 51

typedef struct {
    int pos_raiz;    // Posição da raiz da árvore binária
    int pos_topo;    // Primeira posição livre no fim do arquivo
    int pos_livre;   // Cabeça da lista de registros livres
} cabecalho;

typedef struct Livro {
    int codigo;
    char titulo[MAX_TITULO];
    char autor[MAX_AUTOR];
    char editora[MAX_EDITORA];
    int edicao;
    int ano;
    float preco;
    int estoque;
} Livro;

typedef struct {
    Livro info;
    int esq;     // Posição do filho à esquerda na árvore
    int dir;     // Posição do filho à direita na árvore
} no;

/* Funções para manipular o cabeçalho do arquivo */
// Propósito - Escreve o cabeçalho no arquivo binário.
// Pré-Condições - O arquivo deve estar aberto para escrita e o cabeçalho deve ser válido.
// Pós-Condições - O cabeçalho é gravado no arquivo.
void escreve_cabecalho(FILE *arq, cabecalho *cab);

// Propósito - Lê o cabeçalho do arquivo binário.
// Pré-Condições - O arquivo deve estar aberto para leitura.
// Pós-Condições - Retorna um ponteiro para o cabeçalho lido do arquivo.
cabecalho* le_cabecalho(FILE *arq);

/* Funções para manipular os nós */
// Propósito - Lê um nó a partir da posição especificada no arquivo binário.
// Pré-Condições - O arquivo deve estar aberto para leitura e a posição deve ser válida.
// Pós-Condições - Retorna um ponteiro para o nó lido do arquivo.
no* ler_no(FILE *arq, int pos);

// Propósito - Escreve um nó na posição especificada do arquivo binário.
// Pré-Condições - O arquivo deve estar aberto para escrita e o nó deve ser válido.
// Pós-Condições - O nó é gravado na posição especificada do arquivo.
void escreve_no(FILE *arq, no* x, int pos);

/* Funções principais */
// Propósito - Remove espaços em branco do início e do fim da string.
// Pré-Condições - A string deve ser alocada e válida.
// Pós-Condições - A string é modificada para remover espaços em branco.
void trim(char *str);

// Propósito - Cria uma árvore binária vazia no arquivo binário.
// Pré-Condições - O arquivo deve estar aberto para escrita.
// Pós-Condições - A árvore binária é inicializada com um cabeçalho apropriado.
void criar_arvore_vazia(FILE *arq_bin);

// Propósito - Insere um livro na árvore binária.
// Pré-Condições - O arquivo deve estar aberto para escrita e o livro deve ser válido.
// Pós-Condições - O livro é inserido na árvore binária.
void insere(FILE *arq, Livro *info);

// Propósito - Remove um livro da árvore binária pelo código.
// Pré-Condições - O arquivo deve estar aberto para escrita e o código deve ser válido.
// Pós-Condições - O livro é removido da árvore binária.
void remover(FILE *arq, int codigo);

// Propósito - Imprime os dados de um livro a partir do código.
// Pré-Condições - O arquivo deve estar aberto para leitura e o código deve ser válido.
// Pós-Condições - Os dados do livro são impressos na saída padrão.
void imprimir_dados_livro(FILE *arq, int codigo);

// Propósito - Lista todos os livros presentes na árvore binária.
// Pré-Condições - O arquivo deve estar aberto para leitura.
// Pós-Condições - Todos os livros são impressos na saída padrão.
void listar_todos(FILE *arq);

// Propósito - Busca livros por autor e imprime os títulos encontrados.
// Pré-Condições - O arquivo deve estar aberto para leitura e o autor deve ser uma string válida.
// Pós-Condições - Os títulos dos livros encontrados são impressos na saída padrão.
void busca_por_autor(FILE *arq, const char *autor);

// Propósito - Busca um livro por título e imprime os detalhes encontrados.
// Pré-Condições - O arquivo deve estar aberto para leitura e o título deve ser uma string válida.
// Pós-Condições - Os detalhes do livro encontrado são impressos na saída padrão.
void busca_por_titulo(FILE *arq, const char *titulo);

// Propósito - Calcula o total de livros na árvore binária.
// Pré-Condições - O arquivo deve estar aberto para leitura.
// Pós-Condições - O total de livros é calculado e impresso na saída padrão.
void calcular_total(FILE *arq);

// Propósito - Carrega dados de um arquivo texto para o arquivo binário.
// Pré-Condições - O arquivo binário deve estar aberto para escrita e o nome do arquivo texto deve ser válido.
// Pós-Condições - Os dados são carregados do arquivo texto e inseridos na árvore binária.
void carregar_arquivo(FILE *arq, const char *nome_arquivo);

// Propósito - Imprime a lista de registros livres no arquivo binário.
// Pré-Condições - O arquivo deve estar aberto para leitura.
// Pós-Condições - A lista de registros livres é impressa na saída padrão
void imprimir_lista_livres(FILE *arq);

// Propósito - Imprime a árvore binária por níveis.
// Pré-Condições - O arquivo deve estar aberto para leitura.
// Pós-Condições - A árvore binária é impressa por níveis na saída padrão.
void imprimir_por_niveis(FILE *arq);

// Propósito - Imprime a árvore binária em formato de lista.
// Pré-Condições - O arquivo deve estar aberto para leitura.
// Pós-Condições - A árvore binária é impressa no formato especificado na saída padrão.
void imprimir_arvore_formatada_como_lista(FILE *arq);

#endif // LIVROS_H
